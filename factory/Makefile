# Factory app
#------------------------------------
SHELL = /bin/bash
APP              = factory
APP_SRCS        += factory.c
APP_INC	        +=

LOG_LEVEL ?= 2
DEBUG_FLAGS      += -DPI_LOG_DEFAULT_LEVEL=$(LOG_LEVEL) -DPI_LOG_NO_CORE_ID

io?=uart

EXT_MEM_TYPE ?= QSPI
#export GAP_USE_OPENOCD=1
PMSIS_OS=pulpos

APP_CFLAGS += -D$(EXT_MEM_TYPE)


include $(RULES_DIR)/pmsis_rules.mk
