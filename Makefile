#
# Second Stage Boot Loader Makefile
#------------------------------------

APP              = ssbl
APP_SRCS        += ssbl.c
APP_INC         += .

LOG_LEVEL ?= 2
APP_CFLAGS      += -Wall -Werror -Wextra \
    -DPI_LOG_DEFAULT_LEVEL=$(LOG_LEVEL) -DPI_LOG_NO_CORE_ID

ifeq ($(platform), gvsoc)
APP_LINK_SCRIPT = ssbl-GAP8-gvsoc.ld
io?=host
else
APP_LINK_SCRIPT = ssbl-GAP8.ld
#io?=uart
io?=host
endif

#export GAP_USE_OPENOCD=1
PMSIS_OS = freertos

#
# Partitions
#

PARTITION_TABLE = ota.csv

# ReadFS
READFS_FILES += app0.bin app1.bin

# Rules

%.bin: %.elf
	gapy elf2bin $<

# Factory app
FACTORY_APP_NAME = factory
FACTORY_BIN = $(FACTORY_APP_NAME).bin
FACTORY_ELF = $(FACTORY_APP_NAME).elf
FLASH_DEPS += $(FACTORY_BIN)
GEN_FLASH_IMAGE_FLAGS += -p factory $(FACTORY_BIN)

factory.elf:
	cd factory && make LOG_LEVEL=$(LOG_LEVEL) io=$(io) all && cp BUILD/GAP8_V2/GCC_RISCV/test ../$@ && cd ..

# App
app0.elf app1.elf:
	cd app && touch app.c && make LOG_LEVEL=$(LOG_LEVEL) VERSION=0 io=$(io) all && cp BUILD/GAP8_V2/GCC_RISCV/test ../app0.elf && cd ..
	cd app && touch app.c && make LOG_LEVEL=$(LOG_LEVEL) VERSION=1 io=$(io) all && cp BUILD/GAP8_V2/GCC_RISCV/test ../app1.elf && cd ..

#
# Includes
#
include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk


clean_all: clean
	cd factory/ && make clean && cd ..
	cd app/ && make clean && cd ..

clean::
	rm -f $(FACTORY_BIN) $(FACTORY_ELF)
	rm -f app0.elf app0.bin
	rm -f app1.elf app1.bin




