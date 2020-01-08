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

#include "pi_errno.h"
#include "pi_flash_partition.h"
#include "bootloader_utility.h"

extern void print_partition_table(const pi_partition_info_t *table);

bool bootloader_utility_load_partition_table(pi_device_t *flash, bootloader_state_t *bs)
{
    const pi_partition_info_t *table = NULL;
    const char *partition_usage;
    pi_err_t rc;
    uint8_t nbr_of_partitions;

    rc = pi_partition_table_load(flash, &table, &nbr_of_partitions);
    if (rc != PI_OK)
    {
        puts("Failed to verify partition table");
        return false;
    }

    puts("Partition Table:");
    printf("## Label            SSBL usage     Type ST   Offset   Length\n");

    for (uint8_t i = 0; i < nbr_of_partitions; i++)
    {
        const pi_partition_info_t *partition = table + i;
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

        printf("%2d %-16s %-16s %02x %02x   %08lx %08lx\n",
               i, partition->label, partition_usage,
               partition->type, partition->subtype,
               partition->pos.offset, partition->pos.size);
    }


    printf("End of partition table\n");
    return true;
}
