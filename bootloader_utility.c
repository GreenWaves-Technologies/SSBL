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

#include "bootloader_utility.h"


pi_err_t bootloader_utility_fill_state(pi_device_t *flash, bootloader_state_t *bs)
{
    pi_err_t rc;
    const flash_partition_table_t *table = NULL;
    const char *partition_usage;
    uint8_t nbr_of_partitions;

    rc = flash_partition_table_load(flash, &table, &nbr_of_partitions);
    if (rc != PI_OK)
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
                        if ((partition->subtype & ~PART_SUBTYPE_OTA_MASK) == PART_SUBTYPE_OTA_FLAG)
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

void bootloader_utility_boot_from_partition(pi_device_t *flash, flash_partition_pos_t *partition_pos)
{
	static PI_L2 bin_desc_t bin_desc;
	bin_header_t *header = NULL;
	
	memset(&bin_desc, 0, sizeof(bin_desc_t));
	pi_flash_read(flash, partition_pos->offset, &bin_desc, sizeof(bin_desc_t));
	
	header = &bin_desc.header;
	SSBL_DBG("Nbr of segments: %ld", header->nb_segments);
	
//    aes_init = 1;
    
}
