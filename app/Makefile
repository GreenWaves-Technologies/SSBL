# APP app
#------------------------------------
SHELL = /bin/bash
APP              = app
APP_SRCS        += app.c
APP_INC	        +=

VERSION ?= 0
LOG_LEVEL ?= 2
EXT_MEM_TYPE ?= QSPI

APP_CFLAGS      += -DVERSION_APP=$(VERSION)

APP_CFLAGS += -D$(EXT_MEM_TYPE)
DEBUG_FLAGS    = -DPI_LOG_DEFAULT_LEVEL=$(LOG_LEVEL) -DPI_LOG_NO_CORE_ID

io ?= uart

PMSIS_OS = pulpos

include $(RULES_DIR)/pmsis_rules.mk
