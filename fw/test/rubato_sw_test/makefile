PROJ = rubato_sw_test

TOOLCHAIN_DIR = ../../../sdk/toolchain

LIB_DIR = ../../../sdk/lib

RUBATO_SW_DIR = ../../src/rubato_sw

SRC  = main.c
SRC += $(RUBATO_SW_DIR)/rubato_soft.c
SRC += $(RUBATO_SW_DIR)/poly_soft.c
SRC += $(RUBATO_SW_DIR)/masked_poly_soft.c
SRC += $(RUBATO_SW_DIR)/masked_rubato_soft.c
SRC += $(RUBATO_SW_DIR)/fips202.c
SRC += $(RUBATO_SW_DIR)/keccakf1600_asm.S

SRC += ../../common/unity.c

INC = -I$(RUBATO_SW_DIR)
INC += -I../../common

CFLAGS += -DBUSY_CHECK_DIS


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile

