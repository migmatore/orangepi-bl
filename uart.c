#include "uart.h"

void
uart_init ( int uart, int baud )
{
	struct h3_uart *up = uart_base[uart];

	uart_gpio_init ( uart );
	uart_clock_init ( uart );

	up->ier = 0;
	up->lcr = LCR_DLAB;

	up->divisor_msb = 0;
	up->divisor_lsb = BAUD_115200;

	up->lcr = LCR_SETUP;

	// No sense if we aren't using interrupts.
	// up->ier = IE_RDA | IE_TXE;
}

// Polling loops like this could use a timeout maybe.
void
uart_putc ( int uart, char c )
{
	struct h3_uart *up = uart_base[uart];

	while ( !(up->lsr & TX_READY) )
	    ;
	up->data = c;

	if ( c == '\n' )
	    uart_putc ( uart, '\r' );
}

void
uart_puts ( int uart, char *s )
{
	while ( *s ) {
	    if (*s == '\n')
		uart_putc ( uart, '\r' );
	    uart_putc ( uart, *s++ );
	}
}

// Polling loops like this could use a timeout maybe.
int uart_getc ( int uart )
{
	struct h3_uart *up = uart_base[uart];

	while ( ! (up->lsr & RX_READY) )
	    ;
	return up->data;
}

/* These are the "standard" entry points used for the Kyu console */

/* Called from board.c */
void
serial_init ( int baud )
{
	uart_init ( 0, baud );
}

void
serial_putc ( char c )
{
	uart_putc ( 0, c );
}

void
serial_puts ( char *s )
{
	uart_puts ( 0, s );
}

int
serial_getc ( void )
{
	return uart_getc ( 0 );
}

/* Code below added for interrupt experiment, 7-29-2020 */

// static void
// serial_handler ( int devnum )
// {
//         struct serial_softc *sc = &serial_soft[devnum];
//         struct h3_uart *base = sc->base;
// 	int x;

// 	/* poll data ready */
// 	while ( base->lsr & 0x1 ) {
// 	    x = base->data;
// 	    putchar ( x & 0x7f );
// 	}

// 	/* Disable any more interrupts.
// 	 * OK for some testing,
// 	 * but needs to be more fancy in the future
// 	 */
// 	// base->ier &= ~(IE_TXE | IE_THRE);
// 	base->ier &= IE_RDA;
// 	// printf ( " ** Uart interrupt, lsr = %08x\n", base->lsr );
// }

/* This series of routines are for
 * some quick testing to see if we can get UART interrupts,
 * we can simply enable TXE and we do get interrupts.
 * We can also nicely listen via interrupts to a port (works great)
 */

static void
serial_listen ( int devnum )
{
        struct serial_softc *sc = &serial_soft[devnum];
        struct h3_uart *base = sc->base;

	base->ier = IE_RDA;
}

// static void
// aux_uart_init ( int devnum, int baud )
// {
// 	struct h3_uart *up = uart_base[devnum];

// 	up->ier = 0;
// 	up->lcr = LCR_DLAB;

// 	up->divisor_msb = 0;
// 	up->divisor_lsb = baud;

// 	up->lcr = LCR_SETUP;

// 	// up->ier = IE_RDA | IE_TXE;
// 	up->ier = IE_TXE;
// }


// static void
// serial_setup ( int devnum, int irq, int baud )
// {
//     struct serial_softc *sc = &serial_soft[devnum];

//     irq_hookup ( irq, serial_handler, devnum );

// 	sc->base = uart_base[devnum];
// 	sc->irq = irq;

// 	aux_uart_init ( devnum, baud );
// 	uart_gpio_init ( devnum );
// }

// /* Called from board.c */
// void
// serial_aux_init ( void )
// {
// 	/* XXX - redundant with uart_clock_init () in gpio.c */
// 	serial_clocks_on ();

//         // serial_setup ( 0, IRQ_UART0, BAUD_115200 );
//     serial_setup ( 1, IRQ_UART1, BAUD_9600 );
//     serial_setup ( 2, IRQ_UART2, BAUD_9600 );
//     serial_setup ( 3, IRQ_UART3, BAUD_9600 );

// 	// serial_listen ( 1 );
// }