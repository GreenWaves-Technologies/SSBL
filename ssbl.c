/*
 * Copyright (C) 2018 GreenWaves Technologies
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
 * Authors: Mathieu Barbe, GreenWaves Technologies (mathieu.barbe@greenwaves-technologies.com)
 */

#include "stdio.h"
#include "stdint.h"

#include "pmsis.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash_partition.h"

#include "bootloader_config.h"
#include "bsp/bootloader_utility.h"
#include "partition.h"
#include "bsp/ota_utility.h"

/*
 * Global variables
 */

static pi_device_t flash;
static struct pi_hyperflash_conf flash_conf;


void open_flash(pi_device_t *flash, struct pi_hyperflash_conf *flash_conf)
{
    pi_hyperflash_conf_init(flash_conf);
    pi_open_from_conf(flash, flash_conf);

//    if (conf.info3.flash_type) blockSize = HYPER_FLASH_BLOCK_SIZE;
    
    if(pi_flash_open(flash) < 0)
    {
        SSBL_ERR("unable to open flash device");
        pmsis_exit(PI_FAIL);
    }
    
}

void ota_state_is_valid_test()
{
    ota_state_t state;
    
    printf("Whole struct is set to 0xFF\n");
    memset(&state, 0xFF, sizeof(ota_state_t));
    bool ok = ota_utility_state_is_valid(&state);
    printf("state is %s\n", ok ? "ok" : "bad");
    
    printf("seq --\n");
    state.seq--;
    ok = ota_utility_state_is_valid(&state);
    printf("state is %s\n", ok ? "ok" : "bad");
    
    printf("data size = 0\n");
    state.data_size = 0;
    ok = ota_utility_state_is_valid(&state);
    printf("state is %s\n", ok ? "ok" : "bad");
    
    printf("compute md5\n");
    ota_utility_compute_md5(&state, state.md5);
    ok = ota_utility_state_is_valid(&state);
    printf("state is %s\n", ok ? "ok" : "bad");
    
    printf("Add data\n");
    state.data_size = 4;
    state.update_data = (void *) 42;
    ota_utility_compute_md5(&state, state.md5);
    ok = ota_utility_state_is_valid(&state);
    printf("state is %s\n", ok ? "ok" : "bad");
    printf("data read %lu\n", (uint32_t) state.update_data);
    
    
    exit(0);
}

void boot_to_flash_app(pi_device_t *flash)
{
    pi_err_t rc;
    bootloader_state_t bs;
    const flash_partition_table_t *table;
    
    SSBL_INF("Load partition table from flash");
    rc = flash_partition_table_load(flash, &table, NULL);
    if (rc != PI_OK)
    {
        SSBL_ERR("Unable to load partition table from flash (error %d). Abort bootloader.", rc);
        // Todo maybe restart.
        pmsis_exit(PI_ERR_NOT_FOUND);
    }
    
    rc = bootloader_utility_fill_state(table, &bs);
    if(rc != PI_OK)
    {
        SSBL_ERR("Unable to fill bootloader state from partition table. Abort bootloader.");
        // todo restart
        pmsis_exit(PI_ERR_NOT_FOUND);
    }
    
    pi_partition_subtype_t boot_type = bootloader_utility_get_boot_partition(table, &bs);
    flash_partition_table_free(table);
    
    if(boot_type == PI_PARTITION_SUBTYPE_APP_FACTORY)
    {
        SSBL_INF("Boot to factory partition:");
        bootloader_utility_boot_from_partition(flash, bs.factory.offset);
    }
    
    SSBL_ERR("Unable to boot to an app.");
    pmsis_exit(PI_FAIL);
}

void ssbl(void)
{
    SSBL_INF("GAP Second Stage Boot Loader start");
    
    SSBL_INF("Open flash...");
    open_flash(&flash, &flash_conf);
    SSBL_INF("Open flash done.");
    
    boot_to_flash_app(&flash);
    
    pi_flash_close(&flash);
    
    SSBL_INF("Second Stage Boot Loader exit!");
    pmsis_exit(0);
}


int main(void)
{
    
    sav_pad_func_and_cfg();
    
    return pmsis_kickoff((void *) ssbl);
}
