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
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "util.h"
#include "KB.h"

static unsigned char KB_RxBuf[KB_RX_BUFFER_SIZE];
static volatile unsigned char KB_RxHead;
static volatile unsigned char KB_RxTail;

static unsigned char KB_cache[16];
static volatile unsigned char KB_state;
static volatile unsigned char KB_scan_idx;

static volatile unsigned char KB_repeat_code;
static volatile unsigned int KB_repeat_count;
static volatile unsigned int KB_repeat_match;
static volatile unsigned int KB_repeat_delay;
static volatile unsigned int KB_repeat_period;

void KB_init() {
  KB_state=KB_ST_READ;
  KB_scan_idx=0;
  
  // set keyboard repeat to 0.
  KB_repeat_code=0;
  KB_repeat_count=0;
  
  KB_set_repeat_delay(250);   // wait 250 ms
  KB_set_repeat_period(32);   // once every 32 ms
  
  OCR2=31; //32 counts * 256 cycles/count * 16 times per run * 120 runs/sec  
  TCNT2=0;
  
  // set COL to input
  KB_DDR_COL=0x00;
  // turn on pullups.
  KB_PORT_COL_OUT=0xff;
  
  // Set OC2 clk  to SYSCLK/256 and Compare Timer MOde
  TCCR2 = (1<<CS22) | (1<<CS21) | (1<<WGM21);
  // set up OC2 IRQ
  TIMSK |= (1<<OCIE2);
  
}

void KB_set_repeat_delay(unsigned int ms) {
  // 1800 ticks/sec, .5 ms per tick.
  KB_repeat_delay=ms<<1;
  KB_repeat_match=KB_repeat_delay;
  KB_repeat_count=0;
}

void KB_set_repeat_period(unsigned int period) {
  KB_repeat_period=period<<1;
}

void KB_set_repeat_code(unsigned char code) {
  if(code != KB_repeat_code) {
    KB_repeat_count=0;
    KB_repeat_code=code;
    KB_repeat_match=KB_repeat_delay;
  }
}

unsigned char KB_get_repeat_code() {
  return KB_repeat_code;
}

void KB_store(unsigned char data) {
  unsigned char tmphead;
  
	tmphead = ( KB_RxHead + 1 ) & KB_RX_BUFFER_MASK;
	KB_RxHead = tmphead;      /* Store new index */

	if ( tmphead == KB_RxTail ) {
		/* ERROR! Receive buffer overflow */
	}
	
	KB_RxBuf[tmphead] = data; /* Store received data in buffer */
}

SIGNAL(SIG_OUTPUT_COMPARE2) {
  unsigned char j;
  unsigned char in;
  unsigned char result;
  unsigned char mask;
  unsigned char mult;
  unsigned int tmp;
  // this is where we scan.
  // we scan at 60Hz
  switch(KB_state) {
    default:
    case KB_ST_READ:
      // we just read, prep now.
      // set pin low:
      tmp=(1<<KB_scan_idx);
      j=(tmp & 0xff);
      in=((tmp & 0xff00) >> 8);
      KB_DDR_ROW_LOW=j;
      KB_DDR_ROW_HIGH=in;
      KB_PORT_ROW_LOW_OUT=~j;
      KB_PORT_ROW_HIGH_OUT=~in;
      KB_state=KB_ST_PREP;
      if(KB_repeat_code!= 0) {
        KB_repeat_count++;
        if(KB_repeat_count >= KB_repeat_match) {
          KB_repeat_count=0;
          KB_repeat_match=KB_repeat_period;
          KB_store(KB_repeat_code);
        }
      }
      break;
    case KB_ST_PREP:
      // we just prepped, read.
      in=~KB_PORT_COL_IN;
      if(in != KB_cache[KB_scan_idx]) {
        mult=KB_scan_idx<<3;
        // we have a key change.
        /*
          If last state was 00001010 and new
          state was:        01001000 
          we need to xor:   01000010 which
          will tell us what changed:
          
          Then, in & xor gives us new keys
          state and xor gives us keys no longer pressed.
        */
        mask = in ^ KB_cache[KB_scan_idx];
        result=KB_cache[KB_scan_idx] & mask;
        if(result != 0) {
          // we have keys no longer pressed.
          for(j=0;j<8;j++) {
            if(result & 1) {
              KB_store((mult+j) | 0x80);
            }
            result=result>>1;
          }
        }
        result=in & mask;
        if(result != 0) {
          // we have keys pressed.
          for(j=0;j<8;j++) {
            if(result & 1) {
              KB_store(mult+j);
            }
            result=result>>1;
          }
        }
        KB_cache[KB_scan_idx]=in;
      }
      KB_state=KB_ST_READ;
      KB_scan_idx = ((KB_scan_idx + 1) & 0x0f);
      break;
  }
}

unsigned char KB_recv( void ) {
	unsigned char tmptail;
	
	while ( KB_RxHead == KB_RxTail ) {
    ;
	}
	tmptail = ( KB_RxTail + 1 ) & KB_RX_BUFFER_MASK;/* Calculate buffer index */
	
	KB_RxTail = tmptail;                /* Store new index */
	
	return KB_RxBuf[tmptail];           /* Return data */
}

