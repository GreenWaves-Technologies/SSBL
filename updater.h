/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * on 12/20/2019.
 */

#ifndef SSBL_UPDATER_H
#define SSBL_UPDATER_H
#define UPDATER_BUFF_SIZE 1024

static inline pi_err_t update_from_readfs(pi_device_t *flash, const char *binary_path)
{
    pi_err_t rc;
    pi_fs_file_t *file;
    const pi_partition_table_t table;
    const pi_partition_t *ota;
    uint8_t *buff;
    struct pi_device fs;
    struct pi_readfs_conf fs_conf;
    
    PI_LOG_TRC("fac", "Open fs");
    pi_readfs_conf_init(&fs_conf);
    fs_conf.fs.flash = flash;
    pi_open_from_conf(&fs, &fs_conf);
    
    rc = pi_fs_mount(&fs);
    if(rc)
    {
        PI_LOG_ERR("fac", "Error to mount fs");
        return PI_FAIL;
    }
    
    PI_LOG_TRC("fac", "Open file %s", binary_path);
    file = pi_fs_open(&fs, binary_path, 0);
    if(file == NULL)
    {
        PI_LOG_ERR("fac", "Error to open '%s' file", binary_path);
        rc = PI_FAIL;
        goto close_fs_and_return;
    }
    
    PI_LOG_TRC("fac", "Open partition table");
    rc = pi_partition_table_load(flash, &table);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("fac", "Unable to load partition table");
        rc = PI_FAIL;
        goto close_file_and_return;
    }
    
    ota = ota_get_next_ota_partition(table);
    if(ota == NULL)
    {
        PI_LOG_ERR("fac", "Unable to find next update partition");
        rc = PI_FAIL;
        goto close_table_and_return;
    }
    
    PI_LOG_INF("fac", "Next partition subtype %u", ota->subtype);
    PI_LOG_TRC("fac", "Format partition");
    pi_partition_format(ota);
    
    PI_LOG_TRC("fac", "Copy data");
    buff = pi_l2_malloc(UPDATER_BUFF_SIZE);
    if(buff == NULL)
    {
        PI_LOG_ERR("fac", "Unable to allocate buff into l2.");
        rc = PI_ERR_L2_NO_MEM;
        goto close_partition_and_return;
    }
    
    size_t read_size, total_size = 0, i = 0;
    while ((read_size = pi_fs_read(file, buff, UPDATER_BUFF_SIZE)))
    {
        pi_partition_write(ota, i * UPDATER_BUFF_SIZE, buff, read_size);
        total_size += read_size;
        i++;
    }
    PI_LOG_INF("fac", "Transfered %lu bytes to partition.", total_size);
    
    PI_LOG_INF("fac", "Set boot partition.");
    rc = ota_set_boot_partition(table, ota);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("fac", "Unable to set next boot partition.");
        rc = PI_FAIL;
        goto free_and_return;
    }
    
    free_and_return:
    pi_l2_free(buff, UPDATER_BUFF_SIZE);
    close_partition_and_return:
    pi_partition_close(ota);
    close_table_and_return:
    pi_partition_table_free(table);
    close_file_and_return:
    pi_fs_close(file);
    close_fs_and_return:
    pi_fs_unmount(&fs);
    
    return rc;
}

#endif //SSBL_UPDATER_H
