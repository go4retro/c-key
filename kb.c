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
#include <avr/io.h>
#include <inttypes.h>
#include "util.h"
#include "kb.h"

static unsigned char KB_RxBuf[KB_RX_BUFFER_SIZE];
static volatile uint8_t KB_RxHead;
static volatile uint8_t KB_RxTail;

static unsigned char KB_cache[16];
static volatile uint8_t KB_state;
static volatile uint8_t KB_scan_idx;

#ifdef PORT_KEYS
static unsigned char PORT_cache[2];
#endif

static volatile uint8_t KB_repeat_code;
static volatile uint16_t KB_repeat_count;
static volatile uint16_t KB_repeat_match;
static volatile uint16_t KB_repeat_delay;
static volatile uint16_t KB_repeat_period;
static volatile uint16_t KB_curr_value;

void KB_init() {
  KB_state=KB_ST_READ;
  //KB_scan_idx=0;
  
  // set keyboard repeat to 0.
  KB_repeat_code=KB_NO_REPEAT;
  //KB_repeat_count=0;
  
  KB_set_repeat_delay(250);   // wait 250 ms
  KB_set_repeat_period(32);   // once every 32 ms

#ifdef PORT_KEYS  
  KB_PORT_ROW_LOW_OUT=0xff;
  KB_PORT_ROW_HIGH_OUT=0xff;
#endif
  // set COL to input
  //KB_DDR_COL=0x00;
  // turn on pullups.
  KB_PORT_COL_OUT=0xff;
  
  
}

void KB_set_repeat_delay(uint16_t ms) {
  // 1800 ticks/sec, .5 ms per tick.
  KB_repeat_delay=ms<<1;
  KB_repeat_match=KB_repeat_delay;
  KB_repeat_count=0;
}

void KB_set_repeat_period(uint16_t period) {
  KB_repeat_period=period<<1;
}

void KB_set_repeat_code(uint8_t code) {
  if(code != KB_repeat_code) {
    KB_repeat_count=0;
    KB_repeat_code=code;
    KB_repeat_match=KB_repeat_delay;
  }
}

uint8_t KB_get_repeat_code() {
  return KB_repeat_code;
}

void KB_store(uint8_t data) {
  uint8_t tmphead;
  
	tmphead = ( KB_RxHead + 1 ) & KB_RX_BUFFER_MASK;
	KB_RxHead = tmphead;      /* Store new index */

	if ( tmphead == KB_RxTail ) {
		/* ERROR! Receive buffer overflow */
	}
	
	KB_RxBuf[tmphead] = data; /* Store received data in buffer */
}

void KB_decode(uint8_t in, uint8_t old, uint8_t base) {
    uint8_t j,mask,result;
    // we have a key change.
    /*
      If last state was 00001010 and new
      state was:        01001000 
      we need to xor:   01000010 which
      will tell us what changed:
      
      Then, in & xor gives us new keys
      state and xor gives us keys no longer pressed.
    */
    mask = in ^ old;
    result=old & mask;
    if(result != 0) {
      // we have keys no longer pressed.
      for(j=0;j<8;j++) {
        if(result & 1) {
          KB_store((base+j) | KB_KEY_UP);
        }
        result=result>>1;
      }
    }
    result=in & mask;
    if(result != 0) {
      // we have keys pressed.
      for(j=0;j<8;j++) {
        if(result & 1) {
          KB_store(base+j);
        }
        result=result>>1;
      }
    }
}

void KB_scan(void) {
  // this should be called 120 times/sec
  uint8_t j;
  uint8_t in;
  uint16_t tmp;
  // this is where we scan.
  // we scan at 120Hz
  switch(KB_state) {
    default:
    case KB_ST_READ:
      in=KB_repeat_code;
      if(in != KB_NO_REPEAT) {
        KB_repeat_count++;
        if(KB_repeat_count >= KB_repeat_match) {
          KB_repeat_count=0;
          KB_repeat_match=KB_repeat_period;
          KB_store(in);
        }
      }
      // do housekeeping
      in=KB_curr_value;
      j=KB_scan_idx;
#ifdef PORT_KEYS
      // we broght lines hi, so check for port action.  If we have it, then discard character.
      if(KB_PORT_ROW_LOW_IN==0xff && KB_PORT_COL_IN==0xff && in != KB_cache[j]) {
#else        
      if(in != KB_cache[KB_scan_idx]) {
#endif
        KB_decode(in,KB_cache[j],j<<3);
        KB_cache[j]=in;
      }
      j=(j+1)&0x0f;
      KB_scan_idx = j;
      // we just read, prep now.
      // set pin low:
      tmp=(1<<j);
      j=(tmp & 0xff);
      in=(tmp >> 8);
      KB_DDR_ROW_LOW=j;
      KB_DDR_ROW_HIGH=in;
      KB_PORT_ROW_LOW_OUT=(uint8_t)~j;
      KB_PORT_ROW_HIGH_OUT=(uint8_t)~in;
      KB_state=KB_ST_PREP;
      break;
    case KB_ST_PREP:
      // we just prepped, read.
      KB_curr_value=(uint8_t)~KB_PORT_COL_IN;
#ifdef PORT_KEYS
      // set rows back to input.
      KB_DDR_ROW_LOW=0;
      KB_DDR_ROW_HIGH=0;
      KB_PORT_ROW_LOW_OUT=0xff;
      KB_PORT_ROW_HIGH_OUT=0xff;
    //  KB_state=KB_ST_QUIESCE;
    //  break;
    //case KB_ST_QUIESCE:
    //  in=KB_curr_value;
    //  j=KB_scan_idx;
      // we broght lines hi, so check for port action.  If we have it, then discard character.
    //  if(KB_PORT_COL_IN==0xff && in != KB_cache[j]) {
#else
    //  in=KB_curr_value;
    //  j=KB_scan_idx;
    //  if(in != KB_cache[KB_scan_idx]) {
#endif
    //    KB_decode(in,KB_cache[j],j<<3);
    //    KB_cache[j]=in;
    //  }
      //KB_scan_idx = ((j + 1) & 0x0f);
      KB_state=KB_ST_READ;
#ifdef PORT_KEYS
      j=KB_scan_idx;
      if(j==0) {
        // set rows to input.
        //KB_DDR_ROW_LOW=0;
        // turn on pullups.
        //KB_PORT_ROW_LOW_OUT=0xff;
        KB_state=KB_ST_READ_PORTS;
      }
      break;
    case KB_ST_READ_PORTS:
      // read the two ports directly
      in=(uint8_t)~KB_PORT_ROW_LOW_IN;
      if(in != PORT_cache[1]) {
        KB_decode(in,PORT_cache[1],0x78);
        PORT_cache[1]=in;
      }
      in=(uint8_t)~KB_PORT_COL_IN;
      if(in != PORT_cache[0]) {
        KB_decode(in,PORT_cache[0],0x70);
        PORT_cache[0]=in;
      }
      KB_state=KB_ST_READ;
#endif
      break;
  }
}

uint8_t KB_data_available( void ) {
  return ( KB_RxHead != KB_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

uint8_t KB_recv( void ) {
	uint8_t tmptail;
	
	while ( KB_RxHead == KB_RxTail ) {
    ;
	}
	tmptail = ( KB_RxTail + 1 ) & KB_RX_BUFFER_MASK;/* Calculate buffer index */
	
	KB_RxTail = tmptail;                /* Store new index */
	
	return KB_RxBuf[tmptail];           /* Return data */
}
