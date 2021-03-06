/*
    C=Key - Commodore <-> PS/2 Interface
    Copyright Jim Brain and RETRO Innovations, 2004-2012

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

    main.c: Main application

*/
#include <inttypes.h>
#include <avr/interrupt.h>
#include "config.h"
#include "poll.h"
#include "ps2.h"
#include "scanner.h"
#include "uart.h"

ISR(TIMER2_COMP_vect) {
  if(MODE_DETECT) {
    poll_irq();
  } else {
    scan_irq();
  }
}

void main( void ) {
  uart_init();

  poll_init();  // do it here to reset cross-point switch everytime.
  if(MODE_DETECT) {
    ps2_init(PS2_MODE_HOST);
    uart_putc('H');
    //poll_init();  // do it above to reset cross-point switch everytime
    sei();
    poll();
  } else {
    ps2_init(PS2_MODE_DEVICE);
    uart_putc('D');
    scan_init();
    sei();
    scan();
  }
}
