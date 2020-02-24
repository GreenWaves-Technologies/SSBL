/*
 * Copyright (C) 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 1/6/2020.
 */

#include "string.h"
#include "stdbool.h"

#include "bsp/bootloader_utility.h"
#include "bsp/ota_utility.h"

pi_err_t bootloader_utility_fill_state(pi_device_t *flash, bootloader_state_t *bs)
{
    pi_err_t rc;
    const flash_partition_table_t *table = NULL;
    const char *partition_usage;
    uint8_t nbr_of_partitions;
    
    rc = flash_partition_table_load(flash, &table, &nbr_of_partitions);
    if(rc != PI_OK)
    {
        SSBL_ERR("Partition table can not be loaded.");
        return PI_FAIL;
    }
    
    SSBL_INF("Partition Table:");
    SSBL_INF("## Label            SSBL usage     Type ST   Offset   Length\n");
    
    for (uint8_t i = 0; i < nbr_of_partitions; i++)
    {
        const flash_partition_info_t *partition = table->partitions + i;
        partition_usage = "unknown";
        
        /* valid partition table */
        switch (partition->type)
        {
            case PI_PARTITION_TYPE_APP:
                switch (partition->subtype)
                {
                    case PI_PARTITION_SUBTYPE_APP_FACTORY:
                        bs->factory = partition->pos;
                        partition_usage = "factory app";
                        break;
                    case PI_PARTITION_SUBTYPE_APP_TEST:
                        bs->test = partition->pos;
                        partition_usage = "test app";
                        break;
                    default:
                        /* OTA binary */
                        if((partition->subtype & ~PART_SUBTYPE_OTA_MASK) == PART_SUBTYPE_OTA_FLAG)
                        {
                            bs->ota[partition->subtype & PART_SUBTYPE_OTA_MASK] = partition->pos;
                            bs->app_count++;
                            partition_usage = "OTA app";
                        } else
                        {
                            partition_usage = "Unknown app";
                        }
                        break;
                }
                break; /* PART_TYPE_APP */
            case PI_PARTITION_TYPE_DATA:
                switch (partition->subtype)
                {
                    case PI_PARTITION_SUBTYPE_DATA_OTA: /* ota data */
                        bs->ota_info = partition->pos;
                        partition_usage = "OTA data";
                        break;
                    default:
                        partition_usage = "Unknown data";
                        break;
                }
                break; /* PARTITION_USAGE_DATA */
            default: /* other partition type */
                break;
        }
        
        SSBL_INF("%2d %-16s %-16s %02x %02x   %08lx %08lx\n",
                 i, partition->label, partition_usage,
                 partition->type, partition->subtype,
                 partition->pos.offset, partition->pos.size);
    }
    
    
    SSBL_INF("End of partition table\n");
    return PI_OK;
}

static uint32_t pad_func_sav[ARCHI_PAD_NB_PADFUNC_REG];
static uint32_t pad_cfg_sav[ARCHI_PAD_NB_PADCFG_REG];

void sav_pad_func_and_cfg()
{
    for (uint32_t i = 0; i < (uint32_t) ARCHI_PAD_NB_PADFUNC_REG; i++)
    {
        pad_func_sav[i] = soc_ctrl_safe_padfun_get(i);
    }
    
    for (uint32_t i = 0; i < (uint32_t) ARCHI_PAD_NB_PADCFG_REG; i++)
    {
        pad_cfg_sav[i] = soc_ctrl_safe_padcfg_get(i);
    }
}

void restore_pad_func_and_cfg()
{
    for (uint32_t i = 0; i < (uint32_t) ARCHI_PAD_NB_PADFUNC_REG; i++)
    {
        hal_pad_set_padfunc(i, pad_func_sav[i]);
    }
    
    for (uint32_t i = 0; i < (uint32_t) ARCHI_PAD_NB_PADCFG_REG; i++)
    {
        soc_ctrl_safe_padcfg_set(i, pad_cfg_sav[i]);
    }
}

