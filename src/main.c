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
#include <avr/interrupt.h>
#include "config.h"
#include "poll.h"
#include "ps2.h"
#include "scanner.h"
#include "uart.h"
#include "util.h"

#ifdef REV3
#define PIN_MODE (1<<PIN4)
#else
#define PIN_MODE (1<<PIN5)
#endif

static volatile uint8_t mode;

SIGNAL(SIG_OUTPUT_COMPARE2) {
  if(mode==PS2_MODE_DEVICE) {
    scan_irq();
  } else {
    poll_irq();
  }
}

int main( void ) {
  uart_init(B115200);

  // check for direction
  mode=(PIND&PIN_MODE?PS2_MODE_HOST:PS2_MODE_DEVICE);
  switch(mode) {
    case PS2_MODE_DEVICE:
        debug('D');
        break;
    case PS2_MODE_HOST:
        debug('H');
        break;
    default:
        debug('E');
        break;
  }
  
  PS2_init(mode);
  poll_init();  // do it here to reset cross-point switch everytime.
  if(mode==PS2_MODE_DEVICE) {
    scan_init();
    sei();
    scan();
  } else {
    poll_init();
    sei();
    poll();
  }
}
