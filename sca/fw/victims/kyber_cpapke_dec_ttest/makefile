PROJ = kyber_cpapke_dec_ttest

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

CW305_DRIVER_DIR = ../../driver

SIMPLESERIAL_DIR = ../../simpleserial

KYBER_HW_DIR = ../../../../fw/kyber_hw

SRC = kyber_cpapke_dec_ttest.c
SRC += $(KYBER_HW_DIR)/indcpa.c
SRC += $(KYBER_HW_DIR)/poly.c
SRC += $(KYBER_HW_DIR)/polyvec.c
SRC += $(KYBER_HW_DIR)/pack.c
SRC += $(KYBER_HW_DIR)/symmetric.c
SRC += $(KYBER_HW_DIR)/cbd.c
SRC += $(KYBER_HW_DIR)/masked_indcpa.c
SRC += $(KYBER_HW_DIR)/masked_poly.c
SRC += $(KYBER_HW_DIR)/masked_polyvec.c
SRC += $(KYBER_HW_DIR)/masked_symmetric.c
SRC += $(KYBER_HW_DIR)/masked_cbd.c
SRC += $(KYBER_HW_DIR)/masked_gadgets.c

INC = -I$(KYBER_HW_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef KYBER_K
CFLAGS += -DKYBER_K=$(KYBER_K)
endif

ifdef PRNG_OFF
CFLAGS += -DPRNG_OFF
endif

OPTIMIZATION = -O3

include $(CW305_DRIVER_DIR)/makefile

include $(SIMPLESERIAL_DIR)/makefile

include $(TOOLCHAIN_DIR)/makefile

