PROJ = sphincs_hw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128s
THASH = simple

# Source files
SRC = address.c
SRC += merkle.c
SRC += wots.c
SRC += wotsx1.c
SRC += utils.c
SRC += utilsx1.c
SRC += fors.c
SRC += sign.c
SRC += randombytes.c

SRC += fips202.c hash_shake.c thash_shake_$(THASH).c

# Main file
SRC += main.c

# Common files
SRC += ../common/unity.c

# Include directories
INC = -I../common

# OBJ_DUMP = 1

OPTIMIZATION = -O3 # Default is -O0

# Pass PARAMS to C code
CFLAGS += -DPARAMS=$(PARAMS)
CFLAGS += -DBUSY_CHECK_DIS

include $(TOOLCHAIN_DIR)/makefile
