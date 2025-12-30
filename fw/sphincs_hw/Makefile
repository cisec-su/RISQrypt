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

# Random number generator - The default randombytes.c uses /dev/urandom which is not suitable for embedded.
# You may need to provide a custom randombytes.c or rng.c compatible with your hardware.
SRC += randombytes.c

# Conditional sources based on PARAMS
ifneq (,$(findstring shake,$(PARAMS)))
	SRC += fips202.c hash_shake.c thash_shake_$(THASH).c
endif
ifneq (,$(findstring haraka,$(PARAMS)))
	SRC += haraka.c hash_haraka.c thash_haraka_$(THASH).c
endif
ifneq (,$(findstring sha2,$(PARAMS)))
	SRC += sha2.c hash_sha2.c thash_sha2_$(THASH).c
endif

# Main file - Uncomment if you have a main.c
SRC += main.c

# Common files
SRC += ../common/unity.c

# Include directories
INC = -I../common

OBJ_DUMP = 1 # Uncomment if disassembly is needed

OPTIMIZATION = -O3 # Default is -O0

# Pass PARAMS to C code
CFLAGS += -DPARAMS=$(PARAMS)

include $(TOOLCHAIN_DIR)/makefile
