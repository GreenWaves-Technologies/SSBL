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

#include "string.h"
#include "stdio.h"

#include "pmsis.h"
#include "pi_errno.h"
#include "bsp/bsp.h"
#include "bsp/flash.h"
//Todo back to bsp version
//#include "bsp/partition.h"
#include "pi_partition.h"
#include "pi_flash_partition.h"

static pi_device_api_t partition_api = {
        .open = pi_partition_open,
        .close = pi_partition_close,
        .open_async = NULL,
        .close_async = NULL,
        .read = NULL,
        .write = NULL,
        .ioctl = NULL,
        .ioctl_async = NULL,
        .specific_api = NULL,
};

static const pi_partition_info_t *partition_table = NULL;
static pmsis_mutex_t *partition_table_mutex;

int pi_partition_open(struct pi_device *device)
{
    pi_partition_t *partition;
    uint32_t *p1_offset;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    // At this time, only two partitions are availables on the flash,
    if (conf->id >= 2) return -1;

    p1_offset = pi_l2_malloc(sizeof(*p1_offset));
    if (p1_offset == NULL) return -1;
    // Read partition 1 offset
    pi_flash_read(conf->flash, 0, p1_offset, 4);

    device->data = pi_l2_malloc(sizeof(pi_partition_t));
    if (device->data == NULL) goto table_error;
    partition = (pi_partition_t *) device->data;

    partition->flash = conf->flash;
    if (conf->id == 0)
    {
        partition->offset = 4;
        partition->size = *p1_offset - partition->offset;
    } else
    {
        partition->offset = *p1_offset;
        partition->size = (1 << 26) - partition->offset;
        // todo fetch flash size from flash ioctl.
    }

    device->api = &partition_api;

    pi_l2_free(p1_offset, sizeof(*p1_offset));
    return 0;

    table_error:
    pi_l2_free(p1_offset, sizeof(*p1_offset));
    return -1;
}

size_t pi_partition_get_size(pi_device_t *device)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    if (partition)
        return partition->size;
    else
        return 0;
}

uint32_t pi_partition_get_flash_offset(pi_device_t *device)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    if (partition)
        return partition->offset;
    else
        return UINT32_MAX;
}
// Todo : add static attribut or add into header file.
void print_partition_table(const pi_partition_info_t *table)
{
    if (table == NULL)
    {
        printf("No partition table\n");
        return;
    }

    printf("## Label \t   Type Sub Type Offset Length\n");

    for (uint8_t i = 0;
         table[i].magic_bytes == PI_PARTITION_MAGIC;
         i++)
    {
        printf("%02d %-16s %02x %02x %08lx %08lx\n",
               i, table[i].label,
               table[i].type, table[i].subtype,
               table[i].pos.offset, table[i].pos.size);
    }
}

static pi_err_t ensure_partitions_loaded(pi_device_t *flash)
{
    pi_err_t rc;

    if (partition_table)
        return PI_OK;

    //    Check if partition table mutex is initialized
    if (!partition_table_mutex)
    {
        int irq_enabled;
        hal_compiler_barrier();
        irq_enabled = disable_irq();
        hal_compiler_barrier();
        partition_table_mutex = pmsis_l2_malloc(sizeof(pmsis_mutex_t));
        if (partition_table_mutex == NULL)
            return PI_ERR_NO_MEM;
        if (pmsis_mutex_init(partition_table_mutex))
            return PI_FAIL;
        restore_irq(irq_enabled);
        hal_compiler_barrier();
    }

    // only lock if the partition table is empty (and check again after acquiring lock)
    pmsis_mutex_take(partition_table_mutex);
    if (partition_table)
    {
        pmsis_mutex_release(partition_table_mutex);
        return PI_OK;
    }

    rc = pi_partition_table_load(flash, &partition_table, NULL);
    pmsis_mutex_release(partition_table_mutex);

    print_partition_table(partition_table);

    return rc;
}

const pi_partition_info_t *_pi_partition_find_first(pi_partition_type_t type,
                                              pi_partition_subtype_t subtype, const char *label)
{
    const pi_partition_info_t *part;

    if (partition_table == NULL)
    {
        return NULL;
    }

    for (part = partition_table; part->magic_bytes != PI_PARTITION_MAGIC; part++)
    {
        if (part->type != type || part->subtype != subtype)
            continue;
        if (label == NULL)
            break;
        if (strncmp(label, (char *) &part->label, PI_PARTITION_LABEL_LENGTH))
            continue;
    }
    return part;
}


void test(pi_device_t *flash)
{
    printf("Test pi_partition\n");
    ensure_partitions_loaded(flash);
//    pi_partition_info_t *info;
//    info = _pi_partition_find_first(PI_PARTITION_TYPE_APP, PI_PARTITION_SUBTYPE_APP_FACTORY, NULL);
//    if (info == NULL) {
//        puts("test error");
//    }

}
