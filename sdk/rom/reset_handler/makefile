PROJ = reset_handler

CC32 = /opt/riscv/bin/riscv64-unknown-elf
CC64 = /opt/riscv/bin/riscv64-unknown-elf

TOOLCHAIN_DIR = ../../toolchain

PYTHON = python3
ROM_GEN = $(PYTHON) $(TOOLCHAIN_DIR)/rom_generator.py

CCFLAGS=-march=rv32im -mabi=ilp32 -O3 $(LIB_INC) -T linksc.ld -nostartfiles -o $(PROJ).elf

build:
	$(CC32)-gcc $(PROJ).s $(CCFLAGS)
	$(CC32)-objcopy -O binary -j .init -j .text -j .rodata $(PROJ).elf $(PROJ).bin
	$(ROM_GEN) $(PROJ).bin $(PROJ).mem 
	$(CC32)-objdump -d $(PROJ).elf > dis-$(PROJ).txt

multilib:
	$(CC64)-gcc $(PROJ).s $(CCFLAGS)
	$(CC64)-objcopy -O binary -j .init -j .text -j .rodata $(PROJ).elf $(PROJ).bin
	$(ROM_GEN) $(PROJ).bin $(PROJ).mem 
	$(CC32)-objdump -d $(PROJ).elf > dis-$(PROJ).txt

clean:
	rm -f *.elf *.bin *.mem *.data *.txt