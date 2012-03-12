/*
    PS2Encoder - PS2 Keyboard to serial/parallel converter
    Copyright Jim Brain and RETRO Innovations, 2008-2011

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

    uart.c: UART access routines

*/

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "uart.h"

#ifdef ENABLE_UART0
#  if defined UART0_TX_BUFFER_SHIFT && UART0_TX_BUFFER_SHIFT > 0
static uint8_t tx0_buf[1 << UART0_TX_BUFFER_SHIFT];
static volatile uint8_t tx0_tail;
static volatile uint8_t tx0_head;
#  endif
#  if defined UART0_RX_BUFFER_SHIFT && UART0_RX_BUFFER_SHIFT > 0
static uint8_t rx0_buf[1 << UART0_RX_BUFFER_SHIFT];
static volatile uint8_t rx0_tail;
static volatile uint8_t rx0_head;
#  endif
#endif

#ifdef ENABLE_UART1
#  if defined UART1_TX_BUFFER_SHIFT && UART1_TX_BUFFER_SHIFT > 0
static uint8_t tx1_buf[1 << UART1_TX_BUFFER_SHIFT];
static volatile uint8_t tx1_tail;
static volatile uint8_t tx1_head;
#  endif
#  if defined UART1_RX_BUFFER_SHIFT && UART1_RX_BUFFER_SHIFT > 0
static uint8_t rx1_buf[1 << UART1_RX_BUFFER_SHIFT];
static volatile uint8_t rx1_head;
static volatile uint8_t rx1_tail;
#  endif
#endif

/* UART0 Interrupt handlers */
#if defined ENABLE_UART0
#  if defined UART0_TX_BUFFER_SHIFT && UART0_TX_BUFFER_SHIFT > 0
ISR(USARTA_UDRE_vect) {
  if ( tx0_head != tx0_tail ) {
    UDRA = tx0_buf[tx0_tail];     /* Start transmition */
    /* Calculate and store buffer index */
    tx0_tail = (tx0_tail + 1) & (sizeof(tx0_buf) - 1);
  } else {
    UCSRAB &= ~ _BV(UDRIEA);  /* Disable interrupt */
  }
}
#  endif

#  if defined UART0_RX_BUFFER_SHIFT && UART0_RX_BUFFER_SHIFT > 0
ISR(USARTA_RXC_vect) {
  /* Calculate and store buffer index */
  rx0_head = (rx0_head + 1) & (sizeof(rx0_buf) - 1);
  if ( rx0_head == rx0_tail ) {
    /* ERROR! Receive buffer overflow */
  }
  rx0_buf[rx0_head] = UDRA; /* Read and store received data */
}
#  endif

uint8_t uart0_data_available(void) {
#if defined UART0_RX_BUFFER_SHIFT && UART0_RX_BUFFER_SHIFT > 0
  /* Return 0 (FALSE) if the receive buffer is empty */
  return ( rx0_head != rx0_tail );
#else
  return ((UCSRAA & (1 << RXCA)) != 0);
#endif
}
uint8_t uart_data_available(void) __attribute__ ((weak, alias("uart0_data_available")));

void uart0_putc(uint8_t data) {
#if defined UART0_TX_BUFFER_SHIFT && UART0_TX_BUFFER_SHIFT > 0
  /* Calculate buffer index */
  uint8_t t = (tx0_head + 1) & (sizeof(tx0_buf) - 1);
  while(t == tx0_tail);  /* Wait for free space in buffer */

  tx0_buf[tx0_head] = data;    /* Store data in buffer */
  tx0_head = t;                /* Store new index */
  UCSRAB |= _BV(UDRIEA);       /* Enable UDR0E interrupt */
#else
  loop_until_bit_is_set(UCSRAA,UDREA);
  UDRA = data;
#endif
}
void uart_putc(uint8_t data) __attribute__ ((weak, alias("uart0_putc")));

