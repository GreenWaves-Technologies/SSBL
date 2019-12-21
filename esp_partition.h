// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __pi_PARTITION_H__
#define __pi_PARTITION_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pi_err.h"
#include "pi_flash.h"
#include "pi_spi_flash.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pi_partition.h
 * @brief Partition APIs
 */


/**
 * @brief Partition type
 * @note Keep this enum in sync with PartitionDefinition class gen_esp32part.py
 */
typedef enum {
    pi_PARTITION_TYPE_APP = 0x00,       //!< Application partition type
    pi_PARTITION_TYPE_DATA = 0x01,      //!< Data partition type
} pi_partition_type_t;

/**
 * @brief Partition subtype
 * @note Keep this enum in sync with PartitionDefinition class gen_esp32part.py
 */
typedef enum {
    pi_PARTITION_SUBTYPE_APP_FACTORY = 0x00,                                 //!< Factory application partition
    pi_PARTITION_SUBTYPE_APP_OTA_MIN = 0x10,                                 //!< Base for OTA partition subtypes
    pi_PARTITION_SUBTYPE_APP_OTA_0 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 0,  //!< OTA partition 0
    pi_PARTITION_SUBTYPE_APP_OTA_1 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 1,  //!< OTA partition 1
    pi_PARTITION_SUBTYPE_APP_OTA_2 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 2,  //!< OTA partition 2
    pi_PARTITION_SUBTYPE_APP_OTA_3 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 3,  //!< OTA partition 3
    pi_PARTITION_SUBTYPE_APP_OTA_4 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 4,  //!< OTA partition 4
    pi_PARTITION_SUBTYPE_APP_OTA_5 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 5,  //!< OTA partition 5
    pi_PARTITION_SUBTYPE_APP_OTA_6 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 6,  //!< OTA partition 6
    pi_PARTITION_SUBTYPE_APP_OTA_7 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 7,  //!< OTA partition 7
    pi_PARTITION_SUBTYPE_APP_OTA_8 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 8,  //!< OTA partition 8
    pi_PARTITION_SUBTYPE_APP_OTA_9 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 9,  //!< OTA partition 9
    pi_PARTITION_SUBTYPE_APP_OTA_10 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 10,//!< OTA partition 10
    pi_PARTITION_SUBTYPE_APP_OTA_11 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 11,//!< OTA partition 11
    pi_PARTITION_SUBTYPE_APP_OTA_12 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 12,//!< OTA partition 12
    pi_PARTITION_SUBTYPE_APP_OTA_13 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 13,//!< OTA partition 13
    pi_PARTITION_SUBTYPE_APP_OTA_14 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 14,//!< OTA partition 14
    pi_PARTITION_SUBTYPE_APP_OTA_15 = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 15,//!< OTA partition 15
    pi_PARTITION_SUBTYPE_APP_OTA_MAX = pi_PARTITION_SUBTYPE_APP_OTA_MIN + 16,//!< Max subtype of OTA partition
    pi_PARTITION_SUBTYPE_APP_TEST = 0x20,                                    //!< Test application partition

    pi_PARTITION_SUBTYPE_DATA_OTA = 0x00,                                    //!< OTA selection partition
    pi_PARTITION_SUBTYPE_DATA_PHY = 0x01,                                    //!< PHY init data partition
    pi_PARTITION_SUBTYPE_DATA_NVS = 0x02,                                    //!< NVS partition
    pi_PARTITION_SUBTYPE_DATA_COREDUMP = 0x03,                               //!< COREDUMP partition
    pi_PARTITION_SUBTYPE_DATA_NVS_KEYS = 0x04,                               //!< Partition for NVS keys
    pi_PARTITION_SUBTYPE_DATA_EFUSE_EM = 0x05,                               //!< Partition for emulate eFuse bits

    pi_PARTITION_SUBTYPE_DATA_ESPHTTPD = 0x80,                               //!< ESPHTTPD partition
    pi_PARTITION_SUBTYPE_DATA_FAT = 0x81,                                    //!< FAT partition
    pi_PARTITION_SUBTYPE_DATA_SPIFFS = 0x82,                                 //!< SPIFFS partition

    pi_PARTITION_SUBTYPE_ANY = 0xff,                                         //!< Used to search for partitions with any subtype
} pi_partition_subtype_t;

