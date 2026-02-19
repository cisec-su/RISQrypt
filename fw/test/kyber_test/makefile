PROJ = kyber_test

TOOLCHAIN_DIR = ../../../sdk/toolchain

LIB_DIR = ../../../sdk/lib

KYBER_DIR = ../../kyber

SRC =  main.c
SRC += $(KYBER_DIR)/indcpa.c
SRC += $(KYBER_DIR)/poly.c
SRC += $(KYBER_DIR)/polyvec.c
SRC += $(KYBER_DIR)/cbd.c
SRC += $(KYBER_DIR)/symmetric.c
SRC += $(KYBER_DIR)/rng.c
SRC += $(KYBER_DIR)/kem.c
SRC += $(KYBER_DIR)/verify.c
SRC += $(KYBER_DIR)/pack.c
SRC += $(KYBER_DIR)/masked_indcpa.c
SRC += $(KYBER_DIR)/masked_gadgets.c
SRC += $(KYBER_DIR)/masked_poly.c
SRC += $(KYBER_DIR)/masked_polyvec.c
SRC += $(KYBER_DIR)/masked_cbd.c
SRC += $(KYBER_DIR)/masked_symmetric.c
SRC += $(KYBER_DIR)/masked_kem.c
SRC += ../../common/unity.c

INC = -I$(KYBER_DIR)
INC += -I../../common

CFLAGS += -DBUSY_CHECK_DIS
CFLAGS += -DTEST_RNG


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# CFLAGS = # Use as needed

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

