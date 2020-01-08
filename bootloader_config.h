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

#ifndef SSBL_BOOTLOADER_CONFIG_H
#define SSBL_BOOTLOADER_CONFIG_H

#include "stdint.h"

#include "pi_partition.h"
#include "pi_flash_partition.h"

typedef struct {
    pi_partition_pos_t ota_info;
    pi_partition_pos_t factory;
    pi_partition_pos_t test;
    pi_partition_pos_t ota[PI_PARTITION_MAX_OTA_SLOTS];
    uint32_t app_count;
    uint32_t selected_subtype;
} bootloader_state_t;

#endif //SSBL_BOOTLOADER_CONFIG_H
