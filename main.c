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
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "kb.h"
#include "ps2.h"
#include "scanner.h"
#include "util.h"
#include "poll.h"

#define PORT_KEY_OUT      PORTD
#define PORT_KEY_IN       PIND
#define DDR_KEY           DDRD
#define PIN_KEY_RESTORE   PIN4
#define PIN_KEY_4080      PIN5

#define PORT_LED          PORTD
#define DDR_LED           DDRD
#define PIN_LED           PIN7

void scan_init(void) {
  OCR2=31; //32 counts * 256 cycles/count * 16 times per run * 120 runs/sec  
  TCNT2=0;
  
  // Set OC2 clk  to SYSCLK/256 and Compare Timer MOde
  TCCR2 = (1<<CS22) | (1<<CS21) | (1<<WGM21);
  // set up OC2 IRQ
  TIMSK |= (1<<OCIE2);
}

SIGNAL(SIG_OUTPUT_COMPARE2) {
  kb_scan();
  // check for 80/40 key and RESTORE key.
}

int main( void ) {
  unsigned char mode;
  
  mode=PS2_MODE_HOST;
  
  DDR_LED = (1<<PIN_LED);
	USART0_Init( B115200 );
	PS2_init(mode);
  if(mode==PS2_MODE_DEVICE) {
    KB_init();
    scan_init();
    sei();
    scan();
  } else {
    DDR_KEY|=((1<<PIN_KEY_RESTORE) | (1<<PIN_KEY_4080));
    sei();
    poll();
  }
}
