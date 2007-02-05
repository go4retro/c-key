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

// timing information derived from http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/PS2/ps2.htm

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "util.h"
#include "ps2.h"
#include "ps2_device.h"
#include "ps2_host.h"

static unsigned char PS2_RxBuf[PS2_RX_BUFFER_SIZE];
static volatile unsigned char PS2_RxHead;
static volatile unsigned char PS2_RxTail;
static unsigned char PS2_TxBuf[PS2_TX_BUFFER_SIZE];
static volatile unsigned char PS2_TxHead;
static volatile unsigned char PS2_TxTail;

static volatile unsigned char PS2_State;
static volatile unsigned char PS2_Byte;
static volatile unsigned char PS2_Bit_Count;
static volatile unsigned char PS2_One_Count;

static volatile unsigned char PS2_Mode;

void PS2_init(unsigned char mode) {
  PS2_TxHead=0;
  PS2_TxTail=0;
  PS2_RxHead=0;
  PS2_RxTail=0;
  PS2_Mode=mode;

  PS2_disable_IRQ_timer0();
	PS2_set_CLK();
	PS2_set_DATA();
  PS2_State=PS2_ST_IDLE;
  if(PS2_Mode==PS2_MODE_DEVICE) {
    PS2_device_init();
  } else {
    PS2_host_init();
  }
}

unsigned char PS2_set_CLK() {
	// set pin HI
	PS2_PORT_CLK_OUT |= (1<< PS2_PIN_CLK);
	// set to input (1 above brings pull up resistor online.)
	PS2_PORT_DDR_CLK &= ~(1<<PS2_PIN_CLK);
  return PS2_read_CLK();
}

void PS2_clear_CLK() {
	// set to putput
	PS2_PORT_DDR_CLK |= (1<<PS2_PIN_CLK);
	// bring pin LO
	PS2_PORT_CLK_OUT &= ~(1<< PS2_PIN_CLK);
}

unsigned char PS2_read_CLK() {
	return (PS2_PORT_CLK_IN & (1<<PS2_PIN_CLK)) >> PS2_PIN_CLK;
}

void PS2_set_DATA() {
	// set pin HI
	PS2_PORT_DATA_OUT |= (1<< PS2_PIN_DATA);
	// set to input (1 above brings pull up resistor online.)
	PS2_PORT_DDR_DATA &= ~(1<<PS2_PIN_DATA);
}

void PS2_clear_DATA() {
	// set to putput
	PS2_PORT_DDR_DATA |= (1<<PS2_PIN_DATA);
	// bring pin LO
	PS2_PORT_DATA_OUT &= ~(1<< PS2_PIN_DATA);
}

unsigned char PS2_read_DATA() {
	return (PS2_PORT_DATA_IN & (1<<PS2_PIN_DATA)) >> PS2_PIN_DATA;
}

void PS2_enable_IRQ_CLK_Rise() {
	//GICR &= ~(1 << INT1);
	GIFR |= (1<<INTF1);
	// rising edge
	MCUCR |= (1 << ISC11) | (1<< ISC10);
	// turn on
	GICR |= (1 << INT1);
}

void PS2_enable_IRQ_CLK_Fall() {
	//GICR &= ~(1 << INT1);
	GIFR |= (1<<INTF1);
	// rising edge
	MCUCR |= (1 << ISC11);
	MCUCR &= ~(1<<ISC10);
	// turn on
	GICR |= (1 << INT1);
}

void PS2_disable_IRQ_CLK() {
	GICR &= ~(1 << INT1);
}

void PS2_enable_IRQ_timer0(int us) {
	//TCCR0 &=~((1<<CS01) | (1<<CS00));
	//TIMSK &= ~(1<<OCIE0);
	TIFR |= (1<<OCF0);
	// us is uS....  Need to * 14 to get ticks, then divide by 64...
	// cheat... * 16 / 64 = /4  
	// error over whole range...
	
	// clear TCNT0;
	TCNT0=0;
	// set the count...
	OCR0=(char)(us>>2);
	// set output compare IRQ
	TIMSK |= (1<<OCIE0);
	// set prescaler to System Clock/64 and COmpare Timer
	TCCR0 |=(1<<CS01) |(1<<CS00) | (1<<WGM01);
}

void PS2_disable_IRQ_timer0() {
	// turn off timer
	TCCR0 &=~((1<<CS01) | (1<<CS00));
	TIMSK &= ~(1<<OCIE0);
}

inline unsigned char PS2_get_state(void) {
  return PS2_State;
}

inline void PS2_set_state(unsigned char state) {
  PS2_State=state;
}

inline unsigned char PS2_get_count(void) {
  return PS2_Bit_Count;
}

unsigned char PS2_recv( void ) {
	unsigned char tmptail;
	
	while ( PS2_RxHead == PS2_RxTail ) {
    // wait for char to arrive, if none in Q
    ;
	}
  // Calculate buffer index
	tmptail = ( PS2_RxTail + 1 ) & PS2_RX_BUFFER_MASK;
  // Store new index
	PS2_RxTail = tmptail;
	return PS2_RxBuf[tmptail];
}

