# APP app
#------------------------------------
SHELL = /bin/bash
APP              = app
APP_SRCS        += app.c
APP_INC	        +=

VERSION ?= 0
LOG_LEVEL ?= 2
APP_CFLAGS      += -DVERSION_APP=$(VERSION)

DEBUG_FLAGS    = -DPI_LOG_DEFAULT_LEVEL=$(LOG_LEVEL) -DPI_LOG_NO_CORE_ID

io ?= host

PMSIS_OS = pulpos

include $(RULES_DIR)/pmsis_rules.mk
