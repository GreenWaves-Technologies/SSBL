#
# Second Stage Boot Loader Makefile
#------------------------------------

APP              = ssbl
APP_SRCS        += ssbl.c
APP_INC         += .

LOG_LEVEL ?= 5
APP_CFLAGS      += -Wall -Werror
APP_CFLAGS    += -DPI_LOG_DEFAULT_LEVEL=$(LOG_LEVEL) -DPI_LOG_LOCAL_LEVEL=$(LOG_LEVEL) -DPI_LOG_NO_CORE_ID -DCONFIG_NO_FC_TINY -DCONFIG_NO_CLUSTER=1

CONFIG_NO_LDSCRIPT = 1
CONFIG_PULPRT_LIB = rtnotiny
APP_LDFLAGS += -Tssbl-GAP8.ld

#export GAP_USE_OPENOCD=1
#PMSIS_OS = freertos

#
# Partitions
#

config_args += --config-opt=flash/content/partition-table=ota.csv
config_args += --config-opt=flash/content/partitions/factory/image=factory.bin

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
	cd factory && make LOG_LEVEL=$(LOG_LEVEL) io=$(io) all && cp BUILD/$(TARGET_CHIP)/GCC_RISCV/factory ../$@ && cd ..

# App
app0.elf app1.elf:
	cd app && touch app.c && make clean LOG_LEVEL=$(LOG_LEVEL) VERSION=0 io=$(io) all && cp BUILD/$(TARGET_CHIP)/GCC_RISCV/app ../app0.elf && cd ..
	cd app && touch app.c && make clean LOG_LEVEL=$(LOG_LEVEL) VERSION=1 io=$(io) all && cp BUILD/$(TARGET_CHIP)/GCC_RISCV/app ../app1.elf && cd ..

build_bin: app0.elf app1.elf factory.elf
	gapy --target=$(GAPY_TARGET) elf2bin app0.elf
	gapy --target=$(GAPY_TARGET) elf2bin app1.elf
	gapy --target=$(GAPY_TARGET) elf2bin factory.elf

prebuild: build_bin

all:: prebuild

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




