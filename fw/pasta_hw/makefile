PROJ = pasta_hw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

SRC  = main.c
SRC += poly.c
SRC += pasta.c
SRC += symmetric.c
SRC += masked.c
SRC += masked_poly.c
SRC += masked_gadgets.c
SRC += masked_pasta.c
SRC += ../common/unity.c

INC = -I../common

CFLAGS += -DBUSY_CHECK_DIS
CFLAGS += -DREJ_SAMP_DIS


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile

