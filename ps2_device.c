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
#include "ps2_device.h"

inline void PS2_device_init(void) {
  PS2_send(PS2_CMD_BAT);
}

inline void PS2_device_trigger_send(void) {
  PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
  PS2_clear_DATA();
  PS2_set_state(PS2_ST_PREP_START);
}

void PS2_device_check_data(void) {
  // do we have data to send?
  PS2_enable_IRQ_CLK_Fall();
  if(PS2_data_to_send()) {
    PS2_device_trigger_send();
  } else {
    PS2_set_state(PS2_ST_IDLE);
    PS2_clear_DATA();
    PS2_disable_IRQ_timer0();
  }
}

inline void PS2_device_CLK(void) {
  switch(PS2_get_state()) {
    case PS2_ST_ABORT:
      PS2_enable_IRQ_CLK_Fall();
      PS2_device_check_data();
      break;
    case PS2_ST_IDLE:
    case PS2_ST_PREP_START:
      // host wants to send us data.
      PS2_enable_IRQ_CLK_Rise();
      // we need to wait until CLK is high...
      PS2_set_state(PS2_ST_CHECK_CLK);
      // we should set a timeout...
      // fix
      //PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
      break;
    case PS2_ST_CHECK_CLK:
      if(PS2_read_DATA() == 0) {
        // host wants to send data, CLK is high.
        // wait half cycle to let things settle.
        PS2_disable_IRQ_CLK();
        // clock in data from host.
        PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
        PS2_set_state(PS2_ST_WAIT_START);
      } else {
        // CLK went high, but DATA is high, so 
        PS2_device_check_data();
      }
      break;
    default:
      debug('&');
      debug('a' + PS2_get_state());
      break;
  }
}

void PS2_device_abort(void) {
  // host aborts
  PS2_enable_IRQ_CLK_Rise();
  PS2_set_state(PS2_ST_ABORT);
}

inline void PS2_device_Timer(void) {
  switch (PS2_get_state()) {
    case PS2_ST_PREP_START:
      PS2_disable_IRQ_CLK();
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_START);
      break;
    case PS2_ST_SEND_START:
      PS2_read_byte();
      // bring CLK hi
      if(PS2_set_CLK() == 1) {
        PS2_write_bit();
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_PREP_BIT:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_BIT);
      break;
    case PS2_ST_SEND_BIT:
      if(PS2_get_count() == 8) {
        // we are done..., do parity
        if(PS2_set_CLK() == 1) {
          PS2_write_parity();
          PS2_set_state(PS2_ST_PREP_PARITY);
        } else {
          PS2_device_abort();
        }
      } else if(PS2_set_CLK() == 1) { 
        PS2_write_bit();
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_PREP_PARITY:
      // clock parity
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_PARITY);
      break;
    case PS2_ST_SEND_PARITY:
      if(PS2_set_CLK() == 1) {
        PS2_set_DATA();
        PS2_set_state(PS2_ST_PREP_STOP);
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_PREP_STOP:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_SEND_STOP);
      break;
    case PS2_ST_SEND_STOP:
      if(PS2_set_CLK() ==1) {
        PS2_commit_read_byte();
        PS2_device_check_data();
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_WAIT_START:
      // set CLK lo
      PS2_clear_CLK();
      PS2_clear_counters();
      PS2_set_state(PS2_ST_GET_START);
      break;
    case PS2_ST_GET_START:
      if(PS2_set_CLK() == 1) {
        // read start bit
        if(PS2_read_DATA() == 1) {
          // not sure what you do if start bit is high...
          PS2_set_state(PS2_ST_IDLE);
          PS2_disable_IRQ_timer0();
          PS2_enable_IRQ_CLK_Fall();
        } else {
          PS2_set_state(PS2_ST_WAIT_BIT);
        }
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_WAIT_BIT:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_GET_BIT);
      break;
    case PS2_ST_GET_BIT:
      if(PS2_set_CLK() == 1) {
        // read in bit
        PS2_read_bit();
        if(PS2_get_count() == 8) {
          // done, do Parity bit
          PS2_set_state(PS2_ST_WAIT_PARITY);
        } else {
          PS2_set_state(PS2_ST_WAIT_BIT);
        }
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_WAIT_PARITY:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_GET_PARITY);
      break;
    case PS2_ST_GET_PARITY:
      
      if(PS2_set_CLK() == 1) {
        // ignore for now.
        PS2_set_state(PS2_ST_WAIT_STOP);
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_WAIT_STOP:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_GET_STOP);
      break;
    case PS2_ST_GET_STOP:
      if(PS2_set_CLK() == 1) {
        if(PS2_read_DATA() == 0) {
          // ignore for now
        }
        PS2_set_state(PS2_ST_GET_ACK);
        PS2_clear_DATA();
      } else {
        PS2_device_abort();
      }
      break;
    case PS2_ST_GET_ACK:
      PS2_clear_CLK();
      PS2_set_state(PS2_ST_WAIT_ACK);
      break;
    case PS2_ST_WAIT_ACK:  
      PS2_set_CLK();
      PS2_set_state(PS2_ST_WAIT_ACK2);
      break;
    case PS2_ST_WAIT_ACK2:  
      if(PS2_read_DATA() == 1) {
        PS2_write_byte();
        PS2_device_check_data();
      } else {
        // have not received ack, so loop.
        PS2_clear_CLK();
        PS2_set_state(PS2_ST_WAIT_ACK);
      }
      break;
    default:
      debug2('#');
      debug2('a' + PS2_get_state());
      PS2_disable_IRQ_timer0();
      break;
  } 
}