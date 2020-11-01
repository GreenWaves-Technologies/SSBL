# GAP8 Second Stage Boot Loader

## Introduction

The GAP8 Second Stage Boot Loader (SSBL) is a tool embedded in a GAP8 based system, designed for loading section(s) from different address in flash to on chip memories (L2 and/or L1), and execute it. Based on the SSBL, we can achieve features like:

* OTA (Over The Air) - Update the embedded firmware, a new SW, NN coefficients, Encryption Keys, etc on a GAP8 based device.
* Boot GAP8 with different applications.
* Update the encryption keys and make the system more secure. 
* Configure the BOOT via network


## Architecture of SSBL

The SSBL consisted principally with 2 parts:

* Boot Loader: Manage the partition table, load sections and execute it. 
* Flash Partition Tool: Creat partitions in a flash image based on a partition table. 


## Architecutre of OTA

Based on the SSBL, OTA works with an updater, which can:

* Load a new firmware/filesystem/files from an interface (hyperbus/uart/spi) chunk by chunk (limited by GAP8 L2 Size)
* Flash it chunk by chunk to the target address in Flash
* Check the update is correct (bootable if it's a new firmware binary)
* Modify the partition table in Flash according to the update.
* Reboot (need a HW reboot)


## How it works (This example)

This example shows how to use SSBL, OTA, Flash System and all the tools to achieve a firmware update on the device. 


### Architecture and Process

0. Prepare SSBL(ssbl.c), Updater (factory/factory.c), application (app/app.c) 
1. Precompile SSBL, updater (factory) and applications(app0 and app1), generate binaries. 
2. GAPY: Create partition table according to the CSV file: ota.csv, then build FlashImage with Partitions accordingly. (PATH: gap_sdk/tools/gapy/)
3. Bootloader: Load sections from partitions into L2 (PATH: gap_sdk/rtos/pmsis/pmsis_bsp/bootloader)
4. Partition: Load sections from partitions into L2 (PATH: gap_sdk/rtos/pmsis/pmsis_bsp/bootloader)
5. OTA: Load sections from partitions into L2 (PATH: gap_sdk/rtos/pmsis/pmsis_bsp/bootloader)

#### SSBL 

In this example, the ssbl.c is the principle firmware of SSBL, which is in charge of load the boot section, according to the partition table, from Flash to onchip memory (L2 and FC TCDM), then execute it. 

#### Factory (Updater)

Read Partition table, get the target partition address in flash which should be updated. Get the new application and write it into the target partition. 

In this example, the Updater will get the new application from flash system and write it into an application(ota) section. Then change the boot status in the partition table to set the default boot address.

#### app 

The example user application, which need to confirm the boot in the partition table, then update the partition table. 

Once this is done, except the updater has been executed again, it becomes the default boot partition.


### Build FlashImage

By using the tool "gapy" in the SDK, we can build a flashimage with different partitions and flashimage. 

For this step, we need to prepare a partition table (a csv file), like "ota.csv" in this folder:

~~~~~ shell
# Name,   Type, SubType, Offset,   Size, Flags
# Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
otadata,  data, ota,     ,        2sec,     // Partition for saving otadatas, 2 app sections will be prepared
factory,  app,  factory, ,        1M,       // Partition (1MB) for Factory (Updater)
ota_0,    app,  ota0,   ,        1M,        // Partition (1MB) for ota_0 (app 0)
ota_1,    app,  ota1,   ,        1M,        // Partition (1MB) for ota_1 (app 1)
~~~~~

This CSV file should be provided to the gapy as this line in the Makefile:

~~~~~ shell
config_args += --config-opt=flash/content/partition-table=ota.csv
~~~~~

Add updater binary in the "factory" partition

1. Using gapy to create the binary based on the ELF file: (in Makefile)

~~~~~ shell
gapy --target=$(GAPY_TARGET) elf2bin factory.elf
~~~~~

2. Add the factory in partition:

~~~~~ shell
GEN_FLASH_IMAGE_FLAGS += -p factory $(FACTORY_BIN)
~~~~~

Using the same way to create bin, and add apps in to filesystem -- readFS:

~~~~~ shell
READFS_FILES += app0.bin app1.bin
~~~~~

The flashimage stucture:

            ---------------------------------   Offset 0x0
            |        Partition table        |   
            |           Address             |   
            ---------------------------------   Offset 0x4
            |             SSBL              |
            |                               |
            ---------------------------------   
            |        Partition Table        |
            |                               |
            ---------------------------------   
            |           Factory             |
            |                               |
            ---------------------------------   
            |            OTA_0              |
            |                               |
            ---------------------------------   
            |            OTA_1              |
            |                               |
            ---------------------------------   
            |         File System           |
            |           ReadFS              |
            ---------------------------------   


### Example Scenario

Once the FlashImage has been built, there is nothing to do but flash it into the flash and power the device ON. Since the flashimage will be loaded via JTAG, and it's quiet big, the flash process may takes a while. 

Be attention, this example requires the efuse has been programmed for "BOOT FROM Flash", it works with both HyperFlash and QSPI Flash based device.

To compile and trigger the flash, you just need to use this command:

~~~~~ shell
make all io=uart
~~~~~

Once the flash process is done, when the device be powered on (1st boot after the flash), we will see the console from uart interface (example_log), which you can see these steps:

***1st BOOT: 1st update*** 

1. SSBL be loaded and executed, it read partition table, and cannot find bootable app. It load the updater (factory) sections and execute it.
2. The Factory be executed in GAP8, and it try to update the ota 0 by read the app0 from the filesystem. You will see:
~~~~~ shell
Try to update to app0.bin from readfs
~~~~~
3. Once it's done, the factory will check the update status and change the boot address in the partition table. (***In this example, the 1st update suppose to show you what it will do when update failed***)


***2nd BOOT: (Failed BOOT)***
1. The app0 is designed to show you what the SSBL and OTA will do when update failed. 
2. The SSBL be loaded and executed after reboot, it try to load app0 and execute it since the ota0 has been updated in the 1st boot above.
3. The app0 is booted and mark the app unvalid in the partition table. 
4. Wait for reboot.

***3rd BOOT: update again***
1. SSBL be loaded and executed after reboot, it cannot find bootable and valid app to boot, so load and exectue again the updater (factory).
2. The factory try to update the app1 in ota_1 partition this time, since the ota_0 be marked as unvalid.
3. Update will take some time until you see "Please reboot the device." The partition table has been update to boot with OTA_1 partition.

***4th BOOT:***
1. SSBL will be always executed, it will load the application in the OTA_1 partition and execute it since the 3rd BOOT changes the table.
2. The app 1 has been executed, and mark the app is valid in the partition table.

***AFTER 4th BOOT***
1. The SSBL will be always executed.
2. The app 1 will be loaded and booted until the flash has been reflashed, or the factory has been executed again.


## How to integrate and what need to be modified.

The SSBL and the OTA example illustrated how the OTA works, and how the architecture looks like. Based on this, user can modify and implement it into their own system with different communication interfaces (UART/SPI/Hyperbus/etc).

### SSBL

The SSBL is in charge of loading different sections into GAP and execute it. In this example, we show you how the process it is, but the "Updater" should be able to be executed again when needed. 

Therefore, a GPIO can be added in SSBL to trigger the updater when the GPIO is triggered.

### Updater (factory)

The updater should be in charge of:

1. Communicating with the external device, for example a BLE module, a wifi module, a flash (in this example). 
2. Get the new firmware and write it into the right partition. 
3. Check and validate the update process.

In this case, the user need to integrate his/her protocol to communicate with ex-devices and check the package it received and flashed is complet and correct. 

### Further Features

There are futher features can be added:

1. Security: The new firmware can be encrypted for security reason. The updater should decrypt it on the device.
2. Filesystem update: Able to update the files in the filesystem. 
3. Configure the boot.3. Configure the boot.3. Configure the boot.