uint8_t uart0_getc(void) {
#  if defined UART0_RX_BUFFER_SHIFT && UART0_RX_BUFFER_SHIFT > 0
  while (rx0_head == rx0_tail) {;}
  /* Calculate and store buffer index */
  rx0_tail = ( rx0_tail + 1 ) & (sizeof(rx0_buf)-1);
  return rx0_buf[rx0_tail];           /* Return data */
#  else
  loop_until_bit_is_set(UCSRAA,RXCA);
  return UDRA;
#  endif
}
uint8_t uart_getc(void) __attribute__ ((weak, alias("uart0_getc")));

void uart0_flush(void) {
#  if defined UART0_TX_BUFFER_SHIFT && UART0_TX_BUFFER_SHIFT > 0
  while (tx0_head != tx0_tail) ;
#  endif
}
void uart_flush(void) __attribute__ ((weak, alias("uart0_flush")));

void uart0_puts_P(prog_char *text) {
  uint8_t ch;

  while ((ch = pgm_read_byte(text++))) {
    uart0_putc(ch);
  }
}
void uart_puts_P(prog_char *text) __attribute__ ((weak, alias("uart0_puts_P")));

void uart0_putcrlf(void) {
  uart0_putc(13);
  uart0_putc(10);
}
void uart_putcrlf(void) __attribute__ ((weak, alias("uart0_putcrlf")));

#  ifdef DYNAMIC_UART
void uart0_config(uint16_t rate, uartlen_t length, uartpar_t parity, uartstop_t stopbits) {
  UBRRAH = rate >> 8;
  UBRRAL = rate & 0xff;

  UART0_CONFIG(length, parity, stopbits);
}
void uart_config(uint16_t rate, uartlen_t length, uartpar_t parity, uartstop_t stopbits) __attribute__ ((weak, alias("uart0_config")));
#  endif

void uart0_puthex(uint8_t hex) {
  uint8_t tmp = hex >> 4;

  uart_putc(tmp>9?tmp - 10 + 'a':tmp + '0');
  tmp = hex & 0x0f;
  uart_putc(tmp>9?tmp - 10 + 'a':tmp + '0');
}
void uart_puthex(uint8_t hex) __attribute__ ((weak, alias("uart0_puthex")));

void uart0_trace(void *ptr, uint16_t start, uint16_t len) {
  uint16_t i;
  uint8_t j;
  uint8_t ch;
  uint8_t *data = ptr;

  data+=start;
  for(i=0;i<len;i+=16) {

    uart0_puthex(start >> 8);
    uart0_puthex(start & 0xff);
    uart0_putc('|');
    uart0_putc(' ');
    for(j = 0;j < 16;j++) {
      if(i + j < len) {
        ch=*(data + j);
        uart0_puthex(ch);
      } else {
        uart0_putc(' ');
        uart0_putc(' ');
      }
      uart0_putc(' ');
    }
    uart0_putc('|');
    for(j = 0;j < 16;j++) {
      if(i + j < len) {
        ch=*(data++);
        if(ch < 32 || ch > 0x7e)
          ch='.';
        uart0_putc(ch);
      } else {
        uart0_putc(' ');
      }
    }
    uart0_putc('|');
    uart0_putcrlf();
    start += 16;
  }
}
void uart_trace(void *ptr, uint16_t start, uint16_t len) __attribute__ ((weak, alias("uart0_trace")));


