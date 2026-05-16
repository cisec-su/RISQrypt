PROJ = rubato_profiling

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

RUBATO_DIR = ../../../src/rubato

SRC =  main.c
SRC += $(RUBATO_DIR)/rubato.c
SRC += $(RUBATO_DIR)/poly.c
SRC += $(RUBATO_DIR)/symmetric.c
SRC += $(RUBATO_DIR)/masked_rubato.c
SRC += $(RUBATO_DIR)/masked_poly.c
SRC += $(RUBATO_DIR)/masked_gadgets.c
INC = -I$(RUBATO_DIR)
INC += -I../../../common

OBJ_DUMP = 1 # Uncomment if disassembly is neededcd

CFLAGS += -DSDK_BENCHMARK_ENABLED

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

