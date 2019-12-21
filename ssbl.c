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
#include "string.h"
#include "stdint.h"

#include "pmsis.h"
#include "pmsis/rtos/pmsis_assert.h"
#include "bsp/partition.h"
#include "bsp/flash/hyperflash.h"

#include "traces.h"
#include "partition.h"

/*
 * Global variables
 */

static pi_device_t flash;
static struct pi_hyperflash_conf flash_conf;

void test_write_partition(pi_device_t *part)
{
    uint8_t *data;
    uint8_t *data2;
    const uint32_t addr = 0x40000;

    puts("\nTest write partition");

    pi_assert(
            (data = pi_l2_malloc(8)));
    pi_assert(
            (data2 = pi_l2_malloc(8)));

    // Initialize data
    printf("data: ");
    for (uint8_t i = 0; i < 8; i++)
    {
        data[i] = i;
        printf("%u ", data[i]);
    }
    puts("");

    // Erase sector
    printf("Erase sector at 0x%lx\n", addr);
    pi_assert(
            pi_partition_erase(part, addr, 8) >= 0);

    // Write data
    printf("Write data at 0x%lx\n", addr);
    pi_assert(
            pi_partition_write(part, addr, data, 8) >= 0);

    // Read partition for check writing operation
    puts("Reads the data written and check their integrity");
    pi_assert(
            pi_partition_read(part, addr, data2, 8) >= 0);

    for (uint8_t i = 0; i < 8; i++)
    {
        pi_assert(data[i] == data2[i]);
    }

    pi_l2_free(data, 8);
    pi_l2_free(data2, 8);

    puts("Test write artition done");
}

void test_read_partition(pi_device_t *part)
{
    int8_t rc;
    char *buff;
    const size_t buff_size = 16;
    const char *pattern = "FS in partition";

    printf("\nTest read partition 1\n"
           "Check pattern `%s' at 0x\n", pattern);

    pi_assert(
            buff = pi_l2_malloc(buff_size));
    pi_assert(
            pi_partition_read(part, 0, buff, buff_size) >= 0);

    buff[buff_size - 1] = '\0';
    printf("Read `%s'\n", buff);

    pi_assert(
            strcmp(pattern, buff) == 0);

    pi_l2_free(buff, buff_size);

    puts("Test read partition 1 done");
}

void open_partition(pi_device_t *part, struct pi_partition_conf *conf, uint8_t id, pi_device_t *flash)
{
    printf("Open partition %u\n", id);

    conf->id = id;
    conf->flash = flash;
    part->config = conf;
    pi_assert(
            pi_partition_open(part) >= 0);
}

void open_flash(pi_device_t *flash, struct pi_hyperflash_conf *flash_conf)
{
    pi_hyperflash_conf_init(flash_conf);
    pi_open_from_conf(flash, flash_conf);
    pi_assert(
            pi_flash_open(flash) >= 0
    );
}

void ssbl(void)
{
    SSBL_TRACE("GAP Second Stage Boot Loader start");

    SSBL_TRACE("Open flash...");
    open_flash(&flash, &flash_conf);
    SSBL_TRACE("Open flash done.");

    test_partition();

    pi_flash_close(&flash);

    SSBL_TRACE("Second Stage Boot Loader exit!");
    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    return pmsis_kickoff((void *) ssbl);
}