/**
 * @brief Convenience macro to get pi_partition_subtype_t value for the i-th OTA partition
 */
#define pi_PARTITION_SUBTYPE_OTA(i) ((pi_partition_subtype_t)(pi_PARTITION_SUBTYPE_APP_OTA_MIN + ((i) & 0xf)))

/**
 * @brief Opaque partition iterator type
 */
typedef struct pi_partition_iterator_opaque_* pi_partition_iterator_t;

/**
 * @brief partition information structure
 *
 * This is not the format in flash, that format is pi_partition_info_t.
 *
 * However, this is the format used by this API.
 */
typedef struct {
    pi_flash_t* flash_chip;            /*!< SPI flash chip on which the partition resides */
    pi_partition_type_t type;          /*!< partition type (app/data) */
    pi_partition_subtype_t subtype;    /*!< partition subtype */
    uint32_t address;                   /*!< starting address of the partition in flash */
    uint32_t size;                      /*!< size of the partition, in bytes */
    char label[17];                     /*!< partition label, zero-terminated ASCII string */
    bool encrypted;                     /*!< flag is set to true if partition is encrypted */
} pi_partition_t;

/**
 * @brief Find partition based on one or more parameters
 *
 * @param type Partition type, one of pi_partition_type_t values
 * @param subtype Partition subtype, one of pi_partition_subtype_t values.
 *                To find all partitions of given type, use
 *                pi_PARTITION_SUBTYPE_ANY.
 * @param label (optional) Partition label. Set this value if looking
 *             for partition with a specific name. Pass NULL otherwise.
 *
 * @return iterator which can be used to enumerate all the partitions found,
 *         or NULL if no partitions were found.
 *         Iterator obtained through this function has to be released
 *         using pi_partition_iterator_release when not used any more.
 */
pi_partition_iterator_t pi_partition_find(pi_partition_type_t type, pi_partition_subtype_t subtype, const char* label);

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
const pi_partition_t* pi_partition_find_first(pi_partition_type_t type, pi_partition_subtype_t subtype, const char* label);

/**
 * @brief Get pi_partition_t structure for given partition
 *
 * @param iterator  Iterator obtained using pi_partition_find. Must be non-NULL.
 *
 * @return pointer to pi_partition_t structure. This pointer is valid for the lifetime
 *         of the application.
 */
const pi_partition_t* pi_partition_get(pi_partition_iterator_t iterator);

/**
 * @brief Move partition iterator to the next partition found
 *
 * Any copies of the iterator will be invalid after this call.
 *
 * @param iterator Iterator obtained using pi_partition_find. Must be non-NULL.
 *
 * @return NULL if no partition was found, valid pi_partition_iterator_t otherwise.
 */
pi_partition_iterator_t pi_partition_next(pi_partition_iterator_t iterator);

/**
 * @brief Release partition iterator
 *
 * @param iterator Iterator obtained using pi_partition_find. Must be non-NULL.
 *
 */
void pi_partition_iterator_release(pi_partition_iterator_t iterator);

/**
 * @brief Verify partition data
 *
 * Given a pointer to partition data, verify this partition exists in the partition table (all fields match.)
 *
 * This function is also useful to take partition data which may be in a RAM buffer and convert it to a pointer to the
 * permanent partition data stored in flash.
 *
 * Pointers returned from this function can be compared directly to the address of any pointer returned from
 * pi_partition_get(), as a test for equality.
 *
 * @param partition Pointer to partition data to verify. Must be non-NULL. All fields of this structure must match the
 * partition table entry in flash for this function to return a successful match.
 *
 * @return
 * - If partition not found, returns NULL.
 * - If found, returns a pointer to the pi_partition_t structure in flash. This pointer is always valid for the lifetime of the application.
 */
const pi_partition_t *pi_partition_verify(const pi_partition_t *partition);

