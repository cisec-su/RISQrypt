PROJ = bootloader

CC32 = /opt/riscv/bin/riscv64-unknown-elf
CC64 = /opt/riscv/bin/riscv64-unknown-elf

LIB_DIR = ../../lib
TOOLCHAIN_DIR = ../../toolchain

PYTHON = python3
ROM_GEN = $(PYTHON) $(TOOLCHAIN_DIR)/rom_generator.py

LIB_INC = -I$(LIB_DIR)

SRC = bootloader.c
SRC += $(LIB_DIR)/uart.c
SRC += $(LIB_DIR)/irq.c
SRC += $(TOOLCHAIN_DIR)/start.s

CCFLAGS = -march=rv32i_zicsr -mabi=ilp32 -O3 $(LIB_INC) -nostartfiles -ffunction-sections -fdata-sections  -T linksc.ld -o  $(PROJ).elf

build:
	$(CC32)-gcc $(SRC) $(CCFLAGS)
	$(CC32)-objcopy -O binary -j .init -j .text -j .rodata  $(PROJ).elf  $(PROJ).bin
	$(ROM_GEN)  $(PROJ).bin  $(PROJ).mem
	$(CC32)-objdump -d  $(PROJ).elf >  $(PROJ).txt;

multilib:
	$(CC64)-gcc $(SRC) $(CCFLAGS)
	$(CC64)-objcopy -O binary -j .init -j .text -j .rodata  $(PROJ).elf  $(PROJ).bin
	$(ROM_GEN)  $(PROJ).bin  $(PROJ).mem
	$(CC32)-objdump -d  $(PROJ).elf >  $(PROJ).txt;

