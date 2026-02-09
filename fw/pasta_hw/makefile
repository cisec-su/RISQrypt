PROJ = pasta_hw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

SRC  = main.c
SRC += poly.c
SRC += pasta.c
SRC += ../common/unity.c


INC = -I../common

CFLAGS += -DBUSY_CHECK_DIS

OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile

