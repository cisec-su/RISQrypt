PROJ = sphincs_code_size

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

SPHINCS_DIR = ../../../src/sphincs

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128s
THASH = simple

SRC =  main.c
SRC += $(SPHINCS_DIR)/address.c
SRC += $(SPHINCS_DIR)/merkle.c
SRC += $(SPHINCS_DIR)/wots.c
SRC += $(SPHINCS_DIR)/wotsx1.c
SRC += $(SPHINCS_DIR)/utils.c
SRC += $(SPHINCS_DIR)/utilsx1.c
SRC += $(SPHINCS_DIR)/fors.c
SRC += $(SPHINCS_DIR)/sign.c
SRC += $(SPHINCS_DIR)/randombytes.c
SRC += $(SPHINCS_DIR)/fips202.c
SRC += $(SPHINCS_DIR)/hash_shake.c
SRC += $(SPHINCS_DIR)/thash_shake_$(THASH).c

# HW-masked source files
SRC += $(SPHINCS_DIR)/masked_fips202.c
SRC += $(SPHINCS_DIR)/masked_hash_shake.c
SRC += $(SPHINCS_DIR)/masked_thash_shake_$(THASH).c
SRC += $(SPHINCS_DIR)/masked_utils.c
SRC += $(SPHINCS_DIR)/masked_utilsx1.c
SRC += $(SPHINCS_DIR)/masked_wots.c
SRC += $(SPHINCS_DIR)/masked_wotsx1.c
SRC += $(SPHINCS_DIR)/masked_fors.c
SRC += $(SPHINCS_DIR)/masked_merkle.c
SRC += $(SPHINCS_DIR)/masked_sign.c

INC = -I$(SPHINCS_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

CFLAGS += -DPARAMS=$(PARAMS)

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