static void load_segment(pi_device_t *flash, flash_partition_pos_t *partition_pos, bin_segment_t *segment)
{
    static PI_L2 uint8_t l2_buffer[L2_BUFFER_SIZE];
    
//	int encrypted = conf.info.encrypted;
    
    bool isL2Section = segment->ptr >= 0x1C000000 && segment->ptr < 0x1D000000;
    
    if(isL2Section)
    {
        SSBL_TRC("Load segment to L2 memory at 0x%lX", segment->ptr);
        pi_flash_read(flash, partition_pos->offset + segment->start, (void *) segment->ptr, segment->size);
    } else
    {
        
        SSBL_TRC("Load segment to FC TCDM memory at 0xlX (using a L2 buffer).", segment->ptr);
        size_t remaining_size = segment->size;
        while (remaining_size > 0)
        {
            size_t iter_size = (remaining_size > L2_BUFFER_SIZE) ? L2_BUFFER_SIZE : remaining_size;
            SSBL_TRC("Remaining size 0x%lX, it size %lu", remaining_size, iter_size);
            pi_flash_read(flash, partition_pos->offset + segment->start, l2_buffer, iter_size);
            memcpy((void *) segment->ptr, (void *) l2_buffer, iter_size);
            remaining_size -= iter_size;
        }
    }
    
//	aes_unencrypt(area->ptr, area->size);
}

void bootloader_utility_boot_from_partition(pi_device_t *flash, flash_partition_pos_t *partition_pos)
{
    static PI_L2 bin_desc_t bin_desc;
    static PI_L2 uint8_t buff[0x94];
    
    // Load binary header
    pi_flash_read(flash, partition_pos->offset, &bin_desc, sizeof(bin_desc_t));
    
//    aes_init = 1;
//	aes_unencrypt((unsigned int)&flash_desc, sizeof(flash_desc_t));
    
    SSBL_TRC("App header: nbr of segments %lu, entry point 0x%lx", bin_desc.header.nb_segments,
             bin_desc.header.entry);
    
    for (uint8_t i = 0; i < bin_desc.header.nb_segments; i++)
    {
        bin_segment_t *seg = bin_desc.segments + i;
        SSBL_TRC("Load segment %u: flash offset 0x%lX - size 0x%lX",
                 i, seg->start, seg->size);
        
        // Skip interrupt table entries
        if(seg->ptr == 0x1C000000)
        {
            SSBL_TRC("Skip irq table");
            pi_flash_read(flash, partition_pos->offset + seg->start, (void *) buff, 0x94);
            seg->ptr += 0x94;
            seg->start += 0x94;
            seg->size -= 0x94;
        }
        
        load_segment(flash, partition_pos, seg);
    }
    
    
    SSBL_INF("Close flash");
    pi_flash_close(flash);
    
    // Restore Pad func and config.
    SSBL_INF("Restore PAD configuration");
    restore_pad_func_and_cfg();
    
    SSBL_INF("Disable IRQ to copy IRQ table and jump to app.");
    disable_irq();
    
    uint8_t *ptr = (uint8_t * )
    0x1C000000;
    for (size_t i = 0; i < 0x94; i++)
    {
        ptr[i] = buff[i];
    }
    
    // Flush instruction cache
    SCBC_Type *icache = SCBC;
    icache->ICACHE_FLUSH = 1;

//        SSBL_INF("Boot to app entry point at 0x%lX", bin_desc.header.entry);
    jump_to_address(bin_desc.header.entry);
    
}

int bootloader_utility_get_selected_boot_partition(pi_device_t *flash, const bootloader_state_t *bs)
{
    pi_err_t rc;
    ota_state_t *ota_state = NULL;
    
    if(bs->ota_info.offset == 0)
    {
        SSBL_WNG("OTA info partition not found, try to find bootable partition.");
        if(bs->factory.offset == 0)
        {
            SSBL_ERR("Partition factory not found. Abort bootloader.");
            return INVALID_INDEX;
        } else
        {
            SSBL_INF("Using factory partition to boot.");
            return FACTORY_INDEX;
        }
    }
    
    rc = ota_utility_get_ota_state(flash, &bs->ota_info, ota_state);
    if(rc != PI_OK)
    {
        SSBL_ERR("Unable to read OTA data. Try to boot to factory partition.");
        return bs->factory.offset == 0 ? INVALID_INDEX : INVALID_INDEX;
    }
    
    return FACTORY_INDEX;
}
