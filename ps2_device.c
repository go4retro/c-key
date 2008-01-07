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
#include "ps2_device.h"

static volatile uint8_t PS2_device_holdoff_count;

void PS2_device_host_inhibit(void) {
  // CLK is low.  Host wants to talk to us.
  // turn off timer
  PS2_disable_IRQ_timer0();
  // look for rising clock
  PS2_enable_IRQ_CLK_Rise();
  PS2_set_state(PS2_ST_HOST_INHIBIT);
  // release DATA line, if we happen to have it.
  PS2_set_DATA();
}

void PS2_device_init(void) {
  PS2_set_state(PS2_ST_IDLE);
  PS2_disable_IRQ_CLK();
  PS2_disable_IRQ_timer0();
  PS2_set_CLK();
  PS2_set_DATA();
  // wait 600mS.
  PS2_delay(600);
  //PS2_clear_CLK();
  //PS2_clear_DATA();
  //PS2_delay(775);
  //PS2_set_CLK();
  //PS2_delay(80);
  //PS2_set_DATA();
  //PS2_delay(384);
  //PS2_clear_CLK();
  //PS2_delay(483);
  //PS2_set_CLK();
  //PS2_delay(60);
  PS2_send(PS2_CMD_BAT);
  // need to do this once here, as CLK might already be low.
  //if(!PS2_read_CLK()) {
  //  PS2_device_host_inhibit();
  //} else {
  //  PS2_enable_IRQ_CLK_Fall();
  //}
}

inline void PS2_device_trigger_send(void) {
  // start clocking.
  // wait a half cycle
  //debug2('s');
  PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
  // bring DATA line low to ensure everyone knows our intentions
  PS2_clear_DATA();
  // set state
  PS2_set_state(PS2_ST_PREP_START);
}

void PS2_device_check_data(void) {
  //debug2('d');
  // do we have data to send?
  if(PS2_data_to_send()) {
    PS2_device_trigger_send();
  } else {
    //debug2('n');
    PS2_set_state(PS2_ST_IDLE);
    //PS2_set_DATA();
    PS2_disable_IRQ_timer0();
    PS2_enable_IRQ_CLK_Fall();
  }
}

void PS2_device_CLK(void) {
  PS2_disable_IRQ_CLK();
  //debug2(']');
  switch(PS2_get_state()) {
    case PS2_ST_IDLE:
    case PS2_ST_PREP_START:
      // host is holding us off.  Wait for CLK hi...
      PS2_device_host_inhibit();
      break;
    case PS2_ST_HOST_INHIBIT:
      //debug2('f');
      // CLK went hi
      if(PS2_read_DATA()) {
        //debug2('s');
        // we can send if we need to.
        PS2_device_check_data();
      } else {
        //debug2('r');
        // host wants to send data, CLK is high.
        // wait half cycle to let things settle.
        // clock in data from host.
        PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
        PS2_set_state(PS2_ST_WAIT_START);
      }
      break;
    default:
      debug('&');
      printHex(PS2_get_state());
      break;
  }
}

