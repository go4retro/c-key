/*
    C=Key - Commodore <-> PS/2 Interface
    Copyright Jim Brain and RETRO Innovations, 2004-2011

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    switches.c: Functions for non-matrixed keyboard switches

*/

// TODO: if putc has depressed a IO pin, don't record it as a switch closure.

#include <avr/io.h>
#include <inttypes.h>
#include "config.h"
#include "switches.h"

static unsigned char rx_buf[_BV( SW_RX_BUFFER_SHIFT)];
static volatile uint8_t rx_head;
static volatile uint8_t rx_tail;

static uint8_t cache;
static uint8_t in_mask;

static void sw_store(uint8_t data) {
  
  rx_head = ( rx_head + 1 ) & (sizeof(rx_buf) - 1);  /* Calculate and store new index */

  if ( rx_head == rx_tail ) {
    /* ERROR! Receive buffer overflow */
  }
  
  rx_buf[rx_head] = data; /* Store received data in buffer */
}

uint8_t sw_data_available( void ) {
  return ( rx_head != rx_tail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

void sw_putc( uint8_t sw) {
  // bit 7 indicates key up.
  uint8_t state=(sw & SW_UP);
  sw = _BV(sw); // turn into bit mask;
  if(sw & in_mask) {
    if(state) {
      // bring to input and set HI-Z
      PORT_SW_DDR &= (uint8_t)~sw;
      PORT_SW_OUT |= sw;
    } else {
      PORT_SW_DDR |= sw;
      PORT_SW_OUT &= (uint8_t)~sw;
    }
  }
}

uint8_t sw_getc( void ) {
  
  while ( rx_head == rx_tail ) {
    ;
  }
  rx_tail = ( rx_tail + 1 ) & (sizeof(rx_buf) - 1);  /* Calculate and store buffer index */
  
  return rx_buf[rx_tail];           /* Return data */
}

void sw_scan(void) {
  uint8_t mask, up, down, in;

  in=(PORT_SW_IN & in_mask);
  if(in != cache) {
    // a key has changed.
    mask = in ^ cache;
    down=cache & mask;
    up=in & mask;
    if(up) {
      // keys released.
      if(up & 1)   {sw_store(SW_UP | 0);}
      if(up & 2)   {sw_store(SW_UP | 1);}
      if(up & 4)   {sw_store(SW_UP | 2);}
      if(up & 8)   {sw_store(SW_UP | 3);}
      if(up & 16)  {sw_store(SW_UP | 4);}
      if(up & 32)  {sw_store(SW_UP | 5);}
      if(up & 64)  {sw_store(SW_UP | 6);}
      if(up & 128) {sw_store(SW_UP | 7);}
    }
    if(down) {
      // keys pressed.
      if(down & 1)   {sw_store(0);}
      if(down & 2)   {sw_store(1);}
      if(down & 4)   {sw_store(2);}
      if(down & 8)   {sw_store(3);}
      if(down & 16)  {sw_store(4);}
      if(down & 32)  {sw_store(5);}
      if(down & 64)  {sw_store(6);}
      if(down & 128) {sw_store(7);}
    }
    cache = in;
  }
}

void sw_init(uint8_t mask) {
  // bring them HI-Z
  PORT_SW_OUT |= mask;
  // bring them to inputs;
  PORT_SW_DDR &= (uint8_t)~mask;
  in_mask = mask;
  cache=(PORT_SW_IN & mask);
}
