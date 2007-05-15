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
#include <avr/interrupt.h>
#include <inttypes.h>
#include "usart.h"
#include "ps2.h"
#include "scanner.h"
#include "util.h"
#include "poll.h"


int main( void ) {
  uint8_t mode;
  USART0_Init( B115200 );

  // check for direction
  mode=(PIND&(1<<4)?PS2_MODE_HOST:PS2_MODE_DEVICE);
  switch(mode) {
    case PS2_MODE_DEVICE:
        debug2('D');
        break;
    case PS2_MODE_HOST:
        debug2('H');
        break;
    default:
        debug2('E');
        break;
  }
  
  PS2_init(mode);
    poll_init();
  if(mode==PS2_MODE_DEVICE) {
    scan_init();
    sei();
    scan();
  } else {
    sei();
    poll();
  }
}
