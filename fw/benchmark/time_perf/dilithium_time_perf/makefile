PROJ = dilithium_time_perf

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

DILITHIUM_HW_DIR = ../../../dilithium_hw

SRC =  main.c
SRC += $(DILITHIUM_HW_DIR)/packing.c
SRC += $(DILITHIUM_HW_DIR)/poly.c
SRC += $(DILITHIUM_HW_DIR)/polyvec.c
SRC += $(DILITHIUM_HW_DIR)/randombytes.c
SRC += $(DILITHIUM_HW_DIR)/sign.c
SRC += $(DILITHIUM_HW_DIR)/symmetric.c
SRC += $(DILITHIUM_HW_DIR)/reduce.c
SRC += $(DILITHIUM_HW_DIR)/masked.c
SRC += $(DILITHIUM_HW_DIR)/masked_sign.c
SRC += $(DILITHIUM_HW_DIR)/masked_poly.c
SRC += $(DILITHIUM_HW_DIR)/masked_polyvec.c
SRC += $(DILITHIUM_HW_DIR)/masked_gadgets.c
SRC += $(DILITHIUM_HW_DIR)/masked_packing.c
SRC += $(DILITHIUM_HW_DIR)/masked_symmetric.c

INC = -I$(DILITHIUM_HW_DIR)
INC += -I../../../common

OBJ_DUMP = 1 # Uncomment if disassembly is needed

CFLAGS += -DBUSY_CHECK_DIS


OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

