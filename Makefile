#
# Second Stage Boot Loader Makefile
#------------------------------------

APP              = ssbl
APP_SRCS        += ssbl.c partition.c pi_partition.c
APP_INC         +=
APP_CFLAGS      += -Wall -Werror -Wextra -D SSBL_YES_TRACE

platform = gvsoc

#
# Includes
#
include $(GAP_SDK_HOME)/tools/rules/pmsis_rules.mk

#
# Local variables
#

MAKEFILE_DIR_PATH := $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
FLASH_DATA := data.img
FLASH_DATA_PATH := $(MAKEFILE_DIR_PATH)$(FLASH_DATA)

SSBL_ELF = $(BUILDDIR)/test
RGV_DIR = gvsoc
FLASH_IMG_NAME = flash.img
FLASH_IMG = $(RGV_DIR)/$(FLASH_IMG_NAME)
buildFlashImage = /home/raifer/gapy/build_flash_image.py


#
# Targets
#
img: $(FLASH_IMG)
$(FLASH_IMG): partitions.csv $(buildFlashImage) $(APP_SRCS)
	python3 $(buildFlashImage) --partition-table $< $(SSBL_ELF) -o $@


rgv: $(FLASH_IMG) | $(RGV_DIR)
	gvsoc --config=gapuino \
	--dir=$(RGV_DIR) \
	--config-opt **/flash/preload_file=$(FLASH_IMG_NAME) \
	--config-opt=**/runner/boot_from_flash=false \
	run

$(RGV_DIR):
	mkdir -p $@


