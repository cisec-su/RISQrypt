PROJ = falcon_test

TOOLCHAIN_DIR = ../../../sdk/toolchain

LIB_DIR = ../../../sdk/lib

FALCON_DIR = ../../src/falcon


SRC  = main.c
SRC += $(FALCON_DIR)/verify.c
SRC += $(FALCON_DIR)/falcon.c
SRC += $(FALCON_DIR)/poly.c
SRC += $(FALCON_DIR)/symmetric.c
SRC += ../../common/unity.c

INC = -I$(FALCON_DIR)
INC += -I../../common

CFLAGS += -DBUSY_CHECK_DIS

OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile
