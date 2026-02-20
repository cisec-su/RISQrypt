PROJ = dilithium_sign_ttest

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

CW305_DRIVER_DIR = ../../driver

SIMPLESERIAL_DIR = ../../simpleserial

DILITHIUM_DIR = ../../../../fw/src/dilithium

VICTIMS_COMMONS_DIR = ../victims_commons


SRC = dilithium_sign_ttest.c
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
SRC += $(VICTIMS_COMMONS_DIR)/victims_commons_util.c
SRC += $(VICTIMS_COMMONS_DIR)/victims_commons_dilithium.c

INC = -I$(DILITHIUM_DIR) -I$(VICTIMS_COMMONS_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

CFLAGS += -DDILITHIUM_MODE=3
ifdef VERBOSE
CFLAGS += -DVERBOSE
endif

CFLAGS += -DTTEST
CFLAGS += -DBUSY_CHECK_DIS


OPTIMIZATION = -O3

include $(CW305_DRIVER_DIR)/makefile

include $(SIMPLESERIAL_DIR)/makefile

include $(TOOLCHAIN_DIR)/makefile

