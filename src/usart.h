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

#ifdef UBRR0H

#define UBRRH UBRR0H
#define UBRRL UBRR0L
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define U2X U2X0
#define TXEN TXEN0
#define RXEN RXEN0
#define RXCIE RXCIE0
#define URSEL URSEL0
#define UCSZ0 UCSZ01
#define UCSZ1 UCSZ11
#define UDR UDR0
#define UDRIE UDRIE0

#define SIG_UART_RECV SIG_USART0_RECV
#define SIG_UART_DATA SIG_USART0_DATA


#endif

#define USART_RX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )
#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
	#error RX buffer size is not a power of 2
#endif
#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
#endif

#define B0300	6143
#define B0600	3071
#define B1200	1535
#define B2400	767
#define B4800	383
#define B9600	191
#define B19200	95
#define B38400	47
#define B57600  31
#define B76800	23
#define B115200	15
#define B230400	7
#define B460800	3
#define B921600	1

void USART0_Init( unsigned int baudrate );
uint8_t USART0_Receive( void );
void USART0_Transmit( uint8_t data );

#ifndef USART_C
#endif

#endif

