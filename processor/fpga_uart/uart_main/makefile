CC32=/opt/riscv/bin/riscv64-unknown-elf
CC64=/opt/riscv/bin/riscv64-unknown-elf
CCFLAGS=-march=rv32i -mabi=ilp32 -O0 -T linksc.ld -nostartfiles -o uart_main.elf

PROJ=uart_main
build:
	$(CC32)-gcc uart_main.c ../../../lib/uart.c crt0.s $(CCFLAGS)
	$(CC32)-objcopy -O binary -j .init -j .text -j .rodata uart_main.elf uart_main.bin
	python3 ../../../test/rom_generator.py uart_main.bin uart_main.data 
	mv uart_main.data uart_main.mem 
	$(CC32)-objdump -d $(PROJ).elf > dis-$(PROJ).txt

multilib:
	$(CC64)-gcc uart_main.c ../../../lib/uart.c crt0.s $(CCFLAGS)
	$(CC64)-objcopy -O binary -j .init -j .text -j .rodata uart_main.elf uart_main.bin
	python3 ../../../test/rom_generator.py uart_main.bin uart_main.data 
	mv uart_main.data uart_main.mem 
	$(CC32)-objdump -d $(PROJ).elf > dis-$(PROJ).txt
