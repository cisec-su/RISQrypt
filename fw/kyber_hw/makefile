PROJ = kyber_hw

TOOLCHAIN_DIR = ../../sdk/toolchain

LIB_DIR = ../../sdk/lib



SRC =  main.c
SRC += indcpa.c
SRC += poly.c
SRC += polyvec.c
SRC += cbd.c
SRC += symmetric.c
SRC += rng.c
SRC += kem.c
SRC += verify.c
SRC += pack.c
SRC += masked_indcpa.c
SRC += masked_gadgets.c
SRC += masked_poly.c
SRC += masked_polyvec.c
SRC += masked_cbd.c
SRC += masked_symmetric.c
#SRC += test_kem.c
SRC += ../common/unity.c



INC = ../common


OBJ_DUMP = 1 # Uncomment if disassembly is needed

#CFLAGS += -DKYBER_K=$(KYBER_K) # Use as needed

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

