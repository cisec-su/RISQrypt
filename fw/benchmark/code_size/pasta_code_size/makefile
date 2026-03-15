PROJ = pasta_code_size

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

PASTA_HW_DIR = ../../../pasta_hw

SRC =  main.c
SRC += $(PASTA_HW_DIR)/masked_poly.c
SRC += $(PASTA_HW_DIR)/masked_pasta.c
SRC += $(PASTA_HW_DIR)/masked_gadgets.c
SRC += $(PASTA_HW_DIR)/symmetric.c
SRC += $(PASTA_HW_DIR)/poly.c
SRC += $(PASTA_HW_DIR)/pasta.c

INC = -I$(PASTA_HW_DIR)


OBJ_DUMP = 1 # Uncomment if disassembly is needed

ifdef MASKING_EN
CFLAGS += -DMASKING_EN
endif

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

