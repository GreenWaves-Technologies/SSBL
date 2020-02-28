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

#ifndef OTA_H
#define OTA_H

#include "stdint.h"
#include "pmsis.h"
#include "bsp/partition.h"
#include "bsp/flash_partition.h"

const pi_partition_t *ota_get_next_ota_partition(const pi_partition_table_t table);

/**
 * @brief This function is called to indicate that the running app is working well.
 *
 * @return
 *  - PI_OK: if successful.
 */
pi_err_t ota_mark_app_valid_cancel_rollback(void);


/**
 * @brief This function is called to roll back to the previously workable app with reboot.
 *
 * If rollback is successful then device will reset else API will return with error code.
 * Checks applications on a flash drive that can be booted in case of rollback.
 * If the flash does not have at least one app (except the running app) then rollback is not possible.
 * @return
 *  - pi_FAIL: if not successful.
 */
pi_err_t ota_mark_app_invalid_rollback_and_reboot(void);


/**
 * @brief Configure OTA data for a new boot partition
 *
 * @note If this function returns PI_OK, calling pi_restart() will boot the newly configured app partition.
 *
 * @param table Table where ota data can be fetched.
 * @param partition Pointer to info for partition containing app image to boot.
 *
 * @return
 *    - PI_OK: OTA data updated, next reboot will use specified partition.
 *    - PI_ERR_INVALID_ARG: partition argument was NULL or didn't point to a valid OTA partition of type "app".
 *    - PI_ERR_OTA_VALIDATE_FAILED: Partition contained invalid app image.
 *    - PI_ERR_NOT_FOUND: OTA data partition not found.
 */
pi_err_t ota_set_boot_partition(const pi_partition_table_t table, const pi_partition_t *partition);


#endif //OTA_H

