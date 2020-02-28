#
# Second Stage Boot Loader Makefile
#------------------------------------

APP              = ssbl
APP_SRCS        += \
    ssbl.c \
    bootloader_utility.c \
    partition.c \
    ota_utility.c \
    ota.c

APP_INC         +=
APP_CFLAGS      += -Wall -Werror -Wextra \
    -DPI_LOG_DEFAULT_LEVEL=5 -DPI_LOG_NO_CORE_ID

ifeq ($(platform), gvsoc)
APP_LINK_SCRIPT = ssbl-GAP8-gvsoc.ld
else
APP_LINK_SCRIPT = ssbl-GAP8.ld
endif

io=host
PMSIS_OS = freertos

#
# Local variables
#

PLPBRIDGE_FLAGS = -f

#
# Partitions
#

#PARTITION_TABLE = factory.csv
PARTITION_TABLE = ota.csv

# Factory app
FACTORY_APP_NAME = factory
FACTORY_BIN = $(FACTORY_APP_NAME).bin
FACTORY_ELF = $(FACTORY_APP_NAME).elf
FLASH_DEPS += $(FACTORY_BIN)
GEN_FLASH_IMAGE_FLAGS += -p factory $(FACTORY_BIN)

$(FACTORY_ELF): factory/BUILD/GAP8_V2/GCC_RISCV/test
	cp $< $@

.PHONY: factory/BUILD/GAP8_V2/GCC_RISCV/test
factory/BUILD/GAP8_V2/GCC_RISCV/test:
	cd factory && make all;  cd ..

$(FACTORY_BIN): $(FACTORY_ELF)
	gapy elf2bin $<

#
# Includes
#
include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk


clean::
	rm -f $(FACTORY_BIN) $(FACTORY_ELF)
