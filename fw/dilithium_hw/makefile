PROJ = dilithium_hw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

SRC  = main.c
SRC += packing.c
SRC += poly.c
SRC += polyvec.c
SRC += randombytes.c
SRC += sign.c
SRC += symmetric.c
SRC += reduce.c
SRC += masked.c
SRC += masked_sign.c
SRC += masked_poly.c
SRC += masked_polyvec.c
SRC += masked_gadgets.c
SRC += masked_packing.c
SRC += masked_symmetric.c
SRC += ../common/unity.c


INC = -I../common


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile
