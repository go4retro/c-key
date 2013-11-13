/*
    C=Key - Commodore <-> PS/2 Interface
    Copyright Jim Brain and RETRO Innovations, 2004-2012

    This code is a modification of uart functions in sd2iec:
    Copyright (C) 2007,2008  Ingo Korb <ingo@akana.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License only.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    uart.h: Definitions for the UART access routines

*/

#ifndef UART_H
#define UART_H

#ifdef UART_DOUBLE_SPEED
#define CALC_BPS(x) (int)((double)F_CPU/(8.0*x)-1)
#else
#define CALC_BPS(x) (int)((double)F_CPU/(16.0*x)-1)
#endif

#define B0300   CALC_BPS(300)
#define B0600   CALC_BPS(600)
#define B1200   CALC_BPS(1200)
#define B2400   CALC_BPS(2400)
#define B4800   CALC_BPS(4800)
#define B9600   CALC_BPS(9600)
#define B19200	CALC_BPS(19200)
#define B38400	CALC_BPS(38400)
#define B57600  CALC_BPS(57600)
#define B76800	CALC_BPS(76800)
#define B115200	CALC_BPS(115200)
#define B230400	CALC_BPS(230400)
#define B460800	CALC_BPS(460800)
#define B921600	CALC_BPS(921600)

#if defined __AVR_ATmega162__ || defined __AVR_ATmega644__ || defined __AVR_ATmega644P__ || defined __AVR_ATmega1281__ || defined __AVR_ATmega2561__ || defined __AVR_ATmega128__

#  ifdef SWAP_UART
#    define UDREA  UDRE1
#    define RXCIEA RXCIE1
#    define TXCIEA TXCIE1
#    define RXENA  RXEN1
#    define TXENA  TXEN1
#    define UCSZA0 UCSZ10
#    define UCSZA1 UCSZ11
#    define UPMA0  UPM10
#    define UPMA1  UPM11
#    define USBSA  USBS1
#    define URSELA URSEL1
#    define RXCA   RXC1
#    define RXENA  RXEN1
#    define TXCA   TXC1
#    define TXENA  TXEN1
#    define UBRRAH UBRR1H
#    define UBRRAL UBRR1L
#    define UCSRAA UCSR1A
#    define UCSRAB UCSR1B
#    define UCSRAC UCSR1C
#    define UDRA   UDR1
#    define UDRIEA UDRIE1
#    define U2XA   U2X1
#    define USARTA_UDRE_vect USART1_UDRE_vect
#    define USARTA_RXC_vect USART1_RXC_vect
#    define UDREB  UDRE0
#    define RXCIEB RXCIE0
#    define TXCIEB TXCIE0
#    define RXENB  RXEN0
#    define TXENB  TXEN0
#    define UCSZB0 UCSZ00
#    define UCSZB1 UCSZ01
#    define UPMB0  UPM00
#    define UPMB1  UPM01
#    define USBSB  USBS0
#    define URSELB URSEL0
#    define RXCB   RXC0
#    define TXCB   TXC0
#    define UBRRBH UBRR0H
#    define UBRRBL UBRR0L
#    define UCSRBA UCSR0A
#    define UCSRBB UCSR0B
#    define UCSRBAC UCSR0C
#    define UDRB   UDR0
#    define UDRIEB UDRIE0
#    define U2XB   U2X0
#    define USARTB_UDRE_vect USART0_UDRE_vect
#    define USARTB_RXC_vect USART0_RXC_vect

#  else
     /* Default is USART0 */
#    define UDREA  UDRE0
#    define RXCIEA RXCIE0
#    define TXCIEA TXCIE0
#    define RXENA  RXEN0
#    define TXENA  TXEN0
#    define UCSZA0 UCSZ00
#    define UCSZA1 UCSZ01
#    define UPMA0  UPM00
#    define UPMA1  UPM01
#    define USBSA  USBS0
#    define URSELA URSEL0
#    define RXCA   RXC0
#    define RXENA  RXEN0
#    define TXCA   TXC0
#    define TXENA  TXEN0
#    define UBRRAH UBRR0H
#    define UBRRAL UBRR0L
#    define UCSRAA UCSR0A
#    define UCSRAB UCSR0B
#    define UCSRAC UCSR0C
#    define UDRA   UDR0
#    define UDRIEA UDRIE0
#    define U2XA   U2X0
#    define USARTA_UDRE_vect USART0_UDRE_vect
#    define USARTA_RXC_vect USART0_RXC_vect
#    define UDREB  UDRE1
#    define RXCIEB RXCIE1
#    define TXCIEB TXCIE1
#    define RXENB  RXEN1
#    define TXENB  TXEN1
#    define UCSZB0 UCSZ10
#    define UCSZB1 UCSZ11
#    define UPMB0  UPM10
#    define UPMB1  UPM11
#    define USBSB  USBS1
#    define URSELB URSEL1
#    define RXCB   RXC1
#    define TXCB   TXC1
#    define UBRRBH UBRR1H
#    define UBRRBL UBRR1L
#    define UCSRBA UCSR1A
#    define UCSRBB UCSR1B
#    define UCSRBC UCSR1C
#    define UDRB   UDR1
#    define UDRIEB UDRIE1
#    define U2XB   U2X1
#    define USARTB_UDRE_vect USART1_UDRE_vect
#    define USARTB_RXC_vect USART1_RXC_vect
#  endif

