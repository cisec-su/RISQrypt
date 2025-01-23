CC32 = /opt/riscv/bin/riscv64-unknown-elf
CC64 = /opt/riscv/bin/riscv64-unknown-elf

LINKER = $(TOOLCHAIN_DIR)/linksc.ld
PYTHON = python3
ROM_GEN = $(PYTHON) $(TOOLCHAIN_DIR)/rom_generator.py

LIB_INC = -I$(LIB_DIR)

SRC += $(TOOLCHAIN_DIR)/start.s
SRC += $(LIB_DIR)/uart.c
SRC += $(LIB_DIR)/keccak.c

ifndef OPTIMIZATION
	OPTIMIZATION = -O0
endif

_CFLAGS = $(CFLAGS) $(OPTIMIZATION) $(LIB_INC) -march=rv32i -mabi=ilp32 -mstrict-align -O0 -T $(LINKER) -nostartfiles -o $(PROJ).elf



build:
	@echo "$(_CFLAGS)"
	$(CC32)-gcc $(SRC) $(_CFLAGS)
	$(CC32)-objcopy -O binary -j .init -j .text -j .rodata -j .data $(PROJ).elf $(PROJ).bin
	$(ROM_GEN) $(PROJ).bin $(PROJ).mem 
	@if [ -n "$(OBJ_DUMP)" ]; then \
		$(CC32)-objdump -d $(PROJ).elf > $(PROJ).txt; \
	fi

clean:
	rm -f *.elf *.bin *.mem *.data *.txt