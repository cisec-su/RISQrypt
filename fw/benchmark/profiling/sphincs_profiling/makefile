PROJ = sphincs_profiling

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

SPHINCS_DIR = ../../../src/sphincs

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128f
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

INC = -I$(SPHINCS_DIR)
INC += -I../../../common

OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef MASKING_EN
CFLAGS += -DMASKING_EN
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
endif

CFLAGS += -DPARAMS=$(PARAMS)
CFLAGS += -DSDK_BENCHMARK_ENABLED

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