#elif defined __AVR_ATmega28__ || defined __AVR_ATmega48__ || defined __AVR_ATmega88__ || defined __AVR_ATmega168__
#  define UCSZA0 UCSZ00
#  define UCSZA1 UCSZ01
#  define UPMA0  UPM00
#  define UPMA1  UPM01
#  define USBSA  USBS0
#  define UCSZB0 UCSZ10
#  define UCSZB1 UCSZ11
#  define UPMB0  UPM10
#  define UPMB1  UPM11
#  define USBSB  USBS1

#  define UDRA  UDR0
#  define RXCA   RXC0
#  define RXENA  RXEN0
#  define TXCA   TXC0
#  define TXENA  TXEN0
#  define UBRRAH UBRR0H
#  define UBRRAL UBRR0L
#  define UCSRAA UCSR0A
#  define UCSRAB UCSR0B
#  define UCSRAC UCSR0C
#  define UDRIEA UDRIE0
#  define U2XA   U2X0
#  define USARTA_UDRE_vect USART_UDRE_vect
#  define USARTA_RXC_vect USART_RXC_vect

#elif defined __AVR_ATtiny2313__ || defined __AVR_ATtiny4313__ || defined __AVR_ATmega165__ || defined __AVR_ATmega165A__ || defined __AVR_ATmega165P__ || defined __AVR_ATmega165PA__ || defined __AVR_ATmega32__ || defined __AVR_ATmega16__ || defined __AVR_ATmega8__
// only 1 uart
#    define UDRA   UDR
#    define RXCA   RXC
#    define RXENA  RXEN
#    define TXCA   TXC
#    define TXENA  TXEN
#    define UBRRAH UBRRH
#    define UBRRAL UBRRL
#    define UCSRAA UCSRA
#    define UCSRAB UCSRB
#    define UCSRAC UCSRC
#    define UDRIEA UDRIE
#    define URSELA URSEL
#    define U2XA   U2X
#    if defined __AVR_ATmega165__ || defined __AVR_ATmega165A__ || defined __AVR_ATmega165P__ || defined __AVR_ATmega165PA__
#      define USARTA_UDRE_vect USART0_UDRE_vect
#      define USARTA_RXC_vect USART0_RXC_vect
#    else
#      define USARTA_UDRE_vect USART_UDRE_vect
#      define USARTA_RXC_vect USART_RXC_vect
#    endif
#    define USBSA  USBS
#    define UCSZA0 UCSZ0
#    define UCSZA1 UCSZ1
#    define UPMA0  UPM0
#    define UPMA1  UPM1

#else
#  error Unknown chip!
#endif

#if defined __AVR_ATmega8__ || defined __AVR_ATmega16__ || defined __AVR_ATmega32__  || defined __AVR_ATmega162__
#  define UART0_CONFIG(l,p,s) do{\
                                 uint8_t __tmp; \
                                 ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { \
                                 __tmp = UCSRAC; \
                                 __tmp = UCSRAC & (uint8_t)~(UART_LENGTH_MASK | UART_PARITY_MASK | UART_STOP_MASK); \
                                 UCSRAC = __tmp | _BV(URSELA) | (l & UART_LENGTH_MASK) | (p & UART_PARITY_MASK) | (s & UART_STOP_MASK); \
                                 } \
                                } while(0)
#  define UART0_MODE_SETUP()  do { UCSRAC = _BV(URSELA) | _BV(UCSZA1) | _BV(UCSZA0); } while(0)
#  if defined __AVR_ATmega162__
#    define UART1_CONFIG(l,p,s) do{\
                                   uint8_t __tmp; \
                                   ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { \
                                   __tmp = UCSRBC; \
                                   __tmp = UCSRBC & (uint8_t)~(UART_LENGTH_MASK | UART_PARITY_MASK | UART_STOP_MASK); \
                                   UCSRBC = __tmp | _BV(URSELB) | (l & UART_LENGTH_MASK) | (p & UART_PARITY_MASK) | (s & UART_STOP_MASK); \
                                   } \
                                  } while(0)
#    define UART1_MODE_SETUP()  do { UCSRBC = _BV(URSELB) | _BV(UCSZB1) | _BV(UCSZB0); } while(0)
#  endif
#else
#  define UART0_CONFIG(l,p,s) do{\
                                 UCSRAC = (UCSRAC & (uint8_t)~(UART_LENGTH_MASK | UART_PARITY_MASK | UART_STOP_MASK)) |\
                                          (l & UART_LENGTH_MASK) | (p & UART_PARITY_MASK) | (s & UART_STOP_MASK); \
                                } while(0)
