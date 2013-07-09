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

    ps2.c: Internal functions for host/device PS/2 modes

    timing information derived from http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/PS2/ps2.htm
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "config.h"
#include "ps2.h"
#include "ps2_int.h"
#include "uart.h"

static uint8_t rxbuf[1 << PS2_RX_BUFFER_SHIFT];
static volatile uint8_t rx_head;
static volatile uint8_t rx_tail;
static uint8_t txbuf[1 << PS2_TX_BUFFER_SHIFT];
static volatile uint8_t tx_head;
static volatile uint8_t tx_tail;

static volatile ps2state_t ps2_state;
static volatile uint8_t ps2_byte;
static volatile uint8_t ps2_bit_count;
static volatile uint8_t ps2_parity;

static ps2mode_t ps2_mode;

static volatile uint8_t ps2_holdoff_count;

static void ps2_enable_clk_rise(void) {
  // turn off IRQ
  CLK_INTCR &= (uint8_t)~_BV(CLK_INT);
  // reset flag
  CLK_INTFR |= _BV(CLK_INTF);
  // rising edge
  CLK_INTDR |= _BV(CLK_ISC1) | _BV(CLK_ISC0);
  // turn on
  CLK_INTCR |= _BV(CLK_INT);
}

static void ps2_enable_clk_fall(void) {
  // turn off IRQ
  CLK_INTCR &= (uint8_t)~_BV(CLK_INT);
  // reset flag
  CLK_INTFR |= _BV(CLK_INTF);
  // falling edge
  CLK_INTDR = (CLK_INTDR & (uint8_t)~(_BV(CLK_ISC1) | _BV(CLK_ISC0))) | _BV(CLK_ISC1);
  // turn on
  CLK_INTCR |= _BV(CLK_INT);
}

static void ps2_disable_clk(void) {
  CLK_INTCR &= (uint8_t)~_BV(CLK_INT);
}

static void ps2_enable_timer(uint8_t us) {
  // clear flag.
  PS2_TIFR |= PS2_TIFR_DATA;
  // clear TCNT;
  PS2_TCNT = 0;
  // set the count...
#if F_CPU > 14000000
  // us is uS....  Need to * 14 to get ticks, then divide by 8...
  // cheat... * 14 / 8 = *2 = <<1
  PS2_OCR = (uint8_t)(us << 1);
#elif F_CPU > 7000000
  PS2_OCR = us;
#else
  PS2_OCR = (us >> 1);
#endif
  // enable output compare IRQ
  PS2_TIMSK |= PS2_TIMSK_DATA;
}

static void ps2_disable_timer(void) {
  // disable output compare IRQ
  PS2_TIMSK &= (uint8_t)~PS2_TIMSK_DATA;
}

static void ps2_write_byte(void) {
  uint8_t tmp;
  /* Calculate buffer index */
  tmp = ( rx_head + 1 ) & PS2_RX_BUFFER_MASK;
  rx_head = tmp;      /* Store new index */

  if ( tmp == rx_tail ) {
    /* ERROR! Receive buffer overflow */
  }
  rxbuf[tmp] = ps2_byte; /* Store received data in buffer */
}

static void ps2_read_byte(void) {
  ps2_bit_count = 0;
  ps2_parity = 0;
  ps2_byte = txbuf[( tx_tail + 1 ) & PS2_TX_BUFFER_MASK];  /* Start transmition */
}

static void ps2_commit_read_byte(void) {
  tx_tail = ( tx_tail + 1 ) & PS2_TX_BUFFER_MASK;      /* Store new index */
}

static uint8_t ps2_data_to_send(void) {
  return ( tx_head != tx_tail );
}

static void ps2_write_bit(void) {
  ps2_state=PS2_ST_PREP_BIT;
  // set DATA..
  switch (ps2_byte & 1) {
    case 0:
      PS2_CLEAR_DATA();
      break;
    case 1:
      ps2_parity++;
      PS2_SET_DATA();
      break;
  }
  // shift right.
  ps2_byte= ps2_byte >> 1;
  ps2_bit_count++;
  // valid data now.
}

static void ps2_read_bit(void) {
  ps2_byte = ps2_byte >> 1;
  ps2_bit_count++;
  if(PS2_READ_DATA()) {
    ps2_byte |= 0x80;
    ps2_parity++;
  }
}

