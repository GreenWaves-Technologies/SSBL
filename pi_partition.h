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

#ifndef __BSP_PARTITION_H__
#define __BSP_PARTITION_H__

#include "stdint.h"
#include "stdbool.h"

#include "pi_errno.h"
#include "pmsis.h"
#include "bsp/flash.h"

/**
 * @defgroup Partition Partition
 *
 * The partition driver provides support for handling partition
 * contained into a device storage.
 * Currently, PMSIS and GAP SDK provide both partitions, binary firmware and filesystem.
 *
 * - 0 -> Binary firmware;
 * - 1 -> Filesystem or Free space.
 *
 * The beginning  of the filesystem partition is aligned on a flash sector.
 * Thus, an erase partition operation at address zero does not affect the data of the previous partition.
 */

/**
 * @addtogroup Partition
 * @{
 */

/**@{*/

/**
 * @brief Partition type
 * @note Keep this enum in sync with PartitionDefinition class gen_partition.py
 */
typedef enum {
    PI_PARTITION_TYPE_APP = 0x00,       //!< Application partition type
    PI_PARTITION_TYPE_DATA = 0x01,      //!< Data partition type
} pi_partition_type_t;

#define PI_PARTITION_MAX_OTA_SLOTS 16

/**
 * @brief Partition subtype
 * @note Keep this enum in sync with PartitionDefinition class gen_partition.py
 */
typedef enum {
    PI_PARTITION_SUBTYPE_APP_FACTORY = 0x00,                                 //!< Factory application partition
    PI_PARTITION_SUBTYPE_APP_OTA_MIN = 0x10,                                 //!< Base for OTA partition subtypes
    PI_PARTITION_SUBTYPE_APP_OTA_0 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 0,  //!< OTA partition 0
    PI_PARTITION_SUBTYPE_APP_OTA_1 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 1,  //!< OTA partition 1
    PI_PARTITION_SUBTYPE_APP_OTA_2 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 2,  //!< OTA partition 2
    PI_PARTITION_SUBTYPE_APP_OTA_3 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 3,  //!< OTA partition 3
    PI_PARTITION_SUBTYPE_APP_OTA_4 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 4,  //!< OTA partition 4
    PI_PARTITION_SUBTYPE_APP_OTA_5 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 5,  //!< OTA partition 5
    PI_PARTITION_SUBTYPE_APP_OTA_6 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 6,  //!< OTA partition 6
    PI_PARTITION_SUBTYPE_APP_OTA_7 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 7,  //!< OTA partition 7
    PI_PARTITION_SUBTYPE_APP_OTA_8 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 8,  //!< OTA partition 8
    PI_PARTITION_SUBTYPE_APP_OTA_9 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 9,  //!< OTA partition 9
    PI_PARTITION_SUBTYPE_APP_OTA_10 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 10,//!< OTA partition 10
    PI_PARTITION_SUBTYPE_APP_OTA_11 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 11,//!< OTA partition 11
    PI_PARTITION_SUBTYPE_APP_OTA_12 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 12,//!< OTA partition 12
    PI_PARTITION_SUBTYPE_APP_OTA_13 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 13,//!< OTA partition 13
    PI_PARTITION_SUBTYPE_APP_OTA_14 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 14,//!< OTA partition 14
    PI_PARTITION_SUBTYPE_APP_OTA_15 = PI_PARTITION_SUBTYPE_APP_OTA_MIN + 15,//!< OTA partition 15
    PI_PARTITION_SUBTYPE_APP_OTA_MAX = PI_PARTITION_SUBTYPE_APP_OTA_MIN + PI_PARTITION_MAX_OTA_SLOTS,//!< Max subtype of OTA partition
    PI_PARTITION_SUBTYPE_APP_TEST = 0x20,                                    //!< Test application partition

    PI_PARTITION_SUBTYPE_DATA_OTA = 0x00,                                    //!< OTA selection partition
    PI_PARTITION_SUBTYPE_DATA_PHY = 0x01,                                    //!< PHY init data partition

    PI_PARTITION_SUBTYPE_DATA_RAW = 0x80,                                    //!< RAW space partition
    PI_PARTITION_SUBTYPE_DATA_READONLY = 0x81,                               //!< Readonly filesystem partition
    PI_PARTITION_SUBTYPE_DATA_LFS = 0x82,                                    //!< LittleFS filesystem partition

    pi_PARTITION_SUBTYPE_ANY = 0xff,                                         //!< Used to search for partitions with any subtype
} pi_partition_subtype_t;

