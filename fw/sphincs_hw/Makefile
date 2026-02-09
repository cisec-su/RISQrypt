PROJ = sphincs_hw

TOOLCHAIN_DIR = ../../sdk/toolchain
LIB_DIR = ../../sdk/lib

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128f
THASH = simple

# Source files
SRC = address.c
SRC += masked_merkle.c
SRC += masked_wots.c
SRC += masked_wotsx1.c
SRC += masked_utils.c
SRC += masked_utilsx1.c
SRC += masked_fors.c
SRC += masked_sign.c
SRC += randombytes.c

SRC += masked_fips202.c masked_hash_shake.c masked_thash_shake_simple.c
SRC += merkle.c wots.c wotsx1.c utils.c utilsx1.c fors.c sign.c fips202.c hash_shake.c thash_shake_simple.c

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
