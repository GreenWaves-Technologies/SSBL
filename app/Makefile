# APP app
#------------------------------------
SHELL = /bin/bash
APP              = app
APP_SRCS        += app.c \
../ota.c ../ota_utility.c ../bootloader_utility.c
APP_INC	        += ..

VERSION ?= 0
LOG_LEVEL ?= 2
APP_CFLAGS      += -DPI_LOG_DEFAULT_LEVEL=$(LOG_LEVEL) -DPI_LOG_NO_CORE_ID -DVERSION_APP=$(VERSION)

io?=host
PMSIS_OS=freertos

include $(RULES_DIR)/pmsis_rules.mk
