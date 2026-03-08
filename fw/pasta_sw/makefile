PROJ = pasta_sw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

SRC  = main.c
SRC += pasta_soft.c
SRC += fips202.c
SRC += ../kyber_asm/keccakf1600_asm.S
SRC += ../common/unity.c

INC = -I../common

CFLAGS += -DBUSY_CHECK_DIS
# CFLAGS += -DUSE_BARRETT  # Comment out to use % operator instead of Barrett reduction


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# Optimization level
OPTIMIZATION = -O3 # -O3 # Default is -O0

include $(TOOLCHAIN_DIR)/makefile

