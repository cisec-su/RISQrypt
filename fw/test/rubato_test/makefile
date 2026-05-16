PROJ = rubato_test

TOOLCHAIN_DIR = ../../../sdk/toolchain

LIB_DIR = ../../../sdk/lib

RUBATO_DIR = ../../src/rubato

SRC  = main.c
SRC += $(RUBATO_DIR)/rubato.c
SRC += $(RUBATO_DIR)/poly.c
SRC += $(RUBATO_DIR)/masked_rubato.c
SRC += $(RUBATO_DIR)/masked_poly.c
SRC += $(RUBATO_DIR)/masked_gadgets.c
SRC += $(RUBATO_DIR)/symmetric.c
SRC += ../../common/unity.c

INC = -I$(RUBATO_DIR)
INC += -I../../common

CFLAGS += -DBUSY_CHECK_DIS


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile

