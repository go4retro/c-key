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
#include "util.h"
#include "ps2.h"
#include "ps2_host.h"

inline void PS2_host_init(void) {
  PS2_enable_IRQ_CLK_Fall();
  // reset keyboard.
  PS2_send(0xff);
}

void PS2_host_trigger_send(void) {
  // need to get devices attention...
  PS2_set_state(PS2_ST_PREP_START);
  PS2_disable_IRQ_CLK();
  PS2_clear_CLK();
  // yes, bring CLK lo for 100uS
  PS2_enable_IRQ_timer0(100);
}

void PS2_host_check_for_data(void) {
    if(PS2_data_to_send() != 0) {
      PS2_host_trigger_send();
    } else {
      // wait for something to receive
      PS2_set_state(PS2_ST_IDLE);
    }
}

inline void PS2_host_CLK() {
  switch(PS2_get_state()) {
    case PS2_ST_IDLE:
      // keyboard sent start bit
      // should read it, but will assume it is good.
      PS2_set_state(PS2_ST_GET_BIT);
      PS2_clear_counters();
      break;
    case PS2_ST_GET_BIT:
      // read bit;
      PS2_read_bit();
      if(PS2_get_count() == 8) {
        // done, do Parity bit
        PS2_set_state(PS2_ST_GET_PARITY);
      }
      break;
    case PS2_ST_GET_PARITY:
      // grab parity
      // for now, assume it is OK.
      PS2_set_state(PS2_ST_GET_STOP);
      break;
    case PS2_ST_GET_STOP:
      // stop bit
      // for now, assume it is OK.
      PS2_write_byte();
      // do we have data to send to keyboard?
      PS2_host_check_for_data();
      break;
    case PS2_ST_PREP_BIT:
      // time to send bits...
      if(PS2_get_count() == 8) {
        // we are done..., do parity
        PS2_write_parity();
        PS2_set_state(PS2_ST_SEND_PARITY);
      } else {
        PS2_write_bit();
      }
      break;
    case PS2_ST_SEND_PARITY:
      // send stop bit.
      PS2_set_DATA();
      PS2_set_state(PS2_ST_SEND_STOP);
      break;
    case PS2_ST_SEND_STOP:
      PS2_commit_read_byte();
      // do we have data to send to keyboard?
      PS2_host_check_for_data();
      break;
    default:
      debug('&');
      debug('a' + PS2_get_state());
      break;
  }
}

void PS2_host_Timer() {
  PS2_disable_IRQ_timer0();
  switch (PS2_get_state()) {
    case PS2_ST_PREP_START:
      // we waited 100uS for device to notice us, bring DATA low and CLK hi
      PS2_clear_DATA();
      PS2_set_CLK();
      // really start bit...
      // now, wait for falling CLK
      PS2_enable_IRQ_CLK_Fall();
      PS2_set_state(PS2_ST_PREP_BIT);
      PS2_read_byte();
      break;
    default:
      debug2('#');
      debug2('a' + PS2_get_state());
      break;
  }
}