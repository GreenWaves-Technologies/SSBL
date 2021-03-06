/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License\n");
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
 * Authors: Mathieu Barbe, GreenWaves Technologies (mathieu.barbe@greenwaves-technologies.com)
 */

#include "stdio.h"
#include "stdint.h"

#include "pmsis.h"
#if defined(QSPI)
#define FLASH_NAME "QSPI"
#include "bsp/flash/spiflash.h"
#else
#define FLASH_NAME "HYPER"
#include "bsp/flash/hyperflash.h"
#endif
#include "bsp/flash_partition.h"

#include "bsp/bootloader_utility.h"
#include "bsp/ota_utility.h"

/*
 * Global variables
 */

static pi_device_t flash;

#if defined(QSPI)
static struct pi_spiflash_conf flash_conf;
#else
static struct pi_hyperflash_conf flash_conf;
#endif


void open_flash(pi_device_t *flash)
{
#if defined(QSPI)
    pi_spiflash_conf_init(&flash_conf);
#else
    pi_hyperflash_conf_init(&flash_conf);
#endif
    pi_open_from_conf(flash, &flash_conf);

//    if (conf.info3.flash_type) blockSize = HYPER_FLASH_BLOCK_SIZE;
    
    if(pi_flash_open(flash) < 0)
    {
        SSBL_ERR("unable to open flash device\n");
        pmsis_exit(PI_FAIL);
    }
    
}

void ota_state_is_valid_test()
{
    ota_state_t state;
    
    SSBL_INF("Whole struct is set to 0xFF\n\n");
    memset(&state, 0xFF, sizeof(ota_state_t));
    bool ok = ota_utility_state_is_valid(&state);
    SSBL_INF("state is %s\n", ok ? "ok" : "bad\n");
    
    SSBL_INF("seq --\n\n");
    state.seq--;
    ok = ota_utility_state_is_valid(&state);
    SSBL_INF("state is %s\n", ok ? "ok" : "bad\n");
    
    SSBL_INF("compute md5\n\n");
    ota_utility_compute_md5(&state, state.md5);
    ok = ota_utility_state_is_valid(&state);
    SSBL_INF("state is %s\n", ok ? "ok" : "bad\n");
    
    exit(0);
}

void boot_to_flash_app(pi_device_t *flash)
{
    pi_err_t rc;
    bootloader_state_t bs;
    const flash_partition_table_t *table;
    pi_partition_subtype_t boot_type;
    const flash_partition_info_t *boot_partition;
    
    SSBL_INF("Load partition table from flash\n");
    rc = flash_partition_table_load(flash, &table, NULL);
    if(rc != PI_OK)
    {
        SSBL_ERR("Unable to load partition table from flash (error %d). Abort bootloader.\n", rc);
        // Todo maybe restart.
        pmsis_exit(PI_ERR_NOT_FOUND);
    }
    
    rc = bootloader_utility_fill_state(table, &bs);
    if(rc != PI_OK)
    {
        SSBL_ERR("Unable to fill bootloader state from partition table. Abort bootloader.\n");
        // todo restart
        pmsis_exit(PI_ERR_NOT_FOUND);
    }
    
    flash_partition_print_partition_table(table);

    boot_type = bootloader_utility_get_boot_partition(table, &bs);
    
    if(boot_type == PI_PARTITION_SUBTYPE_UNKNOWN)
    {
        SSBL_ERR("Unable to boot to an app.\n");
        flash_partition_table_free(table);
        pmsis_exit(PI_FAIL);
    }
    
    boot_partition = flash_partition_find_first(table, PI_PARTITION_TYPE_APP, boot_type, NULL);
    if (boot_partition == NULL)
    {
        SSBL_ERR("Unable to load partition subtype 0x%x to boot\n", boot_type);
        flash_partition_table_free(table);
        pmsis_exit(PI_FAIL);
    }
    SSBL_INF("Boot to partition subtype 0x%x at offset 0x%x\n", boot_partition->subtype, boot_partition->pos.offset);
    bootloader_utility_boot_from_partition(flash, boot_partition->pos.offset);
    
    flash_partition_table_free(table);
}

void ssbl(void)
{
    SSBL_INF("GAP Second Stage Boot Loader start\n");
    
    SSBL_INF("Open flash...\n");
    open_flash(&flash);
    SSBL_INF("Open flash done.\n");
    
    boot_to_flash_app(&flash);
    
    pi_flash_close(&flash);
    
    SSBL_INF("Second Stage Boot Loader exit!\n");
    pmsis_exit(0);
}


int main(void)
{
    SSBL_INF("\n\t SSBL %s Version\n\n", FLASH_NAME);
    return pmsis_kickoff((void *) ssbl);
}