/**
 * @brief Read data from the partition
 *
 * @param partition Pointer to partition structure obtained using
 *                  pi_partition_find_first or pi_partition_get.
 *                  Must be non-NULL.
 * @param dst Pointer to the buffer where data should be stored.
 *            Pointer must be non-NULL and buffer must be at least 'size' bytes long.
 * @param src_offset Address of the data to be read, relative to the
 *                   beginning of the partition.
 * @param size Size of data to be read, in bytes.
 *
 * @return pi_OK, if data was read successfully;
 *         pi_ERR_INVALID_ARG, if src_offset exceeds partition size;
 *         pi_ERR_INVALID_SIZE, if read would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
pi_err_t pi_partition_read(const pi_partition_t* partition,
                             size_t src_offset, void* dst, size_t size);

/**
 * @brief Write data to the partition
 *
 * Before writing data to flash, corresponding region of flash needs to be erased.
 * This can be done using pi_partition_erase_range function.
 *
 * Partitions marked with an encryption flag will automatically be
 * written via the spi_flash_write_encrypted() function. If writing to
 * an encrypted partition, all write offsets and lengths must be
 * multiples of 16 bytes. See the spi_flash_write_encrypted() function
 * for more details. Unencrypted partitions do not have this
 * restriction.
 *
 * @param partition Pointer to partition structure obtained using
 *                  pi_partition_find_first or pi_partition_get.
 *                  Must be non-NULL.
 * @param dst_offset Address where the data should be written, relative to the
 *                   beginning of the partition.
 * @param src Pointer to the source buffer.  Pointer must be non-NULL and
 *            buffer must be at least 'size' bytes long.
 * @param size Size of data to be written, in bytes.
 *
 * @note Prior to writing to flash memory, make sure it has been erased with
 *       pi_partition_erase_range call.
 *
 * @return pi_OK, if data was written successfully;
 *         pi_ERR_INVALID_ARG, if dst_offset exceeds partition size;
 *         pi_ERR_INVALID_SIZE, if write would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
pi_err_t pi_partition_write(const pi_partition_t* partition,
                             size_t dst_offset, const void* src, size_t size);

/**
 * @brief Erase part of the partition
 *
 * @param partition Pointer to partition structure obtained using
 *                  pi_partition_find_first or pi_partition_get.
 *                  Must be non-NULL.
 * @param offset Offset from the beginning of partition where erase operation
 *               should start. Must be aligned to 4 kilobytes.
 * @param size Size of the range which should be erased, in bytes.
 *                   Must be divisible by 4 kilobytes.
 *
 * @return pi_OK, if the range was erased successfully;
 *         pi_ERR_INVALID_ARG, if iterator or dst are NULL;
 *         pi_ERR_INVALID_SIZE, if erase would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
pi_err_t pi_partition_erase_range(const pi_partition_t* partition,
                                    size_t offset, size_t size);

/**
 * @brief Configure MMU to map partition into data memory
 *
 * Unlike spi_flash_mmap function, which requires a 64kB aligned base address,
 * this function doesn't impose such a requirement.
 * If offset results in a flash address which is not aligned to 64kB boundary,
 * address will be rounded to the lower 64kB boundary, so that mapped region
 * includes requested range.
 * Pointer returned via out_ptr argument will be adjusted to point to the
 * requested offset (not necessarily to the beginning of mmap-ed region).
 *
 * To release mapped memory, pass handle returned via out_handle argument to
 * spi_flash_munmap function.
 *
 * @param partition Pointer to partition structure obtained using
 *                  pi_partition_find_first or pi_partition_get.
 *                  Must be non-NULL.
 * @param offset Offset from the beginning of partition where mapping should start.
 * @param size Size of the area to be mapped.
 * @param memory  Memory space where the region should be mapped
 * @param out_ptr  Output, pointer to the mapped memory region
 * @param out_handle  Output, handle which should be used for spi_flash_munmap call
 *
 * @return pi_OK, if successful
 */
