PROJ = sphincs_hw

TOOLCHAIN_DIR = ../../../sdk/toolchain
LIB_DIR = ../../../sdk/lib

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128f
THASH = simple

# Source files
SRC = address.c
SRC += randombytes.c
SRC += swmasked_merkle.c swmasked_wots.c swmasked_wotsx1.c swmasked_utils.c swmasked_utilsx1.c swmasked_fors.c swmasked_sign.c
SRC += swmasked_fips202.c swmasked_hash_shake.c swmasked_thash_shake_simple.c
SRC += ../merkle.c ../wots.c ../wotsx1.c ../utils.c ../utilsx1.c ../fors.c ../sign.c ../fips202.c ../hash_shake.c ../thash_shake_simple.c

# Main file
SRC += main.c

# Common files
SRC += ../../common/unity.c

# Include directories
INC = -I../../common -I..

# OBJ_DUMP = 1

OPTIMIZATION = -O3 # Default is -O0

# Pass PARAMS to C code
CFLAGS += -DPARAMS=$(PARAMS)
CFLAGS += -DBUSY_CHECK_DIS

include $(TOOLCHAIN_DIR)/makefile