static int ioputc(char c, FILE *stream) {
  if (c == '\n')
    uart0_putc('\r');
  uart0_putc(c);
  return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(ioputc, NULL, _FDEV_SETUP_WRITE);
#endif

#ifdef ENABLE_UART1
#  if defined UART1_TX_BUFFER_SHIFT && UART1_TX_BUFFER_SHIFT > 0
ISR(USARTB_UDRE_vect) {
  if ( tx1_head != tx1_tail ) {
    UDRB = tx1_buf[tx1_tail];     /* Start transmition */
    /* Calculate and store buffer index */
    tx1_tail = (tx1_tail + 1) & (sizeof(tx1_buf) - 1);
  } else {
    UCSRBB &= ~ _BV(UDRIEB);  /* Disable interrupt */
  }
}
#  endif

#  if defined UART1_RX_BUFFER_SHIFT && UART1_RX_BUFFER_SHIFT > 0
ISR(USARTB_RXC_vect) {
  /* Calculate and store buffer index */
  rx1_head = ( rx1_head + 1 ) & (sizeof(rx1_buf)-1);
  if ( rx1_head == rx1_tail ) {
    /* ERROR! Receive buffer overflow */
  }
  rx1_buf[rx1_head] = UDRB; /* Read and store received data */
}
#  endif

void uart1_putc(char data) {
#  if defined UART1_TX_BUFFER_SHIFT && UART1_TX_BUFFER_SHIFT > 0
  uint8_t t=(tx1_head+1) & (sizeof(tx1_buf)-1);
  UCSRBB &= ~ _BV(UDRIEB);   // turn off RS232 irq
  tx1_buf[tx1_head] = data;
  tx1_head = t;
  UCSRBB |= _BV(UDRIEB);
#  else
  loop_until_bit_is_set(UCSRBA,UDREB);
  UDRB = data;
#  endif
}

uint8_t uart1_getc(void) {
#if !defined UART1_RX_BUFFER_SHIFT || UART1_RX_BUFFER_SHIFT == 0
  loop_until_bit_is_set(UCSRBA,RXCB);
  return UDRB;
#else
  uint8_t tmptail;

  while ( rx1_head == rx1_tail ) { ; }
  tmptail = ( rx1_tail + 1 ) & (sizeof(rx1_buf)-1);/* Calculate buffer index */

  rx1_tail = tmptail;                /* Store new index */

  return rx1_buf[tmptail];           /* Return data */
#endif
}

void uart1_puts(char* str) {
  while(*str)
    uart1_putc(*str++);
}

#  ifdef DYNAMIC_UART
void uart1_config(uint16_t rate, uartlen_t length, uartpar_t parity, uartstop_t stopbits) {
  UBRRBH = rate >> 8;
  UBRRBL = rate & 0xff;

  UART1_CONFIG(length, parity, stopbits);
}
#  endif
#endif

#if defined ENABLE_UART0 || defined ENABLE_UART1
/* Initialize UART */
void uart_init(void) {
  /* Set the baud rate */
#  if defined ENABLE_UART0
  UART0_MODE_SETUP();

  UBRRAH = CALC_BPS(UART0_BAUDRATE) >> 8;
  UBRRAL = CALC_BPS(UART0_BAUDRATE) & 0xff;

#    ifdef UART_DOUBLE_SPEED
  /* double the speed of the serial port. */
  UCSRAA = (1<<U2X0);
#    endif

  /* Enable UART receiver and transmitter */
  UCSRAB = (0
#    if defined UART0_RX_BUFFER_SHIFT && UART0_RX_BUFFER_SHIFT > 0
            | _BV(RXCIEA)
#    endif
            | _BV(RXENA)
            | _BV(TXENA)
           );



  /* Flush buffers */
#    if defined UART0_TX_BUFFER_SHIFT && UART0_TX_BUFFER_SHIFT > 0
  tx0_tail = 0;
  tx0_head = 0;
#    endif
#    if defined UART0_RX_BUFFER_SHIFT && UART0_RX_BUFFER_SHIFT > 0
  rx0_tail = 0;
  rx0_head = 0;
#    endif

  stdout = &mystdout;
#  endif

#  ifdef ENABLE_UART1
  UART1_MODE_SETUP();

  UBRRBH = CALC_BPS(UART1_BAUDRATE) >> 8;
  UBRRBL = CALC_BPS(UART1_BAUDRATE) & 0xff;

  /* Enable UART receiver and transmitter */
  UCSRBB = (0
#    if defined UART1_RX_BUFFER_SHIFT && UART1_RX_BUFFER_SHIFT > 0
            | _BV(RXCIEB)
#    endif
            | _BV(RXENB)
            | _BV(TXENB)
         );

  /* Flush buffers */
#    if defined UART1_TX_BUFFER_SHIFT && UART1_TX_BUFFER_SHIFT > 0
  tx1_tail = 0;
  tx1_head = 0;
#    endif
#    if defined UART1_RX_BUFFER_SHIFT && UART1_RX_BUFFER_SHIFT > 0
  rx1_tail = 0;
  rx1_head = 0;
#    endif
#  endif
}
#endif
