CC32=/opt/riscv/bin/riscv64-unknown-elf
CC64=/opt/riscv/bin/riscv64-unknown-elf
CCFLAGS=-march=rv32im -mabi=ilp32 -O3 -T linksc.ld -nostartfiles -o reset_handler.elf
PROJ=reset_handler
build:
	$(CC32)-gcc reset_handler.s $(CCFLAGS)
	$(CC32)-objcopy -O binary -j .init -j .text -j .rodata reset_handler.elf reset_handler.bin
	python3 ../../../test/rom_generator.py reset_handler.bin reset_handler.data 
	mv reset_handler.data reset_handler.mem 
	$(CC32)-objdump -d $(PROJ).elf > dis-$(PROJ).txt

multilib:
	$(CC64)-gcc reset_handler.s $(CCFLAGS)
	$(CC64)-objcopy -O binary -j .init -j .text -j .rodata reset_handler.elf reset_handler.bin
	python3 ../../../test/rom_generator.py reset_handler.bin reset_handler.data 
	mv reset_handler.data reset_handler.mem 