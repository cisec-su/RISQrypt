PROJ = dilithium_hw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

SRC  = main.c
SRC += packing.c
SRC += poly.c
SRC += polyvec.c
SRC += randombytes.c
SRC += rounding.c
SRC += sign.c
SRC += symmetric-shake.c
SRC += fips202.c
SRC += reduce.c

OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile
