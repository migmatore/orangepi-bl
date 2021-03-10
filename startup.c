#include "uart.h"

typedef void (*entry_fn)();

void delay(int time) {
    for (int i = 0; i <= time * 0x10000; i++) {
        asm("nop");
    }
}

void startup() {
    serial_init(BAUD_115200);

    serial_puts("uart bootloader v1.0\n");
    //char a = serial_getc();

    serial_puts("\x16\x16\x16"); // ctrl + v
    //serial_puts("\x16");
    //serial_puts("\x16");
    

    uint32_t size = serial_getc();
    size |= serial_getc() << 8;
    size |= serial_getc() << 16;
    size |= serial_getc() << 24;

    serial_puts("OK");

    uint8_t *kernel = (uint8_t*)0x42001000;

    while(size-- > 0) {
        *kernel++ = serial_getc();
    }

    serial_puts("booting...\n");

    entry_fn fn = (entry_fn)0x42001000;
    fn();
}