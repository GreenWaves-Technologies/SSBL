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

#include "bsp/crc/md5.h"
#include "pmsis.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash_partition.h"

#include "bootloader_config.h"
#include "bsp/bootloader_utility.h"
#include "partition.h"

/*
 * Global variables
 */


#include "bsp/ota_utility.h"


pi_err_t ota_utility_get_ota_state_from_flash(pi_device_t *flash, ota_state_t *ota_state)
{
    const flash_partition_table_t *table = NULL;
    const flash_partition_info_t *ota_data_partition = NULL;
    pi_err_t rc;
    
    rc = flash_partition_table_load(flash, &table, NULL);
    if(rc != PI_OK)
    {
        return rc;
    }
    
    ota_data_partition = flash_partition_find_first(table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        return PI_ERR_NOT_FOUND;
    }
    
    
    rc = ota_utility_get_ota_state(flash, &ota_data_partition->pos, ota_state);
    
    flash_partition_table_free(table);
    
    return rc;
}

static PI_L2 ota_state_t
ota_states[2];

void ota_utility_compute_md5(const ota_state_t *state, uint8_t *res)
{
    MD5_CTX context;
    MD5_Init(&context);
    
    MD5_Update(&context, &state->seq, sizeof(state->seq));
    MD5_Update(&context, &state->stable_index, sizeof(state->stable_index));
    MD5_Update(&context, &state->uploader_index, sizeof(state->uploader_index));
    MD5_Update(&context, &state->state, sizeof(state->state));
    MD5_Update(&context, &state->data_size, sizeof(state->data_size));
    MD5_Update(&context, &state->update_data, state->data_size);
    
    
    MD5_Final(res, &context);
}

bool ota_utility_state_is_valid(ota_state_t *state)
{
    if(state->seq == UINT32_MAX)
    {
        PI_LOG_WNG("ota", "Check ota state: bad sequence number 0x%lx", state->seq);
        return false;
    }
    
    if(state->data_size > OTA_UPDATE_DATA_SIZE_MAX)
    {
        PI_LOG_WNG("ota", "Check ota state: update data size is too large %lu > %lu", state->data_size,
                   OTA_UPDATE_DATA_SIZE_MAX);
        return false;
    }
    
    uint8_t res[16] = {0};
    ota_utility_compute_md5(state, res);
    int cmp = memcmp(state->md5, res, 16);
    
    if(cmp)
    {
        PI_LOG_WNG("ota", "Check ota state: MD5 differ");
        return false;
    }
    
    return true;
}

pi_err_t
ota_utility_get_ota_state(pi_device_t *flash, const flash_partition_pos_t *ota_data_pos, ota_state_t *ota_state)
{
    struct pi_flash_info flash_info = {0};
    
    pi_flash_ioctl(flash, PI_FLASH_IOCTL_INFO, &flash_info);
    
    pi_flash_read(flash, ota_data_pos->offset, ota_states, sizeof(ota_state_t));
    pi_flash_read(flash, ota_data_pos->offset + flash_info.sector_size, ota_states, sizeof(ota_state_t));
    
    return PI_OK;
}


