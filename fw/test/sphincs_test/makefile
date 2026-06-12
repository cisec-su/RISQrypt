PROJ = sphincs_test

TOOLCHAIN_DIR = ../../../sdk/toolchain
LIB_DIR = ../../../sdk/lib

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128f
THASH = simple

SPHINCS_DIR = ../../src/sphincs

# Unmasked source files
SRC  = $(SPHINCS_DIR)/address.c
SRC += $(SPHINCS_DIR)/randombytes.c
SRC += $(SPHINCS_DIR)/merkle.c
SRC += $(SPHINCS_DIR)/wots.c
SRC += $(SPHINCS_DIR)/wotsx1.c
SRC += $(SPHINCS_DIR)/utils.c
SRC += $(SPHINCS_DIR)/utilsx1.c
SRC += $(SPHINCS_DIR)/fors.c
SRC += $(SPHINCS_DIR)/sign.c
SRC += $(SPHINCS_DIR)/fips202.c
SRC += $(SPHINCS_DIR)/hash_shake.c
SRC += $(SPHINCS_DIR)/thash_shake_simple.c

# HW-masked source files
SRC += $(SPHINCS_DIR)/masked_fips202.c
SRC += $(SPHINCS_DIR)/masked_hash_shake.c
SRC += $(SPHINCS_DIR)/masked_thash_shake_simple.c
SRC += $(SPHINCS_DIR)/masked_utils.c
SRC += $(SPHINCS_DIR)/masked_utilsx1.c
SRC += $(SPHINCS_DIR)/masked_wots.c
SRC += $(SPHINCS_DIR)/masked_wotsx1.c
SRC += $(SPHINCS_DIR)/masked_fors.c
SRC += $(SPHINCS_DIR)/masked_merkle.c
SRC += $(SPHINCS_DIR)/masked_sign.c

# Main test file
SRC += main.c

# Common files
SRC += ../../common/unity.c

# Include directories
INC  = -I$(SPHINCS_DIR)
INC += -I../../common

# OBJ_DUMP = 1

OPTIMIZATION = -O3 # Default is -O0

# Pass PARAMS to C code
CFLAGS += -DPARAMS=$(PARAMS)
CFLAGS += -DBUSY_CHECK_DIS

include $(TOOLCHAIN_DIR)/makefile
