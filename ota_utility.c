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

static PI_L2 ota_state_t ota_states[2];

bool ota_utility_state_is_valid(ota_state_t *state)
{
	if (state->seq == UINT32_MAX)
    {
        return false;
    }
    
    return true;
}

pi_err_t ota_utility_get_ota_state(pi_device_t *flash, const flash_partition_pos_t *ota_data_pos, ota_state_t *ota_state)
{
	struct pi_flash_info flash_info = {0};
	
	pi_flash_ioctl(flash, PI_FLASH_IOCTL_INFO, &flash_info);
	
	pi_flash_read(flash, ota_data_pos->offset, ota_states, sizeof(ota_state_t));
	pi_flash_read(flash, ota_data_pos->offset + flash_info.sector_size, ota_states, sizeof(ota_state_t));
	
	return PI_OK;
}


