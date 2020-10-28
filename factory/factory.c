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


void factory(void)
{
    pi_err_t rc;
    char *path;
    ota_img_states_t ota_img_state = PI_OTA_IMG_UNDEFINED;
    
    printf("Hello word from factory app after kickoff!\n");
    
#if defined(QSPI)
    pi_spiflash_conf_init(&flash_conf);
#else
    pi_hyperflash_conf_init(&flash_conf);
#endif

    pi_open_from_conf(&flash, &flash_conf);
    if(pi_flash_open(&flash))
    {
        PI_LOG_ERR("fac", "Unable to open flash");
        return;
    }
    
    printf("Read OTA img state\n");
    rc = ota_get_state_info_from_flash(&flash, &ota_img_state, NULL);
    if(rc == PI_OK && ota_img_state == PI_OTA_IMG_INVALID)
    {
        printf("Last update failed! Try to update to app v1\n");
        path = "app1.bin";
    } else
    {
        printf("Try to update to app0.bin from readfs\n");
        path = "app0.bin";
    }
    
    rc = update_from_readfs(&flash, path);
    
    pi_flash_close(&flash);
    
    if(rc == PI_OK)
    {
        ota_reboot();
    } else{
        PI_LOG_ERR("fac", "Unable to update app");
    }
    
    pmsis_exit(rc);
}

int main(void)
{
    pmsis_kickoff((void *) factory);
}

