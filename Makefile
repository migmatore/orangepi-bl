CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-T linker.ld -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 -fpic -ffreestanding -O3 -nostdlib -Wextra

uart_bl.bin: uart_bl.elf
	$(OBJCOPY) -O binary --remove-section .uncached uart_bl.elf uart_bl.bin
uart_bl.elf: boot.o startup.o uart.o gpio.o
	$(CC) $(CFLAGS) -o uart_bl.elf boot.o startup.o uart.o gpio.o
# os.elf: boot.o notmain.o
# 	$(CC) $(CFLAGS) -o os.elf boot.o notmain.o

all:
	make clean
	make 
	make dump

boot.o: boot.s
	$(CC) $(CFLAGS) -c boot.s
startup.o: startup.c
	$(CC) $(CFLAGS) -c startup.c
uart.o: uart.c
	$(CC) $(CFLAGS) -c uart.c
gpio.o: gpio.c
	$(CC) $(CFLAGS) -c gpio.c

DUMP = arm-none-eabi-objdump -d

dump: uart_bl.elf
	$(DUMP) uart_bl.elf > uart_bl.dump

clean:
	rm -f *.o os.*