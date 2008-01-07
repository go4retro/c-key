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
#include "util.h"
#include "ps2.h"
#include "ps2_host.h"

void PS2_host_init(void) {
  PS2_enable_IRQ_CLK_Fall();
}

void PS2_host_trigger_send(void) {
  //debug2('t');
  // need to get devices attention...
  PS2_set_state(PS2_ST_PREP_START);
  PS2_disable_IRQ_CLK();
  PS2_clear_CLK();
  // yes, bring CLK lo for 100uS
  PS2_enable_IRQ_timer0(100);
}

inline void PS2_host_check_for_data(void) {
    if(PS2_data_to_send() != 0) {
      //debug2('d');
      PS2_host_trigger_send();
    } else {
      //debug2('n');
      // wait for something to receive
      PS2_set_state(PS2_ST_IDLE);
      PS2_enable_IRQ_CLK_Fall();
    }
}

inline void PS2_host_CLK() {
  switch(PS2_get_state()) {
    case PS2_ST_WAIT_RESPONSE:
    case PS2_ST_IDLE:
      // keyboard sent start bit
      // should read it, but will assume it is good.
      PS2_set_state(PS2_ST_GET_BIT);
      // if we don't get another CLK in 100uS, timeout.
      PS2_enable_IRQ_timer0(100);
      PS2_clear_counters();
      break;
    case PS2_ST_GET_BIT:
      // if we don't get another CLK in 100uS, timeout.
      PS2_enable_IRQ_timer0(100);
      // read bit;
      PS2_read_bit();
      if(PS2_get_count() == 8) {
        // done, do Parity bit
        PS2_set_state(PS2_ST_GET_PARITY);
      }
      break;
    case PS2_ST_GET_PARITY:
      // if we don't get another CLK in 100uS, timeout.
      PS2_enable_IRQ_timer0(100);
      // grab parity
      // for now, assume it is OK.
      PS2_set_state(PS2_ST_GET_STOP);
      break;
    case PS2_ST_GET_STOP:
      PS2_disable_IRQ_timer0();
      // stop bit
      // for now, assume it is OK.
      PS2_write_byte();
      // wait for CLK to rise before doing anything else.
      PS2_set_state(PS2_ST_HOLDOFF);
      PS2_enable_IRQ_CLK_Rise();
      break;
    case PS2_ST_HOLDOFF:
      // CLK rose, so now, check for more data.
      // do we have data to send to keyboard?
      PS2_host_check_for_data();
      break;
    case PS2_ST_PREP_BIT:
      //debug2('b');
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
      if(!PS2_read_DATA()) {
        // commit the send
        PS2_commit_read_byte();
        
        /*
         * We could wait for the CLK hi, then check to see if we have more
         * data to send.  However, all cmds out have a required ack or response
         * so we'll just set to a non-IDLE state and wait for the CLK
         */
        //debug2('a');
        PS2_set_state(PS2_ST_WAIT_RESPONSE);
        PS2_enable_IRQ_CLK_Fall();
      } else {
        // wait for another cycle.  We should timeout here, I think
      }
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
    case PS2_ST_GET_BIT:
    case PS2_ST_GET_PARITY:
    case PS2_ST_GET_STOP:
      //debug2('T');
      //debug2('x' + PS2_get_state());
      // do we have data to send to keyboard?
      PS2_host_check_for_data();
      break;
    case PS2_ST_PREP_START:
      //debug2('p');
      // we waited 100uS for device to notice us, bring DATA low and CLK hi
      PS2_clear_DATA();
      if(!PS2_set_CLK()) {
        //debug2(':');
        // kb wants to talk to us.
        PS2_set_DATA();
        PS2_enable_IRQ_CLK_Fall();
        PS2_set_state(PS2_ST_GET_BIT);
      } else {
        //debug2(';');
        // really start bit...
        // now, wait for falling CLK
        PS2_enable_IRQ_CLK_Fall();
        PS2_set_state(PS2_ST_PREP_BIT);
        PS2_read_byte();
      }
      break;
    default:
      debug('#');
      debug('a' + PS2_get_state());
      break;
  }
}