/**
 * @brief Opaque partition iterator type
 */
typedef struct pi_partition_iterator_opaque_* pi_partition_iterator_t;

/**
 * @brief partition information structure
 */
typedef struct partition {
    struct pi_device *flash;         /*!< Flash device on which the partition resides */
    pi_partition_type_t type;            /*!< partition type (app/data) */
    pi_partition_subtype_t subtype;  /*!< partition subtype */
    uint32_t offset;                 /*!< starting address of the partition in flash */
    uint32_t size;                   /*!< size of the partition, in bytes */
    char label[17];                  /*!< partition label, zero-terminated ASCII string */
    bool encrypted;                  /*!< flag is set to true if partition is encrypted */
    bool read_only;                  /*!< flag is set to true if partition is read only */
} pi_partition_t;

/** @struct pi_partition_conf
 * @brief Partition configuration structure.
 *
 */
struct pi_partition_conf {
    uint8_t id;
    /*!< The partition number: 0 - Firmware binary; 1 - Filesystem or free space. */
    pi_device_t *flash; /*!<
 * The flash device where the partition is stored. */
};

/** @brief Open a partition device.
 *
 * This function must be called before the partition can be used.
 * It will do all the needed configuration to make it usable and initialize
 * the handle used to refer to this opened device when calling other functions.
 *
 * @param device
 * A pointer to the device structure of the device to open.
 *   This structure is allocated by the called and must be kept alive until the
 *   device is closed.
 * @return          0 if the operation is successfull, -1 if there was an error.
 */
int pi_partition_open(struct pi_device *device);

/** @brief Close an opened partition device.
 *
 * This function can be called to close an opened partition device once it is
 * not needed anymore, in order to free all allocated resources. Once this
 * function is called, the device is not accessible anymore and must be opened
 * again before being used.
 *
 * @param device
 * The device structure of the device to close.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_close(struct pi_device *device);

/**
 * @brief Find first partition based on one or more parameters
 *
 * @param type Partition type, one of pi_partition_type_t values
 * @param subtype Partition subtype, one of pi_partition_subtype_t values.
 *                To find all partitions of given type, use
 *                pi_PARTITION_SUBTYPE_ANY.
 * @param label (optional) Partition label. Set this value if looking
 *             for partition with a specific name. Pass NULL otherwise.
 *
 * @return pointer to pi_partition_t structure, or NULL if no partition is found.
 *         This pointer is valid for the lifetime of the application.
 */
const pi_partition_t *
pi_partition_find_first(pi_partition_type_t type, pi_partition_subtype_t subtype, const char *label);

/** @brief Enqueue an asynchronous read copy to the flash partition
 * (from flash partition to processor).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The buffer address of the copy.
 * @param size
 * The size in bytes of the copy.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_read_async(struct pi_device *device, const uint32_t partition_addr,
                                          void *data, const size_t size, pi_task_t *task);

/** @brief Enqueue a read copy to the flash partition (from flash to processor).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The address of the copy in the processor.
 * @param size
 * The size in bytes of the copy
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_read(struct pi_device *device, const uint32_t partition_addr,
                                    void *data, const size_t size);

/** @brief Enqueue an asynchronous read copy to the flash partition
 * (from processor to flash partition).
 *
 * The copy will make a transfer between the flash partition and one of the processor
 * memory areas.
 *
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The buffer address of the copy.
 * @param size
 * The size in bytes of the copy.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_write_async(struct pi_device *device, const uint32_t partition_addr, const void *data,
                                           const size_t size, pi_task_t *task);

/** @brief Enqueue a write copy to the flash partition (from processor to flash).
 *
 * The copy will make a write transfer from one of the processor memory areas
 * to the partition.
 * The locations in the flash being written should have first been erased.
 * The caller is blocked until the transfer is finished.
 * Depending on the chip, there may be some restrictions on the memory which
 * can be used. Check the chip-specific documentation for more details.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the copy.
 * @param partition_addr
 * The address of the copy in the partition.
 * @param data
 * The address of the copy in the processor.
 * @param size
 * The size in bytes of the copy
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int
pi_partition_write(struct pi_device *device, const uint32_t partition_addr, const void *data, const size_t size);

/** @brief Erase an area in the flash partition asynchronously.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @param partition_addr
 * The address of the partition area to be erased.
 * @param size
 * The size of the area to be erased.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
  * @return 0 if the operation is successfull, -1 if there was an error.
*/
static inline int
pi_partition_erase_async(struct pi_device *device, uint32_t partition_addr, int size, pi_task_t *task);

