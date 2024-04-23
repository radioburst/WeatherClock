/*************************************************************************

	Title:    Interrupt UART library with receive/transmit circular buffers
	Author:   Andy Gock
	Software: AVR-GCC 4.1, AVR Libc 1.4
	Hardware: any AVR with built-in UART, tested on AT90S8515 & ATmega8 at 4 Mhz
	License:  GNU General Public License
	Usage:    see README.md and Doxygen manual

	Based on original library by Peter Fluery, Tim Sharpe, Nicholas Zambetti.

	https://github.com/andygock/avr-uart

	Updated UART library (this one) by Andy Gock
	https://github.com/andygock/avr-uart

	Based on updated UART library (this one) by Tim Sharpe
	http://beaststwo.org/avr-uart/index.shtml

	Based on original library by Peter Fluery
	http://homepage.hispeed.ch/peterfleury/avr-software.html

*************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "uart.h"

/*
 *  constants and macros
 */
	//__AVR_ATmega644__
	/* ATmega with one USART */
#define ATMEGA_USART0
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0

/*************************************************************************
Function: uart0_init()
Purpose:  initialize UART and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
void uart0_init(uint16_t baudrate)
{
	
	/* Set baud rate */
	if (baudrate & 0x8000) {
		UART0_STATUS = (1<<U2X0);  //Enable 2x speed
		baudrate &= ~0x8000;
	}
	UBRR0H = (uint8_t)(baudrate>>8);
	UBRR0L = (uint8_t) baudrate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = _BV(RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

} /* uart0_init */

/*************************************************************************
Function: uart0_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none
**************************************************************************/
void uart0_putc(uint8_t data)
{
	UART0_DATA = data;										/* Write data to be transmitting in UDR */
	while (!(UCSR0A & (1<<UDRIE0)));
} /* uart0_putc */


/*************************************************************************
Function: uart0_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none
**************************************************************************/
void uart0_puts(const char *s)
{
	while (*s) {
		uart0_putc(*s++);
	}

} /* uart0_puts */


/*************************************************************************
Function: uart0_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart0_puts_p(const char *progmem_s)
{
	register char c;

	while ((c = pgm_read_byte(progmem_s++))) {
		uart0_putc(c);
	}

} /* uart0_puts_p */

