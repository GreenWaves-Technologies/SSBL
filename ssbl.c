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
#include "pi_errno.h"
#include "bsp/flash/hyperflash.h"
#include "bootloader_utility.h"

#include "traces.h"
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

    if (pi_flash_open(flash) < 0)
    {
        SSBL_TRACE("Error: unable to open flash device");
        pmsis_exit(PI_FAIL);
    }

}

void boot_to_flash_app(pi_device_t *flash)
{
    bootloader_state_t bs = {0};

    SSBL_TRACE("Boot to flash app");

    SSBL_TRACE("Load partition table from flash");
    if (!bootloader_utility_load_partition_table(flash, &bs))
    {
        SSBL_TRACE("Error to find bootable partition.");
        pmsis_exit(PI_FAIL);
    }
}

void ssbl(void)
{
    SSBL_TRACE("GAP Second Stage Boot Loader start");

    SSBL_TRACE("Open flash...");
    open_flash(&flash, &flash_conf);
    SSBL_TRACE("Open flash done.");

    boot_to_flash_app(&flash);

    pi_flash_close(&flash);

    SSBL_TRACE("Second Stage Boot Loader exit!");
    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    return pmsis_kickoff((void *) ssbl);
}
