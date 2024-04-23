#ifndef UART_H
#define UART_H

/************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Andy Gock
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: any AVR with built-in UART, tested on AT90S8515 & ATmega8 at 4 Mhz
License:  GNU General Public License
Usage:    see README.md and Doxygen manual

Based on original library by Peter Fluery, Tim Sharpe, Nicholas Zambetti.

https://github.com/andygock/avr-uart

@{*/
#include <stdint.h>
#include <avr/io.h>

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif

/*
 * constants and macros
 */

/* Enable USART 1, 2, 3 as required */
/* Can be defined in compiler symbol setup with -D option (preferred) */
#define USART0_ENABLED /**< Enable USART0 */

/** @brief  UART Baudrate Expression
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)
//#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu/(16UL*baudRate))-1UL)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000L for 4Mhz
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) ((((xtalCpu)+4UL*(baudRate))/(8UL*(baudRate))-1)|0x8000)

/* Macros, to allow use of legacy names */

/** @brief Macro to initialize USART0 (only available on selected ATmegas) @see uart0_init */
#define uart_init(b)      uart0_init(b)

/** @brief Macro to put byte to ringbuffer for transmitting via USART0 (only available on selected ATmega) @see uart0_putc */
#define uart_putc(d)      uart0_putc(d)

/** @brief Macro to put string to ringbuffer for transmitting via USART0 (only available on selected ATmega) @see uart0_puts */
#define uart_puts(s)      uart0_puts(s)

/** @brief Macro to put string from program memory to ringbuffer for transmitting via USART0 (only available on selected ATmega) @see uart0_puts_p */
#define uart_puts_p(s)    uart0_puts_p(s)

/*
** function prototypes
*/

/**
   @brief   Initialize UART and set baudrate
   @param   baudrate Specify baudrate using macro UART_BAUD_SELECT()
   @return  none
*/
extern void uart0_init(uint16_t baudrate);

/**
 *  @brief   Put byte to ringbuffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uart0_putc(uint8_t data);


/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 *
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uart0_puts(const char *s);


/**
 * @brief    Put string from program memory to ringbuffer for transmitting via UART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart0_puts_P
 */
extern void uart0_puts_p(const char *s);

#endif