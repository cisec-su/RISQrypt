PROJ = kyber_asm

TOOLCHAIN_DIR = ../../sdk/toolchain

LIB_DIR = ../../sdk/lib

SRC  = main.c
SRC += indcpa.c
SRC += poly.c
SRC += polyvec.c
SRC += reduce.c
SRC += ntt.c
SRC += ntt_forward.S
SRC += ntt_backward.S
SRC += BaseMul.S


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# CFLAGS = # Use as needed

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

