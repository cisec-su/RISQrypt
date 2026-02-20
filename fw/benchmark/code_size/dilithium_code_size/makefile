PROJ = dilithium_code_size

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

DILITHIUM_DIR = ../../../src/dilithium

SRC =  main.c
SRC += $(DILITHIUM_DIR)/packing.c
SRC += $(DILITHIUM_DIR)/poly.c
SRC += $(DILITHIUM_DIR)/polyvec.c
SRC += $(DILITHIUM_DIR)/randombytes.c
SRC += $(DILITHIUM_DIR)/sign.c
SRC += $(DILITHIUM_DIR)/symmetric.c
SRC += $(DILITHIUM_DIR)/reduce.c
SRC += $(DILITHIUM_DIR)/masked.c
SRC += $(DILITHIUM_DIR)/masked_sign.c
SRC += $(DILITHIUM_DIR)/masked_poly.c
SRC += $(DILITHIUM_DIR)/masked_polyvec.c
SRC += $(DILITHIUM_DIR)/masked_gadgets.c
SRC += $(DILITHIUM_DIR)/masked_packing.c
SRC += $(DILITHIUM_DIR)/masked_symmetric.c

INC = -I$(DILITHIUM_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef MASKING_EN
CFLAGS += -DMASKING_EN
endif

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

