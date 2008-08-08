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

// TODO Allow users to mask off certain lines of the rows and columns as not used.
#include <avr/io.h>
#include <inttypes.h>
#include "config.h"
#include "kb.h"

static uint8_t          kb_rxbuf[KB_RX_BUFFER_SIZE];
static volatile uint8_t kb_rxhead;
static volatile uint8_t kb_rxtail;

static uint8_t          kb_save[16];
static volatile uint8_t kb_state;
static volatile uint8_t kb_scan_idx;

#ifdef KB_SCAN_PORTS
static uint8_t          kb_port_save[2];
#endif

static volatile uint8_t  kb_repeat_code;
static volatile uint16_t kb_repeat_count;
static volatile uint16_t kb_repeat_delay;
static volatile uint16_t kb_repeat_period;
static volatile uint8_t  kb_curr_value;

static void kb_store(uint8_t data) {
  uint8_t tmphead;
  
  tmphead = (kb_rxhead + 1) & KB_RX_BUFFER_MASK;
  kb_rxhead = tmphead;      /* Store new index */

  //if ( tmphead == kb_rxtail ) {
    /* ERROR! Receive buffer overflow */
  //}
  
  kb_rxbuf[tmphead] = data; /* Store received data in buffer */
}

static void kb_decode(uint8_t new, uint8_t *old, uint8_t base) {
  uint8_t i, mask, result;
  // we have a key change.
  /*
    If old was:     00001010
    and new was:    01001000 
    we need to xor: 01000010
    which will tell us what changed.
    
    Then, new & xor gives us new keys
    and old and xor gives us keys no longer pressed.
  */
  mask = new ^ *old;
  result = (*old & mask);
  i = 0;
  while(result) {
    // we have keys no longer pressed.
    if(result & 1)
      kb_store((base + i) | KB_KEY_UP);
    result = result >> 1;
    i++;
  }
  result=(new & mask);
  i = 0;
  while(result) {
    // we have keys pressed.
    if(result & 1)
      kb_store(base + i);
    result = result >> 1;
    i++;
  }
  *old = new;
}

void kb_scan(void) {
  // this should be called 120 times/sec
  uint8_t j;
  uint8_t in;
  uint16_t tmp;
  // this is where we scan.
  // we scan at 120Hz
  switch(kb_state) {
    default:
    case KB_ST_READ:
      in = kb_repeat_code;
      if(in != KB_NO_REPEAT) {
        kb_repeat_count--;
        if(!kb_repeat_count) {
          kb_repeat_count = kb_repeat_period;
          kb_store(in);
        }
      }
      // do housekeeping
      in = kb_curr_value;
      j = kb_scan_idx;
#ifdef KB_SCAN_PORTS
      // we broght lines hi, so check for port action.  If we have it, then discard character.
      if(KB_ROW_LO_IN == 0xff && KB_COL_IN == 0xff && in != kb_save[j]) {
#else        
      if(in != kb_save[j]) {
#endif
        kb_decode(in, &kb_save[j], j<<3);
      }
      j = (j + 1) & 0x0f;
      kb_scan_idx = j;
      // we just read, prep now.
      // set pin low:
      tmp = (1 << j);
      j = (tmp & 0xff);
      in = (tmp >> 8);
      KB_ROW_LO_DDR = j;
      KB_ROW_HI_DDR = in;
      KB_ROW_LO_OUT = (uint8_t)~j;
      KB_ROW_HI_OUT = (uint8_t)~in;
      kb_state = KB_ST_PREP;
      break;
    case KB_ST_PREP:
      // we just prepped, read.
      kb_curr_value = (uint8_t) ~KB_COL_IN;
      kb_state = KB_ST_READ;
#ifdef KB_SCAN_PORTS
      // set rows back to input.
      KB_ROW_LO_DDR = 0;
      KB_ROW_HI_DDR = 0;
      KB_ROW_LO_OUT = 0xff;
      KB_ROW_HI_OUT = 0xff;
      j = kb_scan_idx;
      if(j == 0) {
        kb_state = KB_ST_READ_PORTS;
      }
      break;
    case KB_ST_READ_PORTS:
      // read the two ports directly
      in = (uint8_t)~KB_ROW_LO_IN;
      if(in != kb_port_save[1]) {
        kb_decode(in, &kb_port_save[1], 0x78);
      }
      in = (uint8_t)~KB_COL_IN;
      if(in != kb_port_save[0]) {
        kb_decode(in, &kb_port_save[0], 0x70);
      }
      kb_state = KB_ST_READ;
#endif
      break;
  }
}

void kb_init() {
  kb_state = KB_ST_READ;
  kb_repeat_code = KB_NO_REPEAT;  // set keyboard repeat to 0.
  kb_set_repeat_delay(250);       // wait 250 ms
  kb_set_repeat_period(32);       // once every 32 ms

#ifdef KB_SCAN_PORTS  
  KB_ROW_LO_OUT = 0xff;
  KB_ROW_HI_OUT = 0xff;
#endif
  KB_COL_OUT = 0xff;         // turn on pullups.
}

void kb_set_repeat_delay(uint16_t ms) {
  // 1800 ticks/sec, .5 ms per tick.
  kb_repeat_delay = (ms << 1);
  kb_repeat_count = kb_repeat_delay;
}

void kb_set_repeat_period(uint16_t period) {
  kb_repeat_period = (period << 1);
}

void kb_set_repeat_code(uint8_t code) {
  if(code != kb_repeat_code) {
    kb_repeat_count = kb_repeat_delay;
    kb_repeat_code = code;
  }
}

uint8_t kb_get_repeat_code() {
  return kb_repeat_code;
}

uint8_t kb_data_available(void) {
  return ( kb_rxhead != kb_rxtail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

uint8_t kb_recv( void ) {
	uint8_t tmptail;
	
	while (kb_rxhead == kb_rxtail);
	tmptail = (kb_rxtail + 1) & KB_RX_BUFFER_MASK;/* Calculate buffer index */
	
	kb_rxtail = tmptail;                /* Store new index */
	
	return kb_rxbuf[tmptail];           /* Return data */
}

