PROJ = kyber_cpapke_dec_cpa

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

CW305_DRIVER_DIR = ../../driver

SIMPLESERIAL_DIR = ../../simpleserial

KYBER_DIR = ../../../../fw/src/kyber

SRC = kyber_cpapke_dec_cpa.c
SRC += $(KYBER_DIR)/indcpa.c
SRC += $(KYBER_DIR)/poly.c
SRC += $(KYBER_DIR)/polyvec.c
SRC += $(KYBER_DIR)/pack.c
SRC += $(KYBER_DIR)/symmetric.c
SRC += $(KYBER_DIR)/cbd.c
ifdef MASK_EN
SRC += $(KYBER_DIR)/masked_indcpa.c
SRC += $(KYBER_DIR)/masked_poly.c
SRC += $(KYBER_DIR)/masked_polyvec.c
SRC += $(KYBER_DIR)/masked_symmetric.c
SRC += $(KYBER_DIR)/masked_cbd.c
SRC += $(KYBER_DIR)/masked_gadgets.c
endif

INC = -I$(KYBER_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef KYBER_K
CFLAGS += -DKYBER_K=$(KYBER_K)
endif

ifdef MASK_EN
CFLAGS += -DMASK_EN
endif

ifdef PRNG_OFF
CFLAGS += -DPRNG_OFF
endif

OPTIMIZATION = -O3

include $(CW305_DRIVER_DIR)/makefile

include $(SIMPLESERIAL_DIR)/makefile

include $(TOOLCHAIN_DIR)/makefile

