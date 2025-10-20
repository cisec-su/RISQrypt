CC32 = /opt/riscv/bin/riscv64-unknown-elf
CC64 = /opt/riscv/bin/riscv64-unknown-elf

SIZE = /opt/riscv/bin/riscv64-unknown-elf-size

LINKER = $(TOOLCHAIN_DIR)/linksc.ld
PYTHON = python3
ROM_GEN = $(PYTHON) $(TOOLCHAIN_DIR)/rom_generator.py

LIB_INC = -I$(LIB_DIR)

SRC += $(TOOLCHAIN_DIR)/start.s
SRC += $(LIB_DIR)/util.c
SRC += $(LIB_DIR)/uart.c
SRC += $(LIB_DIR)/timer.c
SRC += $(LIB_DIR)/gpio.c
SRC += $(LIB_DIR)/keccak.c
SRC += $(LIB_DIR)/ntt_lite.c
SRC += $(LIB_DIR)/x2x.c



ifndef OPTIMIZATION
	OPTIMIZATION = -O0
endif

_CFLAGS = $(CFLAGS) $(OPTIMIZATION) $(INC) $(LIB_INC) -march=rv32im -mabi=ilp32 -mstrict-align -T $(LINKER) -flto -ffunction-sections -fdata-sections -nostartfiles -o $(PROJ).elf



build:
	@echo "$(_CFLAGS)"
	$(CC32)-gcc $(SRC) $(_CFLAGS)
	$(CC32)-objcopy -O binary -j .init   -j .text $(PROJ).elf $(PROJ).inst.bin
	$(CC32)-objcopy -O binary -j .rodata -j .data $(PROJ).elf $(PROJ).data.bin
	$(ROM_GEN) $(PROJ).bin $(PROJ).mem 
	@if [ -n "$(OBJ_DUMP)" ]; then \
		$(CC32)-objdump -d $(PROJ).elf > $(PROJ).txt; \
	fi
	$(SIZE) -B $(PROJ).elf

clean:
	rm -f *.elf *.bin *.mem *.data *.txt