static void ps2_write_parity(void) {
  if((ps2_parity & 1) == 1) {
    PS2_CLEAR_DATA();
  } else {
    PS2_SET_DATA();
  }
}

static void ps2_clear_counters(void) {
  ps2_byte = 0;
  ps2_bit_count = 0;
  ps2_parity = 0;
}

void ps2_clear_buffers(void) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    tx_head = 0;
    tx_tail = 0;
    rx_head = 0;
    rx_tail = 0;
  }
}

#ifdef PS2_ENABLE_DEVICE
static void ps2_device_trigger_send(void) {
  // start clocking.
  // wait a half cycle
  ps2_enable_timer(PS2_HALF_CYCLE);
  // bring DATA line low to ensure everyone knows our intentions
  PS2_CLEAR_DATA();
}
#endif

#ifdef PS2_ENABLE_HOST
static void ps2_host_trigger_send(void) {
  // need to get devices attention...
  ps2_disable_clk();
  PS2_CLEAR_CLK();
  // yes, bring CLK lo for 100uS
  ps2_enable_timer(100);
}
#endif

static void ps2_trigger_send(void) {
  // set state
  ps2_state = PS2_ST_PREP_START;
  PS2_CALL(ps2_device_trigger_send(),ps2_host_trigger_send());
}

#ifdef PS2_ENABLE_HOST
static void ps2_host_check_for_data(void) {
  if(ps2_data_to_send() != 0) {
    ps2_trigger_send();
  } else {
    // wait for something to receive
    ps2_state = PS2_ST_IDLE;
    ps2_enable_clk_fall();
  }
}

static inline void ps2_host_timer_irq(void) {
  ps2_disable_timer();
  switch (ps2_state) {
    case PS2_ST_GET_BIT:
    case PS2_ST_GET_PARITY:
    case PS2_ST_GET_STOP:
      // do we have data to send to keyboard?
      ps2_host_check_for_data();
      break;
    case PS2_ST_PREP_START:
      // we waited 100uS for device to notice us, bring DATA low and CLK hi
      PS2_CLEAR_DATA();
      PS2_SET_CLK();
      if(!PS2_READ_CLK()) {
        // kb wants to talk to us.
        PS2_SET_DATA();
        ps2_enable_clk_fall();
        ps2_state = PS2_ST_GET_BIT;
      } else {
        // really start bit...
        // now, wait for falling CLK
        ps2_enable_clk_fall();
        //ps2_state = PS2_ST_SEND_START;  JLB incorrect
        ps2_state = PS2_ST_PREP_BIT;
        ps2_read_byte();
      }
      break;
    default:
      break;
  }
}

static inline void ps2_host_clk_irq(void) {
  switch(ps2_state) {
    case PS2_ST_WAIT_RESPONSE:
    case PS2_ST_IDLE:
      // keyboard sent start bit
      // should read it, but will assume it is good.
      ps2_state = PS2_ST_GET_BIT;
      // if we don't get another CLK in 100uS, timeout.
      ps2_enable_timer(100);
      ps2_clear_counters();
      break;
    case PS2_ST_GET_BIT:
      // if we don't get another CLK in 100uS, timeout.
      ps2_enable_timer(100);
      // read bit;
      ps2_read_bit();
      if(ps2_bit_count == 8) {
        // done, do Parity bit
        ps2_state = PS2_ST_GET_PARITY;
      }
      break;
    case PS2_ST_GET_PARITY:
      // if we don't get another CLK in 100uS, timeout.
      ps2_enable_timer(100);
      // grab parity
      // for now, assume it is OK.
      ps2_state = PS2_ST_GET_STOP;
      break;
    case PS2_ST_GET_STOP:
      ps2_disable_timer();
      // stop bit
      // for now, assume it is OK.
      ps2_write_byte();
      // wait for CLK to rise before doing anything else.
      ps2_state = PS2_ST_HOLDOFF;
      ps2_enable_clk_rise();
      break;
    case PS2_ST_HOLDOFF:
      // CLK rose, so now, check for more data.
      // do we have data to send to keyboard?
      ps2_host_check_for_data();
      break;
//    case PS2_ST_SEND_START:
//      ps2_state = PS2_ST_PREP_BIT;
//      break;
    case PS2_ST_PREP_BIT:
      // time to send bits...
      if(ps2_bit_count == 8) {
        // we are done..., do parity
        ps2_write_parity();
        ps2_state = PS2_ST_SEND_PARITY;
      } else {
        ps2_write_bit();
      }
      break;
    case PS2_ST_SEND_PARITY:
      // send stop bit.
      PS2_SET_DATA();
      ps2_state = PS2_ST_SEND_STOP;
      break;
    case PS2_ST_SEND_STOP:
      if(!PS2_READ_DATA()) {
        // commit the send
        ps2_commit_read_byte();
        /*
         * We could wait for the CLK hi, then check to see if we have more
         * data to send.  However, all cmds out have a required ack or response
         * so we'll just set to a non-IDLE state and wait for the CLK
         */
        ps2_state = PS2_ST_WAIT_RESPONSE;
        ps2_enable_clk_fall();
      } else {
        // wait for another cycle.  We should timeout here, I think
      }
      break;
    default:
      break;
  }
}

