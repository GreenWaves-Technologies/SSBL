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
#include "bootloader_utility.h"
#include "partition.h"

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

void boot_to_flash_app(pi_device_t *flash)
{
    pi_err_t rc;
    bootloader_state_t bs = {0};
    
    SSBL_INF("Boot to flash app");
    
    SSBL_INF("Load partition table from flash");
    rc = bootloader_utility_fill_state(flash, &bs);
    if(rc != PI_OK)
    {
        SSBL_ERR("bootable partition not found.");
        pmsis_exit(PI_FAIL);
    }
    
    bootloader_utility_boot_from_partition(flash, &bs.factory);
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
