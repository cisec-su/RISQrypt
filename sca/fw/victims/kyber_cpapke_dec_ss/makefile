PROJ = kyber_cpapke_dec_ss

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

CW305_DRIVER_DIR = ../../driver

SIMPLESERIAL_DIR = ../../simpleserial

KYBER_HW_DIR = ../../../../fw/kyber_hw

SRC = kyber_cpapke_dec_ss.c
SRC += $(KYBER_HW_DIR)/indcpa.c
SRC += $(KYBER_HW_DIR)/poly.c
SRC += $(KYBER_HW_DIR)/polyvec.c
SRC += $(KYBER_HW_DIR)/pack.c
SRC += $(KYBER_HW_DIR)/symmetric.c
SRC += $(KYBER_HW_DIR)/cbd.c

INC = -I$(KYBER_HW_DIR)

OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef KYBER_K
CFLAGS += -DKYBER_K=$(KYBER_K)
endif


OPTIMIZATION = -O3

include $(CW305_DRIVER_DIR)/makefile

include $(SIMPLESERIAL_DIR)/makefile

include $(TOOLCHAIN_DIR)/makefile

