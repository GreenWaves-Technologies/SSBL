# Factory app
#------------------------------------
SHELL = /bin/bash
APP              = factory
APP_SRCS        += factory.c \
../ota.c ../ota_utility.c ../bootloader_utility.c
APP_INC	        += ..
APP_CFLAGS      += -DPI_LOG_DEFAULT_LEVEL=5 -DPI_LOG_NO_CORE_ID

PARTITION_TABLE = ota.csv
READFS_FILES = app0.bin
io=host
override runner_args += --config-opt=runner/bridge=openocd

PMSIS_OS=freertos



include $(RULES_DIR)/pmsis_rules.mk
