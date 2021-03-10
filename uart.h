#include "types.h"
#include "ints.h"

#define NUM_UART	4

#define UART0_BASE	0x01C28000
#define UART1_BASE	0x01C28400
#define UART2_BASE	0x01C28800
#define UART3_BASE	0x01C28C00

#define UART_R_BASE	0x01F02800

struct h3_uart {
	vu32 data;	/* 00 - Rx/Tx data */
	vu32 ier;	/* 04 - interrupt enables */
	vu32 iir;	/* 08 - interrupt ID / FIFO control */
	vu32 lcr;	/* 0c - line control */
	vu32 mcr;	/* 10 - modem control */
	vu32 lsr;	/* 14 - line status */
	vu32 msr;	/* 18 - modem status */
};

static struct h3_uart * uart_base[] = {
    (struct h3_uart *) UART0_BASE,
    (struct h3_uart *) UART1_BASE,
    (struct h3_uart *) UART2_BASE,
    (struct h3_uart *) UART3_BASE,
    (struct h3_uart *) UART_R_BASE
};

#define divisor_msb	ier
#define divisor_lsb	data

/* It looks like the basic clock is 24 Mhz
 * We need 16 clocks per character.
 */
#define BAUD_115200   13 /* 24 * 1000 * 1000 / 16 / 115200 = 13 */
#define BAUD_38400    39 /* 24 * 1000 * 1000 / 16 / 38400 = 39 */
#define BAUD_19200    78 /* 24 * 1000 * 1000 / 16 / 19200 = 78 */
#define BAUD_9600    156 /* 24 * 1000 * 1000 / 16 / 9600 = 156 */


/* bits in the lsr */
#define RX_READY	0x01
#define TX_READY	0x40
#define TX_EMPTY	0x80

/* bits in the ier */
#define IE_RDA		0x01	/* Rx data available */
#define IE_TXE		0x02	/* Tx register empty */
#define IE_LS		0x04	/* Line status */
#define IE_MS		0x08	/* Modem status */
#define IE_THRE		0x80	/* Modem status */

#define LCR_DATA_5	0x00	/* 5 data bits */
#define LCR_DATA_6	0x01	/* 6 data bits */
#define LCR_DATA_7	0x02	/* 7 data bits */
#define LCR_DATA_8	0x03	/* 8 data bits */

#define LCR_STOP	0x04	/* extra (2) stop bits, else: 1 */
#define LCR_PEN		0x08	/* parity enable */

#define LCR_EVEN	0x10	/* even parity */
#define LCR_STICK	0x20	/* stick parity */
#define LCR_BREAK	0x40

#define LCR_DLAB	0x80	/* divisor latch access bit */

/* 8 bits, no parity, 1 stop bit */
#define LCR_SETUP	LCR_DATA_8

void uart_gpio_init ( int );
void uart_clock_init ( int );

void uart_init ( int uart, int baud );

// Polling loops like this could use a timeout maybe.
void uart_putc ( int uart, char c );
void uart_puts( int uart, char *s );

// Polling loops like this could use a timeout maybe.
int uart_getc(int uart);

/* These are the "standard" entry points used for the Kyu console */

/* Called from board.c */
void serial_init ( int baud );
void serial_putc ( char c );

void serial_puts ( char *s );

int serial_getc ( void );

/* Code below added for interrupt experiment, 7-29-2020 */

static struct serial_softc {
        struct h3_uart *base;
        int irq;
} serial_soft[NUM_UART];

static void serial_handler ( int devnum );

/* This series of routines are for
 * some quick testing to see if we can get UART interrupts,
 * we can simply enable TXE and we do get interrupts.
 * We can also nicely listen via interrupts to a port (works great)
 */

static void serial_listen ( int devnum );

static void aux_uart_init ( int devnum, int baud );


static void serial_setup ( int devnum, int irq, int baud );

/* Called from board.c */
void
serial_aux_init (void);