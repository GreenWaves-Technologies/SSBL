/* PMSIS includes */

#include "pmsis.h"
#include "bsp/ota.h"
#include "bsp/partition.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/fs.h"
#include <bsp/fs/hostfs.h>
#include <bsp/fs/readfs.h>

#define BUFF_SIZE 1024

static struct pi_device flash;
static struct pi_hyperflash_conf flash_conf;
static struct pi_hyperflash_conf flash_conf;

pi_err_t update_from_semi_hosting(pi_device_t *flash, const char *binary_path)
{
    pi_err_t rc;
    pi_fs_file_t *file;
    const pi_partition_table_t table;
    const pi_partition_t *ota;
    uint8_t *buff;
    struct pi_device fs;
//    struct pi_hostfs_conf fs_conf;
    struct pi_readfs_conf fs_conf;
    
    PI_LOG_TRC("fac", "Open fs");
//    pi_hostfs_conf_init(&fs_conf);
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
    buff = pi_l2_malloc(BUFF_SIZE);
    if(buff == NULL)
    {
        PI_LOG_ERR("fac", "Unable to allocate buff into l2.");
        rc = PI_ERR_L2_NO_MEM;
        goto close_partition_and_return;
    }
    
    size_t read_size, i = 0;
    while ((read_size = pi_fs_read(file, buff, BUFF_SIZE)))
    {
        PI_LOG_TRC("fac", "Read %lu from file", read_size);
        pi_partition_write(ota, i * BUFF_SIZE, buff, read_size);
        i++;
    }
    
    PI_LOG_INF("fac", "Set boot partition.");
    rc = ota_set_boot_partition(table, ota);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("fac", "Unable to set next boot partition.");
        rc = PI_FAIL;
        goto free_and_return;
    }
    
    free_and_return:
    pi_l2_free(buff, BUFF_SIZE);
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

void factory(void)
{
    printf("Hello word from factory app after kickoff!\n");
    
    PI_LOG_TRC("fac", "Open flash");
    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(&flash, &flash_conf);
    if(pi_flash_open(&flash))
    {
        PI_LOG_ERR("fac", "Unable to open flash");
        return;
    }
    
    PI_LOG_INF("fac", "Try to update to app0.bin from semi hosting");
//    pi_err_t rc = update_from_semi_hosting(&flash, "/g/app/ssbl/app0.bin");
    pi_err_t rc = update_from_semi_hosting(&flash, "app0.bin");
    
    if (rc == PI_OK)
    {
    printf("Please reboot the device.\n");
    while(1)
    {
        __asm__("wfi"::);
    }
    }
    pmsis_exit(rc);
}

int main(void)
{
    printf("\n\n\t *** PMSIS Factory App ***\n\n");
    pmsis_kickoff((void *) factory);
}