static void ps2_host_init(void) {
  ps2_enable_clk_fall();
}
#endif

#ifdef PS2_ENABLE_DEVICE
static void ps2_device_check_data(void) {
  // do we have data to send?
  if(ps2_data_to_send()) {
    ps2_trigger_send();
  } else {
    ps2_state = PS2_ST_IDLE;
    ps2_disable_timer();
    ps2_enable_clk_fall();
  }
}

static void ps2_device_host_inhibit(void) {
  // CLK is low.  Host wants to talk to us.
  // turn off timer
  ps2_disable_timer();
  // look for rising clock
  ps2_enable_clk_rise();
  ps2_state = PS2_ST_HOST_INHIBIT;
  // release DATA line, if we happen to have it.
  PS2_SET_DATA();
}

static inline void ps2_device_timer_irq(void) {
  switch (ps2_state) {
    case PS2_ST_PREP_START:
      // disable the CLK IRQ
      ps2_disable_clk();
      // clk the start bit
      PS2_CLEAR_CLK();
      ps2_state = PS2_ST_SEND_START;
      break;
    case PS2_ST_SEND_START:
      ps2_read_byte();
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        ps2_write_bit();
      } else {
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_PREP_BIT:
      PS2_CLEAR_CLK();
      ps2_state = PS2_ST_SEND_BIT;
      break;
    case PS2_ST_SEND_BIT:
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        if(ps2_bit_count == 8) {
          // we are done..., do parity
          ps2_write_parity();
          ps2_state = PS2_ST_PREP_PARITY;
        } else {
          // state is set in function.
          ps2_write_bit();
        }
      } else {
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_PREP_PARITY:
      // clock parity
      PS2_CLEAR_CLK();
      ps2_state = PS2_ST_SEND_PARITY;
      break;
    case PS2_ST_SEND_PARITY:
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        PS2_SET_DATA();
        ps2_state = PS2_ST_PREP_STOP;
      } else {
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_PREP_STOP:
      PS2_CLEAR_CLK();
      ps2_state = PS2_ST_SEND_STOP;
      break;
    case PS2_ST_SEND_STOP:
      // If host wanted to abort, they had to do it before now.
      ps2_commit_read_byte();
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        if(PS2_READ_DATA()) {
          // for some reason, you have to wait a while before sending again.
          ps2_holdoff_count=PS2_SEND_HOLDOFF_COUNT;
          ps2_state = PS2_ST_HOLDOFF;
        } else {
          // Host wants to talk to us.
          ps2_state = PS2_ST_WAIT_START;
        }
      } else {
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_WAIT_START:
      // set CLK lo
      PS2_CLEAR_CLK();
      ps2_clear_counters();
      // read start bit
      if(PS2_READ_DATA()) {
        // not sure what you do if start bit is high...
        PS2_SET_CLK();
        ps2_state = PS2_ST_IDLE;
        ps2_disable_timer();
        ps2_enable_clk_fall();
      } else {
        ps2_state = PS2_ST_GET_START;
      }
      break;
    case PS2_ST_GET_START:
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        ps2_state = PS2_ST_WAIT_BIT;
      } else {
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_WAIT_BIT:
      PS2_CLEAR_CLK();
      // you read incoming bits on falling clock.
      ps2_read_bit();
      ps2_state = PS2_ST_GET_BIT;
      break;
    case PS2_ST_GET_BIT:
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        if(ps2_bit_count == 8) {
          // done, do Parity bit
          ps2_state = PS2_ST_GET_PARITY;
        } else {
          ps2_state = PS2_ST_WAIT_BIT;
        }
      } else {
        // host aborted send.
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_GET_PARITY:
      PS2_CLEAR_CLK();
      // ignore parity for now.
      ps2_state = PS2_ST_WAIT_STOP;
      break;
    case PS2_ST_WAIT_STOP:
      PS2_SET_CLK();  // bring CLK hi
      if(PS2_READ_CLK()) {
        if(PS2_READ_DATA()) {
          ps2_state = PS2_ST_WAIT_ACK;
          // bing DATA low to ack
          PS2_CLEAR_DATA();
          // commit data
          //ps2_write_byte();  jlb, moved.
        } else {
          ps2_state = PS2_ST_GET_PARITY;
        }
      } else {
        // host aborted send.
        ps2_device_host_inhibit();
      }
      break;
    case PS2_ST_WAIT_ACK:
      PS2_CLEAR_CLK();
      ps2_state = PS2_ST_GET_ACK;
      break;
    case PS2_ST_GET_ACK:
      PS2_SET_CLK();
      PS2_SET_DATA();
      // we just need to wait a 50uS or so, to ensure the host saw the CLK go high
      ps2_holdoff_count = 1;
      ps2_state = PS2_ST_HOLDOFF;
      ps2_write_byte();   //jlb moved
      break;
    case PS2_ST_HOLDOFF:
      ps2_holdoff_count--;
      if(!ps2_holdoff_count) {
        if(PS2_READ_CLK()) {
          if(PS2_READ_DATA()) {
            ps2_device_check_data();
          } else {
            ps2_state = PS2_ST_WAIT_START;
          }
        } else {
          ps2_device_host_inhibit();
        }
      }
      break;
    default:
      ps2_disable_timer();
      break;
  }
}