#  define UART1_CONFIG(l,p,s) do{\
                                 UCSRBC = (UCSRBC & (uint8_t)~(UART_LENGTH_MASK | UART_PARITY_MASK | UART_STOP_MASK)) |\
                                          (l & UART_LENGTH_MASK) | (p & UART_PARITY_MASK) | (s & UART_STOP_MASK); \
                                } while(0)
#  define UART0_MODE_SETUP()  do { UCSRAC = _BV(UCSZA1) | _BV(UCSZA0); } while(0)
#  define UART1_MODE_SETUP()  do { UCSRBC = _BV(UCSZB1) | _BV(UCSZB0); } while(0)
#endif

#if defined ENABLE_UART0 || defined ENABLE_UART1
#include <avr/pgmspace.h>
void uart_init(void);
uint8_t uart_getc(void);
void uart_putc(uint8_t c);
void uart_puthex(uint8_t hex);
void uart_trace(void *ptr, uint16_t start, uint16_t len);
void uart_flush(void);
//void uart_puts_P(prog_char *text);
void uart_puts_P(const char *text);
uint8_t uart_data_available(void);
void uart_putcrlf(void);

#else
#  define uart_init()           do {} while(0)
#define uart_getc()             0
#define uart_putc(x)            do {} while(0)
#define uart_puthex(x)          do {} while(0)
#define uart_trace(x,y,z)       do {} while(0)
#define uart_flush()            do {} while(0)
#define uart_puts_P(x)          do {} while(0)
#define uart_putcrlf()          do {} while(0)

#endif

#if defined ENABLE_UART0
uint8_t uart0_getc(void);
void uart0_putc(uint8_t data);
void uart_puthex(uint8_t hex);
void uart_trace(void *ptr, uint16_t start, uint16_t len);
void uart0_flush(void);
//void uart0_puts_P(prog_char *text);
void uart0_puts_P(const char *text);
uint8_t uart0_data_available(void);
void uart0_putcrlf(void);
#  include <stdio.h>
#  define dprintf(str,...) printf_P(PSTR(str), ##__VA_ARGS__)
#else
#  define uart0_getc()           0
#  define uart0_putc(x)          do {} while(0)
#  define uart0_puthex(x)        do {} while(0)
#  define uart_trace(x,y,z)      do {} while(0)
#  define uart0_puts_P(x)        do {} while(0)
#  define uart0_data_available() do {} while(0)
#  define uart0_putcrlf()        do {} while(0)
#endif

#ifdef ENABLE_UART1
uint8_t uart1_getc(void);
void uart1_putc(char c);
void uart1_puts(char* str);
#else
#  define uart1_getc()    0
#  define uart1_putc(x)   do {} while(0)
#  define uart1_puts(x)   do {} while(0)
#endif

#define UART_LENGTH_MASK   (_BV(UCSZA1) | _BV(UCSZA0))
#define UART_LENGTH_5      0
#define UART_LENGTH_6      _BV(UCSZA0)
#define UART_LENGTH_7      _BV(UCSZA1)
#define UART_LENGTH_8      UART_LENGTH_MASK

#define UART_PARITY_MASK   (_BV(UPMA1) | _BV(UPMA0))
#define UART_PARITY_NONE   0
#define UART_PARITY_EVEN   _BV(UPMA1)
#define UART_PARITY_ODD    UART_PARITY_MASK

#define UART_STOP_MASK     _BV(USBSA)
#define UART_STOP_1        0
#define UART_STOP_2        UART_STOP_MASK

typedef enum {STOP_0 = UART_STOP_1,
              STOP_1 = UART_STOP_2,
             } uartstop_t;

typedef enum {LENGTH_5 = UART_LENGTH_5,
              LENGTH_6 = UART_LENGTH_6,
              LENGTH_7 = UART_LENGTH_7,
              LENGTH_8 = UART_LENGTH_8
             } uartlen_t;

typedef enum {PARITY_NONE = UART_PARITY_NONE,
              PARITY_EVEN = UART_PARITY_EVEN,
              PARITY_ODD = UART_PARITY_ODD
             } uartpar_t;

#if defined ENABLE_UART0 && defined DYNAMIC_UART
void uart_config(uint16_t rate, uartlen_t length, uartpar_t parity, uartstop_t stopbits);
#else
#define uart_config(bps, length, parity, stopbits) do {} while(0)
#endif

#if defined ENABLE_UART1 && defined DYNAMIC_UART
void uart1_config(uint16_t rate, uartlen_t length, uartpar_t parity, uartstop_t stopbits);
#else
#define uart1_config(bps, length, parity, stopbits) do {} while(0)
#endif

#endif