pi_err_t pi_partition_mmap(const pi_partition_t* partition, size_t offset, size_t size,
                             spi_flash_mmap_memory_t memory,
                             const void** out_ptr, spi_flash_mmap_handle_t* out_handle);

/**
 * @brief Get SHA-256 digest for required partition.
 *
 * For apps with SHA-256 appended to the app image, the result is the appended SHA-256 value for the app image content.
 * The hash is verified before returning, if app content is invalid then the function returns pi_ERR_IMAGE_INVALID.
 * For apps without SHA-256 appended to the image, the result is the SHA-256 of all bytes in the app image.
 * For other partition types, the result is the SHA-256 of the entire partition.
 *
 * @param[in]  partition    Pointer to info for partition containing app or data. (fields: address, size and type, are required to be filled).
 * @param[out] sha_256      Returned SHA-256 digest for a given partition.
 *
 * @return
 *          - pi_OK: In case of successful operation.
 *          - pi_ERR_INVALID_ARG: The size was 0 or the sha_256 was NULL.
 *          - pi_ERR_NO_MEM: Cannot allocate memory for sha256 operation.
 *          - pi_ERR_IMAGE_INVALID: App partition doesn't contain a valid app image.
 *          - pi_FAIL: An allocation error occurred.
 */
pi_err_t pi_partition_get_sha256(const pi_partition_t *partition, uint8_t *sha_256);

/**
 * @brief Check for the identity of two partitions by SHA-256 digest.
 *
 * @param[in] partition_1 Pointer to info for partition 1 containing app or data. (fields: address, size and type, are required to be filled).
 * @param[in] partition_2 Pointer to info for partition 2 containing app or data. (fields: address, size and type, are required to be filled).
 *
 * @return
 *         - True:  In case of the two firmware is equal.
 *         - False: Otherwise
 */
bool pi_partition_check_identity(const pi_partition_t *partition_1, const pi_partition_t *partition_2);

/**
 * @brief Register a partition on an external flash chip
 *
 * This API allows designating certain areas of external flash chips (identified by the pi_flash_t structure)
 * as partitions. This allows using them with components which access SPI flash through the pi_partition API.
 *
 * @param flash_chip  Pointer to the structure identifying the flash chip
 * @param offset  Address in bytes, where the partition starts
 * @param size  Size of the partition in bytes
 * @param label  Partition name
 * @param type  One of the partition types (pi_PARTITION_TYPE_*). Note that applications can not be booted from external flash
 *              chips, so using pi_PARTITION_TYPE_APP is not supported.
 * @param subtype  One of the partition subtypes (pi_PARTITION_SUBTYPE_*)
 * @param[out] out_partition  Output, if non-NULL, receives the pointer to the resulting pi_partition_t structure
 * @return
 *      - pi_OK on success
 *      - pi_ERR_NOT_SUPPORTED if CONFIG_CONFIG_SPI_FLASH_USE_LEGACY_IMPL is enabled
 *      - pi_ERR_NO_MEM if memory allocation has failed
 *      - pi_ERR_INVALID_ARG if the new partition overlaps another partition on the same flash chip
 *      - pi_ERR_INVALID_SIZE if the partition doesn't fit into the flash chip size
 */
pi_err_t pi_partition_register_external(pi_flash_t* flash_chip, size_t offset, size_t size,
                                     const char* label, pi_partition_type_t type, pi_partition_subtype_t subtype,
                                     const pi_partition_t** out_partition);

/**
 * @brief Deregister the partition previously registered using pi_partition_register_external
 * @param partition  pointer to the partition structure obtained from pi_partition_register_external,
 * @return
 *      - pi_OK on success
 *      - pi_ERR_NOT_FOUND if the partition pointer is not found
 *      - pi_ERR_INVALID_ARG if the partition comes from the partition table
 *      - pi_ERR_INVALID_ARG if the partition was not registered using
 *        pi_partition_register_external function.
 */
pi_err_t pi_partition_deregister_external(const pi_partition_t* partition);

#ifdef __cplusplus
}
#endif


#endif /* __pi_PARTITION_H__ */
