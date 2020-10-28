/* PMSIS includes */

#include "pmsis.h"
#include "bsp/ota.h"
#include "bsp/partition.h"
#if defined(QSPI)
#define FLASH_NAME "QSPI"
#include "bsp/flash/spiflash.h"
#else
#define FLASH_NAME "HYPER"
#include "bsp/flash/hyperflash.h"
#endif

#include "bsp/fs.h"
#include <bsp/fs/hostfs.h>
#include <bsp/fs/readfs.h>
#include "bsp/updater.h"

static struct pi_device flash;
#if defined(QSPI)
static struct pi_spiflash_conf flash_conf;
#else
static struct pi_hyperflash_conf flash_conf;
#endif

bool test(void)
{
    if (VERSION_APP == 0)
        return false;
    else
        return true;
}

void app(void)
{
    pi_err_t rc = PI_OK;
    pi_partition_table_t table;
    ota_img_states_t ota_img_state;
    
    printf("Hello word from app version %u after kickoff!\n", VERSION_APP);
    
#if defined(QSPI)
    pi_spiflash_conf_init(&flash_conf);
#else
    pi_hyperflash_conf_init(&flash_conf);
#endif

    pi_open_from_conf(&flash, &flash_conf);
    if(pi_flash_open(&flash))
    {
        PI_LOG_ERR("app", "Unable to open flash");
        pmsis_exit(PI_FAIL);
    }
    
    rc = pi_partition_table_load(&flash, &table);
    if (rc != PI_OK)
    {
        PI_LOG_ERR("app", "Unable to load partition table.");
        return;
    }
    
    rc = ota_get_state_info(table, &ota_img_state, NULL);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("app", "Unable to load OTA state");
        pmsis_exit(PI_FAIL);
    }
    
    if(ota_img_state == PI_OTA_IMG_PENDING_VERIFY)
    {
        printf("This is the first boot for this version, run tests...\n");
        bool test_is_ok = test();
        if(test_is_ok)
        {
            printf("Test ok, mark app valid\n");
            ota_mark_app_valid_cancel_rollback(table);
        } else
        {
            printf("Test not ok, mark app unvalid\n");
            ota_mark_app_invalid_rollback_and_reboot(table);
        }
        
    } else
    {
        printf("My app run normaly\n");
    }
    
    pi_partition_table_free(table);
    pi_flash_close(&flash);
    pmsis_exit(PI_OK);
}

int main(void)
{
    pmsis_kickoff((void *) app);
}

