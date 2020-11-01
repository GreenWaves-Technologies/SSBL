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

'''''sh
# Name,   Type, SubType, Offset,   Size, Flags
# Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
otadata,  data, ota,     ,        2sec,
factory,  app,  factory, ,        1M,
ota_0,    app,  ota0,   ,        1M,
ota_1,    app,  ota1,   ,        1M,
'''''


### BOOT Updater (factory)

### Update app and boot app from Filesystem

#### Update Failed

#### Update Success


## How to integrate

