PROJ = pasta_time_perf

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

PASTA_DIR = ../../../src/pasta

SRC =  main.c
SRC += $(PASTA_DIR)/masked_gadgets.c
SRC += $(PASTA_DIR)/masked_pasta.c
SRC += $(PASTA_DIR)/masked_poly.c
SRC += $(PASTA_DIR)/pasta.c
SRC += $(PASTA_DIR)/poly.c
SRC += $(PASTA_DIR)/symmetric.c
INC = -I$(PASTA_DIR)
INC += -I../../../common

OBJ_DUMP = 1 # Uncomment if disassembly is neededcd

CFLAGS += -DREJ_SAMP_DIS

CFLAGS += -DSDK_BENCHMARK_ENABLED

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

