/*
    Copyright Jim Brain and Brain Innovations, 2004
  
    This file is part of C=Key.

    C=Key is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    C=Key is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with C=Key; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef USART_H
#define USART_H 1

#if defined __AVR_ATmega16__
// for CPUs with 1 USART
#define UCSR0A  UCSRA
#define UDRE0   UDRE
#define RXC0    RXC
#define UBRR0H  UBRRH
#define UBRR0L  UBRRL
#define UCSR0B  UCSRB
#define UCSR0C  UCSRC
#define UDR0    UDR
#define UDRIE0  UDRIE
#define RXCIE0  RXCIE
#define RXEN0   RXEN
#define TXEN0   TXEN
#define USART0_RXC_vect USART_RXC_vect
#define USART0_TXC_vect USART_TXC_vect
#define USART0_UDRE_vect USART_UDRE_vect
#endif

#define UART0_RX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART0_TX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#if UART0_RX_BUFFER_SIZE > 0
#  define UART0_RX_BUFFER_MASK ( UART0_RX_BUFFER_SIZE - 1 )
#  if ( UART0_RX_BUFFER_SIZE & UART0_RX_BUFFER_MASK )
#    error RX buffer size is not a power of 2
#  endif
#endif
#if UART0_TX_BUFFER_SIZE > 0
#define UART0_TX_BUFFER_MASK ( UART0_TX_BUFFER_SIZE - 1 )
#  if ( UART0_TX_BUFFER_SIZE & UART0_TX_BUFFER_MASK )
#    error TX buffer size is not a power of 2
#  endif
#endif

#ifdef UART_DOUBLE_SPEED
#define CALC_BPS(x) ((double)F_CPU/(8.0*x)-1)
#else
#define CALC_BPS(x) ((double)F_CPU/(16.0*x)-1)
#endif

#define B0300	  CALC_BPS(300)
#define B0600	  CALC_BPS(600)
#define B1200	  CALC_BPS(1200)
#define B2400	  CALC_BPS(2400)
#define B4800	  CALC_BPS(4800)
#define B9600	  CALC_BPS(9600)
#define B19200	CALC_BPS(19200)
#define B38400	CALC_BPS(38400)
#define B57600  CALC_BPS(57600)
#define B76800	CALC_BPS(76800)
#define B115200	CALC_BPS(115200)
#define B230400	CALC_BPS(230400)
#define B460800	CALC_BPS(460800)
#define B921600	CALC_BPS(921600)

#ifndef UART0_BAUDRATE
void uart0_init(uint16_t baudrate);
#else
void uart0_init(void);
#endif
uint8_t uart0_getc(void);
void uart0_putc(uint8_t data);
void uart0_puthex(uint8_t hex);
void uart0_putcrlf(void);
uint8_t uart0_data_available(void);

#define uart_init uart0_init
#define uart_getc uart0_getc
#define uart_putc uart0_putc
#define uart_puthex uart0_puthex
#define uart_putcrlf uart0_putcrlf
#define uart_data_available uart0_data_available

#endif