static inline void ps2_device_clk_irq(void) {
  ps2_disable_clk();

  switch(ps2_state) {
    case PS2_ST_IDLE:
    case PS2_ST_PREP_START:
      // host is holding us off.  Wait for CLK hi...
      ps2_device_host_inhibit();
      break;
    case PS2_ST_HOST_INHIBIT:
      // CLK went hi
      if(PS2_READ_DATA()) {
        // we can send if we need to.
        ps2_device_check_data();
      } else {
        // host wants to send data, CLK is high.
        // wait half cycle to let things settle.
        // clock in data from host.
        ps2_enable_timer(PS2_HALF_CYCLE);
        ps2_state = PS2_ST_WAIT_START;
      }
      break;
    default:
      break;
  }
}

static void ps2_device_init(void) {
  ps2_disable_clk();
  ps2_disable_timer();
  PS2_SET_CLK();
  PS2_SET_DATA();
  // wait 600mS.
  _delay_ms(600);
  ps2_putc(PS2_CMD_BAT);
}
#endif

ISR(PS2_TIMER_COMP_vect) {
  PS2_CALL(ps2_device_timer_irq(),ps2_host_timer_irq());
}

ISR(CLK_INT_vect) {
  PS2_CALL(ps2_device_clk_irq(),ps2_host_clk_irq());
}

uint8_t ps2_getc( void ) {
  uint8_t tmptail;

  while ( rx_head == rx_tail ) {
    // wait for char to arrive, if none in Q
    ;
  }
  // Calculate buffer index
  tmptail = ( rx_tail + 1 ) & PS2_RX_BUFFER_MASK;
  // Store new index
  rx_tail = tmptail;
  return rxbuf[tmptail];
}

void ps2_putc( uint8_t data ) {
  uint8_t tmphead;
  // Calculate buffer index
  tmphead = ( tx_head + 1 ) & PS2_TX_BUFFER_MASK;
  while ( tmphead == tx_tail ) {
    // Wait for free space in buffer
    ;
  }
  // Store data in buffer
  txbuf[tmphead] = data;
  // Store new index
  tx_head = tmphead;

  // turn off IRQs
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if(ps2_state == PS2_ST_IDLE) {
      // start transmission;
      ps2_trigger_send();
    }
  }
}

uint8_t ps2_data_available( void ) {
  return ( rx_head != rx_tail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

void ps2_init(ps2mode_t mode) {
  // set prescaler to System Clock/8
  PS2_TCCR = PS2_TCCR_DATA;

  ps2_mode = mode;
  ps2_clear_buffers();

  PS2_SET_CLK();
  PS2_SET_DATA();

  ps2_state = PS2_ST_IDLE;
  PS2_CALL(ps2_device_init(),ps2_host_init());
}
