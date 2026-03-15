PROJ = pasta_sw_code_size

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

PASTA_SW_DIR = ../../../pasta_sw

SRC =  main.c
SRC += $(PASTA_SW_DIR)/fips202.c
SRC += $(PASTA_SW_DIR)/pasta_soft.c
SRC += $(PASTA_SW_DIR)/masked_pasta_soft.c
SRC += ../../../kyber_asm/keccakf1600_asm.S

INC = -I$(PASTA_SW_DIR)


OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef MASKING_EN
CFLAGS += -DMASKING_EN
endif

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

