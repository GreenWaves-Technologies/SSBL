#
# Second Stage Boot Loader Makefile
#------------------------------------

APP              = ssbl
APP_SRCS        += \
    ssbl.c \
    partition.c \
    bootloader_utility.c

APP_INC         +=
APP_CFLAGS      += -Wall -Werror -Wextra \
    -DPI_LOG_DEFAULT_LEVEL=5 -DPI_LOG_NO_CORE_ID

LD_SCRIPT = ssbl-GAP8.ld

platform = gvsoc

#
# Local variables
#

MAKEFILE_DIR_PATH := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
FLASH_DATA := data.img
FLASH_DATA_PATH := $(MAKEFILE_DIR_PATH)$(FLASH_DATA)

#PARTITION_TABLE = factory.csv
PARTITION_TABLE = ota.csv

FACTORY_APP_NAME = factory
FACTORY_BIN = $(FACTORY_APP_NAME).bin
FACTORY_ELF = $(FACTORY_APP_NAME).elf

FLASH_DEPS += $(FACTORY_BIN)
GEN_FLASH_IMAGE_FLAGS += -p factory $(FACTORY_BIN)

$(FACTORY_ELF): factory/BUILD/GAP8_V2/GCC_RISCV/test
	cp $< $@

factory/BUILD/GAP8_V2/GCC_RISCV/test: factory/factory.c factory/Makefile
	cd factory && make -j8 all;  cd ..

$(FACTORY_BIN): $(FACTORY_ELF)
	gapy elf2bin $<

rgv: $(FLASH_IMG) | $(RGV_DIR)
	gvsoc --config=gapuino \
	--dir=$(RGV_DIR) \
	--config-opt **/flash/preload_file=$(FLASH_IMG_NAME) \
	--config-opt=**/runner/boot_from_flash=false \
	prepare run

$(RGV_DIR):
	mkdir -p $@


#
# Includes
#
include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk


clean::
	rm -f $(FACTORY_BIN) $(FACTORY_ELF)
