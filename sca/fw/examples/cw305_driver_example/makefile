PROJ = cw305_driver_example

TOOLCHAIN_DIR = ../../../../sdk/toolchain

LIB_DIR = ../../../../sdk/lib

CW305_DRIVER_DIR = ../../driver

SRC = cw305_driver_example.c


OBJ_DUMP = 1 # Uncomment if disassembly is needed

# CFLAGS = # Use as needed

ifdef USE_DONE
CFLAGS += -DUSE_DONE
endif

ifdef BUFF_LEN
CFLAGS += -DBUFF_LEN=$(BUFF_LEN)
endif


# OPTIMIZATION = -O2 # Default is -O0

include $(CW305_DRIVER_DIR)/makefile

include $(TOOLCHAIN_DIR)/makefile

