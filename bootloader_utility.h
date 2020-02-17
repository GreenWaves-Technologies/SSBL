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

#ifndef SSBL_BOOTLOADER_UTILITY_H
#define SSBL_BOOTLOADER_UTILITY_H

#include "stdbool.h"
#include "stdint.h"

#include "pmsis.h"
#include "bsp/flash_partition.h"
#include "bsp/partition.h"

#define MAX_NB_SEGMENT 16

typedef struct {
	uint32_t start;
	uint32_t ptr;
	uint32_t size;
} bin_segment_t;

typedef struct {
	uint32_t nb_segments;
	uint32_t entry;
} bin_header_t;

typedef struct {
	bin_header_t header;
	bin_segment_t segments[MAX_NB_SEGMENT];
	uint32_t crc;
} bin_desc_t;

typedef struct {
    flash_partition_pos_t ota_info;
    flash_partition_pos_t factory;
    flash_partition_pos_t test;
    flash_partition_pos_t ota[PI_PARTITION_MAX_OTA_SLOTS];
    uint32_t app_count;
    uint32_t selected_subtype;
} bootloader_state_t;

pi_err_t bootloader_utility_fill_state(pi_device_t *flash, bootloader_state_t *bs);
void bootloader_utility_boot_from_partition(pi_device_t *flash, flash_partition_pos_t *partition_pos);

static inline void __attribute__((noreturn)) jump_to_address(unsigned int address) {
    void (*entry)() = (void (*)())(address);
    entry();
    while(1);
}


#endif //SSBL_BOOTLOADER_UTILITY_H
