PROJ = sphincs_profiling

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

SPHINCS_HW_DIR = ../../../sphincs_hw

# SPHINCS+ Parameters
PARAMS = sphincs-shake-128f
THASH = simple

SRC =  main.c
SRC += $(SPHINCS_HW_DIR)/address.c
SRC += $(SPHINCS_HW_DIR)/merkle.c
SRC += $(SPHINCS_HW_DIR)/wots.c
SRC += $(SPHINCS_HW_DIR)/wotsx1.c
SRC += $(SPHINCS_HW_DIR)/utils.c
SRC += $(SPHINCS_HW_DIR)/utilsx1.c
SRC += $(SPHINCS_HW_DIR)/fors.c
SRC += $(SPHINCS_HW_DIR)/sign.c
SRC += $(SPHINCS_HW_DIR)/randombytes.c
SRC += $(SPHINCS_HW_DIR)/fips202.c
SRC += $(SPHINCS_HW_DIR)/hash_shake.c
SRC += $(SPHINCS_HW_DIR)/thash_shake_$(THASH).c

INC = -I$(SPHINCS_HW_DIR)
INC += -I../../../common

OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef MASKING_EN
CFLAGS += -DMASKING_EN
SRC += $(SPHINCS_HW_DIR)/swmasked_fips202.c
SRC += $(SPHINCS_HW_DIR)/swmasked_fors.c
SRC += $(SPHINCS_HW_DIR)/swmasked_hash_shake.c
SRC += $(SPHINCS_HW_DIR)/swmasked_merkle.c
SRC += $(SPHINCS_HW_DIR)/swmasked_sign.c
SRC += $(SPHINCS_HW_DIR)/swmasked_utils.c
SRC += $(SPHINCS_HW_DIR)/swmasked_utilsx1.c
SRC += $(SPHINCS_HW_DIR)/swmasked_wots.c
SRC += $(SPHINCS_HW_DIR)/swmasked_wotsx1.c
SRC += $(SPHINCS_HW_DIR)/swmasked_thash_shake_$(THASH).c
endif

CFLAGS += -DPARAMS=$(PARAMS)
CFLAGS += -DSDK_BENCHMARK_ENABLED

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