void PS2_device_Timer(void) {
  switch (PS2_get_state()) {
    case PS2_ST_PREP_START:
      // disable the CLK IRQ
      PS2_disable_IRQ_CLK();
      // clk the start bit
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_START);
      break;
    case PS2_ST_SEND_START:
      PS2_read_byte();
      // bring CLK hi
      if(PS2_set_CLK()) {
        PS2_write_bit();
      } else {
        PS2_device_host_inhibit();
      }
      break;
    case PS2_ST_PREP_BIT:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_BIT);
      break;
    case PS2_ST_SEND_BIT:
      if(PS2_set_CLK()) {
        if(PS2_get_count() == 8) {
          // we are done..., do parity
          PS2_write_parity();
          PS2_set_state(PS2_ST_PREP_PARITY);
        } else {
          // state is set in function.
          PS2_write_bit();
        }
      } else {
        PS2_device_host_inhibit();
      }
      break;
    case PS2_ST_PREP_PARITY:
      // clock parity
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_PARITY);
      break;
    case PS2_ST_SEND_PARITY:
      if(PS2_set_CLK()) {
        PS2_set_DATA();
        PS2_set_state(PS2_ST_PREP_STOP);
      } else {
        PS2_device_host_inhibit();
      }
      break;
    case PS2_ST_PREP_STOP:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_STOP);
      break;
    case PS2_ST_SEND_STOP:
      // If host wanted to abort, they had to do it before now.
      PS2_commit_read_byte();
      if(PS2_set_CLK()) { // bring CLK hi
        if(PS2_read_DATA()) {
          // for some reason, you have to wait a while before sending again.
          PS2_device_holdoff_count=PS2_SEND_HOLDOFF_COUNT;
          PS2_set_state(PS2_ST_HOLDOFF);
        } else {
          // Host wants to talk to us.
          PS2_set_state(PS2_ST_WAIT_START);
        }
      } else {
        //debug2('*');
        PS2_device_host_inhibit();
      }
      break;
    case PS2_ST_WAIT_START:
      // set CLK lo
      PS2_clear_CLK();
      PS2_clear_counters();
      // read start bit
      if(PS2_read_DATA()) {
        // not sure what you do if start bit is high...
        PS2_set_CLK();
        PS2_set_state(PS2_ST_IDLE);
        PS2_disable_IRQ_timer0();
        PS2_enable_IRQ_CLK_Fall();
        //debug2('-');
      } else {
        //debug2('+');
        PS2_set_state(PS2_ST_GET_START);
      }
      break;
    case PS2_ST_GET_START:
      if(PS2_set_CLK()) {
        PS2_set_state(PS2_ST_WAIT_BIT);
      } else {
        PS2_device_host_inhibit();
      }
      break;
    case PS2_ST_WAIT_BIT:
      PS2_clear_CLK();
      // you read incoming bits on falling clock.
      PS2_read_bit();
      PS2_set_state(PS2_ST_GET_BIT);
      break;
    case PS2_ST_GET_BIT:
      if(PS2_set_CLK()) {
        if(PS2_get_count() == 8) {
          // done, do Parity bit
          PS2_set_state(PS2_ST_GET_PARITY);
        } else {
          PS2_set_state(PS2_ST_WAIT_BIT);
        }
      } else {
        // host aborted send.
        PS2_device_host_inhibit();
      }
      break;
    case PS2_ST_GET_PARITY:
      PS2_clear_CLK();
      // ignore parity for now.
      PS2_set_state(PS2_ST_WAIT_STOP);
      break;
    case PS2_ST_WAIT_STOP:
      if(PS2_set_CLK()) {
        if(PS2_read_DATA()) {
          PS2_set_state(PS2_ST_WAIT_ACK);
          // bing DATA low to ack
          PS2_clear_DATA();
          // commit data
          //PS2_write_byte();  jlb, moved.
        } else {
          PS2_set_state(PS2_ST_GET_PARITY);
        }
      } else {
        // host aborted send.
        PS2_device_host_inhibit(); 
      }
      break;
    case PS2_ST_WAIT_ACK:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_GET_ACK);
      break;
    case PS2_ST_GET_ACK:  
      PS2_set_CLK();
      PS2_set_DATA();
      // we just need to wait a 50uS or so, to ensure the host saw the CLK go high
      PS2_device_holdoff_count=1;
      PS2_set_state(PS2_ST_HOLDOFF);
      PS2_write_byte();   //jlb moved
      break;
    case PS2_ST_HOLDOFF:
      PS2_device_holdoff_count--;
      if(!PS2_device_holdoff_count) {
        if(PS2_read_CLK()) {
          if(PS2_read_DATA()) {
            PS2_device_check_data();
          } else {
            PS2_set_state(PS2_ST_WAIT_START);
          }
        } else {
          PS2_device_host_inhibit();
        }
      }
      break;
    default:
      debug('#');
      printHex(PS2_get_state());
      PS2_disable_IRQ_timer0();
      break;
  } 
}