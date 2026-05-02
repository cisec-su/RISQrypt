PROJ = pasta_test

TOOLCHAIN_DIR = ../../../sdk/toolchain

LIB_DIR = ../../../sdk/lib

PASTA_DIR = ../../src/pasta

SRC  = main.c
SRC += $(PASTA_DIR)/poly.c
SRC += $(PASTA_DIR)/pasta.c
SRC += $(PASTA_DIR)/symmetric.c
SRC += $(PASTA_DIR)/masked_poly.c
SRC += $(PASTA_DIR)/masked_gadgets.c
SRC += $(PASTA_DIR)/masked_pasta.c
SRC += ../../common/unity.c

INC = -I$(PASTA_DIR)
INC += -I../../common

CFLAGS += -DBUSY_CHECK_DIS

# CFLAGS += -DMEMORY_OPT_DIS
CFLAGS += -DREJ_SAMP_DIS



 
OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile
