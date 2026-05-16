PROJ = rubato_code_size

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

RUBATO_DIR = ../../../src/rubato

SRC =  main.c
SRC += $(RUBATO_DIR)/masked_poly.c
SRC += $(RUBATO_DIR)/masked_rubato.c
SRC += $(RUBATO_DIR)/masked_gadgets.c
SRC += $(RUBATO_DIR)/symmetric.c
SRC += $(RUBATO_DIR)/poly.c
SRC += $(RUBATO_DIR)/rubato.c

INC = -I$(RUBATO_DIR)


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# CFLAGS += -DMEMORY_OPT_DIS
CFLAGS += -DREJ_SAMP_DIS

ifdef MASKING_EN
CFLAGS += -DMASKING_EN
endif

OPTIMIZATION = -O3 # Default is -O0


include $(TOOLCHAIN_DIR)/makefile