void PS2_send( unsigned char data ) {
	unsigned char tmphead;
	// Calculate buffer index
	tmphead = ( PS2_TxHead + 1 ) & PS2_TX_BUFFER_MASK; 
	while ( tmphead == PS2_TxTail ) {
    // Wait for free space in buffer
    ;
  }

  // Store data in buffer
	PS2_TxBuf[tmphead] = data;
  // Store new index
	PS2_TxHead = tmphead;

  // turn off IRQs
  cli();
  if(PS2_State == PS2_ST_IDLE) {
    // start transmission;
    if(PS2_Mode==PS2_MODE_DEVICE) {
      PS2_device_trigger_send();
    } else {
      PS2_host_trigger_send();
    }
  }
  // turn on IRQs
  sei();
}

unsigned char PS2_data_available( void ) {
	return ( PS2_RxHead != PS2_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

void PS2_write_byte(void) {
  unsigned char tmp;
  /* Calculate buffer index */
  tmp = ( PS2_RxHead + 1 ) & PS2_RX_BUFFER_MASK;
  PS2_RxHead = tmp;      /* Store new index */

  if ( tmp == PS2_RxTail ) {
    /* ERROR! Receive buffer overflow */
  }

  PS2_RxBuf[tmp] = PS2_Byte; /* Store received data in buffer */
  debug2('r');
  debug2('x');
  printHex(PS2_Byte);
}

void PS2_read_byte(void) {
  PS2_Bit_Count=0;
  PS2_One_Count=0;
  PS2_Byte = PS2_TxBuf[( PS2_TxTail + 1 ) & PS2_TX_BUFFER_MASK];  /* Start transmition */
  debug2('s');
  debug2('x');
  printHex(PS2_Byte);
}

void PS2_commit_read_byte(void) {
  PS2_TxTail = ( PS2_TxTail + 1 ) & PS2_TX_BUFFER_MASK;      /* Store new index */
}

unsigned char PS2_data_to_send(void) {
  return ( PS2_TxHead != PS2_TxTail );
}

void PS2_write_bit() {
  PS2_State=PS2_ST_PREP_BIT;
	// set DATA..
	switch (PS2_Byte & 1) {
		case 0:
			PS2_clear_DATA();
			break;
		case 1:
			PS2_One_Count++;
			PS2_set_DATA();
			break;
	}
	// shift right.
	PS2_Byte= PS2_Byte >> 1;
	PS2_Bit_Count++;
	// valid data now.
}

void PS2_read_bit(void) {
  PS2_Byte=PS2_Byte>>1;
  PS2_Bit_Count++;
  if(PS2_read_DATA() == 1) {
    PS2_Byte|=0x80;
    PS2_One_Count++;
  }
}

void PS2_write_parity(void) {
  if((PS2_One_Count & 1) == 1) {
    PS2_clear_DATA();
  } else {
    PS2_set_DATA();
  }
}

void PS2_clear_counters(void) {
  PS2_Byte = 0;
  PS2_Bit_Count=0;
  PS2_One_Count=0;
}

SIGNAL(SIG_INTERRUPT1) {
	// turn off CLK IRQ
  if(PS2_Mode==PS2_MODE_DEVICE) {
    PS2_device_CLK();
  } else {
    PS2_host_CLK();
  }
}

SIGNAL(SIG_OUTPUT_COMPARE0) {
  if(PS2_Mode==PS2_MODE_DEVICE) {
    PS2_device_Timer();
  } else {
    PS2_host_Timer();
  }
}

void PS2_handle_cmds(unsigned char data) {
    switch(data) {
      case PS2_CMD_RESET:
      case PS2_CMD_ENABLE:
      case PS2_CMD_DISABLE:
      default:
        PS2_send(PS2_CMD_ACK);
        break;
      case PS2_CMD_ECHO:
        PS2_send(PS2_CMD_ECHO);
        break;
      case PS2_CMD_SET_CODE_SET:
        PS2_send(PS2_CMD_ACK);
        if(PS2_recv() == 0) {
          PS2_send(2);
        }
        break;
      case PS2_CMD_SET_RATE:
        // this needs to be done in another area.
        break;
      case PS2_CMD_READ_ID:
        PS2_send(PS2_CMD_ACK);
        PS2_send(0xab);
        break;
      case PS2_CMD_LEDS:
        PS2_send(PS2_CMD_ACK);
        PS2_recv();
        break;
      case PS2_CMD_RESEND:
        break;
    }
}

unsigned int PS2_get_typematic_delay(unsigned char rate) {
  return (((rate & 0x30) >> 5) + 1) * 250;
}

unsigned int PS2_get_typematic_period(unsigned char rate) {
  return ((8 + (rate & 0x07)) * (1 << ((rate & 0x18) >> 3)) << 2);
}

