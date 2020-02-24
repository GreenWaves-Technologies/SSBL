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

#ifndef OTA_UTILITY_H
#define OTA_UTILITY_H

#include "stdint.h"
#include "pmsis.h"
#include "bsp/partition.h"
#include "bsp/flash_partition.h"

/// OTA_DATA states for checking operability of the app.
typedef enum {
	ESP_OTA_IMG_NEW             = 0x0U,         /*!< Monitor the first boot. In bootloader this state is changed to ESP_OTA_IMG_PENDING_VERIFY. */
	ESP_OTA_IMG_PENDING_VERIFY  = 0x1U,         /*!< First boot for this app was. If while the second boot this state is then it will be changed to ABORTED. */
	ESP_OTA_IMG_VALID           = 0x2U,         /*!< App was confirmed as workable. App can boot and work without limits. */
	ESP_OTA_IMG_INVALID         = 0x3U,         /*!< App was confirmed as non-workable. This app will not selected to boot at all. */
	ESP_OTA_IMG_ABORTED         = 0x4U,         /*!< App could not confirm the workable or non-workable. In bootloader IMG_PENDING_VERIFY state will be changed to IMG_ABORTED. This app will not selected to boot at all. */
	ESP_OTA_IMG_UNDEFINED       = 0xFFFFFFFFU,  /*!< Undefined. App can boot and work without limits. */
} ota_img_states_t;

typedef struct {
	uint32_t seq;
	uint8_t stable_index;
	uint8_t once_index;
	uint8_t once_state;
	uint32_t md5;
} ota_state_t;

pi_err_t ota_utility_get_ota_state_from_flash(pi_device_t *flash, ota_state_t *ota_state);


pi_err_t ota_utility_get_ota_state(pi_device_t *flash, const flash_partition_pos_t *ota_data_pos, ota_state_t *ota_state);

#endif //OTA_UTILITY_H

