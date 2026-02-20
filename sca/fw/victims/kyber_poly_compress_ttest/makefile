PROJ = kyber_poly_compress_ttest

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

CW305_DRIVER_DIR = ../../driver

SIMPLESERIAL_DIR = ../../simpleserial

KYBER_DIR = ../../../../fw/src/kyber

VICTIMS_COMMONS_DIR = ../victims_commons


SRC = kyber_poly_compress_ttest.c
SRC += $(KYBER_DIR)/indcpa.c
SRC += $(KYBER_DIR)/poly.c
SRC += $(KYBER_DIR)/polyvec.c
SRC += $(KYBER_DIR)/pack.c
SRC += $(KYBER_DIR)/symmetric.c
SRC += $(KYBER_DIR)/cbd.c
SRC += $(KYBER_DIR)/masked_indcpa.c
SRC += $(KYBER_DIR)/masked_poly.c
SRC += $(KYBER_DIR)/masked_polyvec.c
SRC += $(KYBER_DIR)/masked_symmetric.c
SRC += $(KYBER_DIR)/masked_cbd.c
SRC += $(KYBER_DIR)/masked_gadgets.c
SRC += $(KYBER_DIR)/kem.c
SRC += $(KYBER_DIR)/masked_kem.c
SRC += $(KYBER_DIR)/verify.c
SRC += $(VICTIMS_COMMONS_DIR)/victims_commons_util.c
SRC += $(VICTIMS_COMMONS_DIR)/victims_commons_kyber.c

INC = -I$(KYBER_DIR) -I$(VICTIMS_COMMONS_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

CFLAGS += -DKYBER_K=3
ifdef VERBOSE
CFLAGS += -DVERBOSE
endif


OPTIMIZATION = -O3

include $(CW305_DRIVER_DIR)/makefile

include $(SIMPLESERIAL_DIR)/makefile

include $(TOOLCHAIN_DIR)/makefile

