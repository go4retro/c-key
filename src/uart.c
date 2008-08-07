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
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "uart.h"

static int ioputc(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(ioputc, NULL, _FDEV_SETUP_WRITE);

#if UART0_RX_BUFFER_SIZE > 0
static uint8_t uart0_rxbuf[UART0_RX_BUFFER_SIZE];
static volatile uint8_t uart0_rxhead;
static volatile uint8_t uart0_rxtail;
#endif
#if UART0_TX_BUFFER_SIZE > 0
static uint8_t uart0_txbuf[UART0_TX_BUFFER_SIZE];
static volatile uint8_t uart0_txhead;
static volatile uint8_t uart0_txtail;
#endif

/* Initialize UART */
#ifndef UART0_BAUDRATE
void uart0_init( uint16_t baudrate ) {
  /* Set the baud rate */
  UBRRH = (uint8_t) baudrate >> 8;         
  UBRRL = (uint8_t) baudrate & 0xff;
#else
void uart0_init(void) {
  /* Set the baud rate */
  UBRRH = (uint8_t) CALC_BPS(UART0_BAUDRATE) >> 8;         
  UBRRL = (uint8_t) CALC_BPS(UART0_BAUDRATE) & 0xff;
#endif
	
#ifdef UART_DOUBLE_SPEED
  /* double the speed of the serial port. */
	UCSRA = (1<<U2X);
#endif
	
	/* Enable UART receiver and transmitter */
#if UART0_RX_BUFFER_SIZE > 0 || UART0_TX_BUFFER_SIZE > 0
	UCSRB = (0 
#  if UART0_RX_BUFFER_SIZE > 0
	    | (1 << RXCIE)
	    | (1 << RXEN)
#  endif
#  if UART0_TX_BUFFER_SIZE > 0
	    | (1 << TXEN)
#  endif	    
	   ); 
#endif
	
	/* Flush buffers */
#if	UART0_RX_BUFFER_SIZE > 0
	uart0_rxtail = 0;
	uart0_rxhead = 0;
#endif
#if UART0_TX_BUFFER_SIZE > 0
	uart0_txtail = 0;
	uart0_txhead = 0;
#endif

	stdout = &mystdout;
}


/* Interrupt handlers */

#if UART0_RX_BUFFER_MASK > 0
ISR(USART0_RXC_vect) {
	uint8_t data;
	uint8_t tmphead;
	
	/* Read the received data */
	data = UDR;        
	
	/* Calculate buffer index */
	tmphead = ( uart0_rxhead + 1 ) & UART0_RX_BUFFER_MASK;
	uart0_rxhead = tmphead;      /* Store new index */

	if ( tmphead == uart0_rxtail ) {
		/* ERROR! Receive buffer overflow */
	}
	
	uart0_rxbuf[tmphead] = data; /* Store received data in buffer */
}
#endif

#if UART0_TX_BUFFER_MASK > 0
ISR(USART0_UDRE_vect) {
	uint8_t tmptail;

	if ( uart0_txhead != uart0_txtail ) {
		/* Calculate buffer index */
		tmptail = ( uart0_txtail + 1 ) & UART0_TX_BUFFER_MASK;
		uart0_txtail = tmptail;      /* Store new index */
	
		UDR = uart0_txbuf[tmptail];  /* Start transmition */
	} else {
		UCSRB &= ~(1<<UDRIE);         /* Disable UDRE interrupt */
	}
}
#endif

/* Read and write functions */
uint8_t uart0_getc(void) {
#if UART0_RX_BUFFER_MASK > 0
	uint8_t tmptail;

	while ( uart0_rxhead == uart0_rxtail ) { ; }
	tmptail = ( uart0_rxtail + 1 ) & UART0_RX_BUFFER_MASK;/* Calculate buffer index */
	
	uart0_rxtail = tmptail;                /* Store new index */
	
	return uart0_rxbuf[tmptail];           /* Return data */
#else
  loop_until_bit_is_set(UCSRA,RXC);
	return UDR;
#endif
}

void uart0_putc(uint8_t data) {
#if UART0_TX_BUFFER_MASK > 0
	uint8_t tmphead;
	/* Calculate buffer index */
	tmphead = ( uart0_txhead + 1 ) & UART0_TX_BUFFER_MASK; /* Wait for free space in buffer */
	while ( tmphead == uart0_txtail ) ;

	uart0_txbuf[tmphead] = data;           /* Store data in buffer */
	uart0_txhead = tmphead;                /* Store new index */

	UCSRB |= (1<<UDRIE);                    /* Enable UDRE interrupt */
#else
	// TODO should check for overflow here
	UDR = data;
#endif
}

uint8_t uart0_data_available(void) {
#if UART0_RX_BUFFER_MASK > 0
	return ( uart0_rxhead != uart0_rxtail ); /* Return 0 (FALSE) if the receive buffer is empty */
#else
	return ((UCSRA & RXC) != 0);
#endif
}

void uart0_puthex(uint8_t hex) {
  uint8_t tmp = hex >> 4;
  
  uart_putc(tmp>9?tmp - 10 + 'A':tmp + '0');
  tmp = hex & 0x0f;
  uart_putc(tmp>9?tmp - 10 + 'A':tmp + '0');
}

void uart0_flush(void) {
#if UART0_TX_BUFFER_SIZE > 0  
  while (uart0_txhead != uart0_txtail) ;
#endif
}

void uart0_puts_P(prog_char *text) {
  uint8_t ch;

  while ((ch = pgm_read_byte(text++))) {
    uart0_putc(ch);
  }
}

void uart0_putcrlf(void) {
  uart0_putc(13);
  uart0_putc(10);
}

/* functions for USART0 only */

static int ioputc(char c, FILE *stream) {
  if (c == '\n') 
    uart_putc('\r');
  uart_putc(c);
  return 0;
}

void uart_trace(void *ptr, uint16_t start, uint16_t len) {
  uint16_t i;
  uint8_t j;
  uint8_t ch;
  uint8_t *data = ptr;

  data+=start;
  for(i=0;i<len;i+=16) {

    uart_puthex(start>>8);
    uart_puthex(start&0xff);
    uart_putc('|');
    uart_putc(' ');
    for(j=0;j<16;j++) {
      if(i+j<len) {
        ch=*(data + j);
        uart_puthex(ch);
      } else {
        uart_putc(' ');
        uart_putc(' ');
      }
      uart_putc(' ');
    }
    uart_putc('|');
    for(j=0;j<16;j++) {
      if(i+j<len) {
        ch=*(data++);
        if(ch<32 || ch>0x7e)
          ch='.';
        uart_putc(ch);
      } else {
        uart_putc(' ');
      }
    }
    uart_putc('|');
    uart_putcrlf();
    start+=16;
  }
}
