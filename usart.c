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
#include <inttypes.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "usart.h"

static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile uint8_t USART_RxHead;
static volatile uint8_t USART_RxTail;
static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile uint8_t USART_TxHead;
static volatile uint8_t USART_TxTail;

/* Initialize USART */
void USART0_Init( unsigned int baudrate ) {
	uint8_t x;
	
	/* Set the baud rate */
	UBRRH = (uint8_t) (baudrate>>8);                  
	UBRRL = (uint8_t) baudrate;
	
	// double the speed of the serial port.
	
	UCSRA = (1<<U2X); 
	
	/* Enable UART receiver and transmitter */
	UCSRB = ( ( 1 << RXCIE ) | ( 1 << RXEN ) | ( 1 << TXEN ) ); 
	
	/* Set frame format: 8 data 1stop */
	//UCSRC = (1<<URSEL) | (1<<UCSZ1)|(1<<UCSZ0);
	
	/* Flush receive buffer */
	x = 0; 			    

	USART_RxTail = x;
	USART_RxHead = x;
	USART_TxTail = x;
	USART_TxHead = x;
}


/* Interrupt handlers */
SIGNAL(SIG_UART_RECV) {
	uint8_t data;
	uint8_t tmphead;
	
	/* Read the received data */
	data = UDR;        
	
	/* Calculate buffer index */
	tmphead = ( USART_RxHead + 1 ) & USART_RX_BUFFER_MASK;
	USART_RxHead = tmphead;      /* Store new index */

	if ( tmphead == USART_RxTail ) {
		/* ERROR! Receive buffer overflow */
	}
	
	USART_RxBuf[tmphead] = data; /* Store received data in buffer */
}

SIGNAL(SIG_UART_DATA) {
	uint8_t tmptail;

	if ( USART_TxHead != USART_TxTail ) {
		/* Calculate buffer index */
		tmptail = ( USART_TxTail + 1 ) & USART_TX_BUFFER_MASK;
		USART_TxTail = tmptail;      /* Store new index */
	
		UDR = USART_TxBuf[tmptail];  /* Start transmition */
	} else {
		UCSRB &= ~(1<<UDRIE);         /* Disable UDRE interrupt */
	}
}

/* Read and write functions */
uint8_t USART0_Receive( void ) {
	uint8_t tmptail;
	
	while ( USART_RxHead == USART_RxTail ) { ; }
	tmptail = ( USART_RxTail + 1 ) & USART_RX_BUFFER_MASK;/* Calculate buffer index */
	
	USART_RxTail = tmptail;                /* Store new index */
	
	return USART_RxBuf[tmptail];           /* Return data */
}

void USART0_Transmit( uint8_t data ) {
	uint8_t tmphead;
	/* Calculate buffer index */
	tmphead = ( USART_TxHead + 1 ) & USART_TX_BUFFER_MASK; /* Wait for free space in buffer */
	while ( tmphead == USART_TxTail ) ;

	USART_TxBuf[tmphead] = data;           /* Store data in buffer */
	USART_TxHead = tmphead;                /* Store new index */

	UCSRB |= (1<<UDRIE);                    /* Enable UDRE interrupt */
}

uint8_t USART0_Data_Available( void ) {
	return ( USART_RxHead != USART_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}