/** @brief Erase an area in the flash partition.
 *
 * This will erase the specified area. The duration of this operation may be
 * long and may be retrieved from the datasheet. If the flash only supports
 * sector erasing, all the sectors partially or entirely covered by this aread
 * will be erased.
 * The caller is blocked until the operation is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @param partition_addr
 * The address of the partition area to be erased.
 * @param size
 * The size of the area to be erased.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_erase(struct pi_device *device, uint32_t partition_addr, int size);

/** @brief Erase the whole flash partition asynchronously.
 *
 * This will erase the entire partition. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * A task must be specified in order to specify how the caller should be
 * notified when the transfer is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @param task
 * The task used to notify the end of transfer.
 * See the documentation of pi_task_t for more details.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_format_async(struct pi_device *device, pi_task_t *task);

/** @brief Erase the whole flash partition.
 *
 * This will erase the entire partition. The duration of this operation may be long
 * and may be retrieved from the datasheet.
 * The caller is blocked until the operation is finished.
 *
 * @param device
 * The device descriptor of the flash partition on which to do the operation.
 * @return 0 if the operation is successfull, -1 if there was an error.
 */
static inline int pi_partition_format(struct pi_device *device);

/** @brief Get the size in byte of the partition
 *
 * @param device
 * The partition where the size will be fetched.
 * @return The size in byte of the partition
 */
size_t pi_partition_get_size(pi_device_t *device);

/** @brief Get flash partition start offset
 *
 * @param device
 * The partition where the offset will be fetched.
 * @return The flash offset in byte where the partition starts.
 */
uint32_t pi_partition_get_flash_offset(pi_device_t *device);

//!@}

/**
 * @} end of Partition
 */

/// @cond IMPLEM

static inline int pi_partition_close(struct pi_device *device)
{
    pi_l2_free(device->data, sizeof(pi_partition_t));
    return 0;
}

#define CHECK_ADDR() if (partition_addr + size > partition->size) return -1

static inline int pi_partition_read_async(struct pi_device *device, const uint32_t partition_addr,
                                          void *data, const size_t size, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    CHECK_ADDR();
    pi_flash_read_async(conf->flash, partition_addr + partition->offset, data, size, task);
    return 0;
}

static inline int pi_partition_read(struct pi_device *device, const uint32_t partition_addr,
                                    void *data, const size_t size)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_read_async(device, partition_addr, data, size, &task);
    if (rc < 0)
        return rc;
    pi_task_wait_on(&task);
    return 0;
}

static inline int pi_partition_write_async(struct pi_device *device, const uint32_t partition_addr, const void *data,
                                           const size_t size, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    CHECK_ADDR();
    pi_flash_program_async(conf->flash, partition_addr + partition->offset, data, size, task);
    return 0;
}

static inline int
pi_partition_write(struct pi_device *device, const uint32_t partition_addr, const void *data, const size_t size)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_write_async(device, partition_addr, data, size, &task);
    if (rc < 0)
        return rc;
    pi_task_wait_on(&task);
    return 0;
}

static inline int pi_partition_erase_async(struct pi_device *device, uint32_t partition_addr, int size, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    CHECK_ADDR();
    pi_flash_erase_async(conf->flash, partition_addr + partition->offset, size, task);
    return 0;
}

static inline int pi_partition_erase(struct pi_device *device, uint32_t partition_addr, int size)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_erase_async(device, partition_addr, size, &task);
    if (rc < 0)
        return rc;
    pi_task_wait_on(&task);
    return 0;
}

static inline int pi_partition_erase_partition_async(struct pi_device *device, pi_task_t *task)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;

    return pi_partition_erase_async(device, 0, partition->size, task);
}

static inline int pi_partition_erase_partition(struct pi_device *device)
{
    int rc;
    pi_task_t task;

    pi_task_block(&task);
    rc = pi_partition_erase_partition_async(device, &task);
    if (rc < 0)
        return 0;
    pi_task_wait_on(&task);
    return 0;
}

size_t pi_partition_get_size(pi_device_t *device);

uint32_t pi_partition_get_flash_offset(pi_device_t *device);

/// @endcond

#endif
