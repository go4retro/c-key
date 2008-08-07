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

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "ps2_device.h"
#include "ps2_host.h"
#include "uart.h"
#include "ps2.h"

static unsigned char PS2_RxBuf[PS2_RX_BUFFER_SIZE];
static volatile uint8_t PS2_RxHead;
static volatile uint8_t PS2_RxTail;
static unsigned char PS2_TxBuf[PS2_TX_BUFFER_SIZE];
static volatile uint8_t PS2_TxHead;
static volatile uint8_t PS2_TxTail;

static volatile uint8_t PS2_State;
static volatile uint8_t PS2_Byte;
static volatile uint8_t PS2_Bit_Count;
static volatile uint8_t PS2_One_Count;

static volatile uint8_t PS2_Mode;
static volatile uint8_t PS2_LEDs;
static uint8_t PS2_CodeSet;
static volatile uint8_t PS2_debug;

void PS2_clear_buffers(void) {
  PS2_TxHead=0;
  PS2_TxTail=0;
  PS2_RxHead=0;
  PS2_RxTail=0;
}

uint8_t PS2_set_CLK() {
	// set pin HI
	PS2_PORT_CLK_OUT |= ( PS2_PIN_CLK);
	// set to input (1 above brings pull up resistor online.)
	PS2_PORT_DDR_CLK &= (uint8_t)~(PS2_PIN_CLK);
  return PS2_read_CLK();
}

void PS2_clear_CLK() {
	// set to putput
	PS2_PORT_DDR_CLK |= (PS2_PIN_CLK);
	// bring pin LO
	PS2_PORT_CLK_OUT &= (uint8_t)~( PS2_PIN_CLK);
}

uint8_t PS2_read_CLK() {
	return (PS2_PORT_CLK_IN & (PS2_PIN_CLK));
}

void PS2_set_DATA() {
	// set pin HI
	PS2_PORT_DATA_OUT |= ( PS2_PIN_DATA);
	// set to input (1 above brings pull up resistor online.)
	PS2_PORT_DDR_DATA &= (uint8_t)~(PS2_PIN_DATA);
}

void PS2_clear_DATA() {
	// set to putput
	PS2_PORT_DDR_DATA |= (PS2_PIN_DATA);
	// bring pin LO
	PS2_PORT_DATA_OUT &= (uint8_t)~( PS2_PIN_DATA);
}

uint8_t PS2_read_DATA() {
	return (PS2_PORT_DATA_IN & (PS2_PIN_DATA));
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
	MCUCR &=(uint8_t)~(1<<ISC10);
	// turn on
	GICR |= (1 << INT1);
}

void PS2_disable_IRQ_CLK() {
	GICR &= (uint8_t)~(1 << INT1);
}

void PS2_delay(uint16_t ms) {
  // careful, we use the same timer as the IRQ.
  // set to /1024. and CTC mode
  OCR=14;  // 14 is ~1ms
  TCNT=0;
  TCCR = TCCR_DATA_DELAY;
  do {
    while(!(TIFR & TIFR_DATA));
    TIFR|=TIFR_DATA;
  } while (ms-- > 0);
  // shut timer off.
  TCCR=0;
}

void PS2_enable_IRQ_timer0(uint8_t us) {
	//TCCR0 &=~(1<<CS01);
	//TIMSK &= ~(1<<OCIE0);
	TIFR |= TIFR_DATA;
	// us is uS....  Need to * 14 to get ticks, then divide by 8...
	// cheat... * 16 / 8 = *2 = <<1  
	
	// clear TCNT0;
	TCNT=0;
	// set the count...
	OCR=(uint8_t)(us<<1);
	// set output compare IRQ
	TIMSK |= TIMSK_DATA;
	// set prescaler to System Clock/8 and Compare Timer
  TCCR =TCCR_DATA;
}

void PS2_disable_IRQ_timer0() {
	// turn off timer
  TCCR =0;
	TIMSK &=(uint8_t)~TIMSK_DATA;
}

void PS2_init(uint8_t mode) {
  PS2_clear_buffers();
  PS2_Mode=mode;
  PS2_LEDs=0;
  PS2_CodeSet=2;
  PS2_debug=FALSE;
  
  PS2_set_CLK();
  PS2_set_DATA();
  
  PS2_State=PS2_ST_IDLE;
  if(PS2_Mode==PS2_MODE_DEVICE) {
    PS2_device_init();
  } else {
    PS2_host_init();
  }
}

uint8_t PS2_get_state(void) {
  return PS2_State;
}

void PS2_set_state(uint8_t state) {
  PS2_State=state;
}

uint8_t PS2_get_count(void) {
  return PS2_Bit_Count;
}

uint8_t PS2_recv( void ) {
	uint8_t tmptail;
	
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

void PS2_send( uint8_t data ) {
	uint8_t tmphead;
	// Calculate buffer index
	tmphead = ( PS2_TxHead + 1 ) & PS2_TX_BUFFER_MASK; 
	while ( tmphead == PS2_TxTail ) {
    // Wait for free space in buffer
    ;
  }
  
  //uart_puthex(data);
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

uint8_t PS2_data_available( void ) {
	return ( PS2_RxHead != PS2_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

void PS2_write_byte(void) {
  uint8_t tmp;
  /* Calculate buffer index */
  tmp = ( PS2_RxHead + 1 ) & PS2_RX_BUFFER_MASK;
  PS2_RxHead = tmp;      /* Store new index */

  if ( tmp == PS2_RxTail ) {
    /* ERROR! Receive buffer overflow */
  }

  if(PS2_debug) {
    debug('i');
    uart_puthex(PS2_Byte);
  }
  PS2_RxBuf[tmp] = PS2_Byte; /* Store received data in buffer */
}

void PS2_read_byte(void) {
  PS2_Bit_Count=0;
  PS2_One_Count=0;
  PS2_Byte = PS2_TxBuf[( PS2_TxTail + 1 ) & PS2_TX_BUFFER_MASK];  /* Start transmition */
  if(PS2_debug) {
    debug('o');
    uart_puthex(PS2_Byte);
  }
}

void PS2_commit_read_byte(void) {
  PS2_TxTail = ( PS2_TxTail + 1 ) & PS2_TX_BUFFER_MASK;      /* Store new index */
}

uint8_t PS2_data_to_send(void) {
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
  if(PS2_read_DATA()) {
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
  if(PS2_Mode==PS2_MODE_DEVICE) {
    PS2_device_CLK();
  } else {
    PS2_host_CLK();
  }
}

SIGNAL(SIG_OUTPUT_COMPARE) {
  if(PS2_Mode==PS2_MODE_DEVICE) {
    PS2_device_Timer();
  } else {
    PS2_host_Timer();
  }
}

void PS2_handle_cmds(uint8_t data) {
  uint8_t i;
  
    switch(data) {
      case PS2_CMD_ACK:
        //ignore.
        break;
      case PS2_CMD_RESET:
        PS2_send(PS2_CMD_ACK);
        PS2_send(PS2_CMD_BAT);
        break;
      case PS2_CMD_DISABLE:
        // we should disable sending output if we receive this command.
      case PS2_CMD_ENABLE:
        //clear out KB buffers 
        cli();
        PS2_clear_buffers();
        sei();
        PS2_send(PS2_CMD_ACK);
        break;
      default:
        PS2_send(PS2_CMD_ACK);
        break;
      case PS2_CMD_ECHO:
        PS2_send(PS2_CMD_ECHO);
        break;
      case PS2_CMD_SET_CODE_SET:
        PS2_send(PS2_CMD_ACK);
        i=PS2_recv();
        if(i == 0) {
          PS2_send(PS2_CodeSet);
        } else {
          PS2_CodeSet=i;
        }
        break;
      case PS2_CMD_SET_RATE:
        // this should to be caught in another area, ignore if received here.
        break;
      case PS2_CMD_READ_ID:
        PS2_send(PS2_CMD_ACK);
        PS2_send(0xab);
        PS2_send(0x83);
        break;
      case PS2_CMD_LEDS:
        PS2_send(PS2_CMD_ACK);
        PS2_LEDs=PS2_recv()&0x07;
        PS2_send(PS2_CMD_ACK);
        break;
      case PS2_CMD_RESEND:
        break;
    }
}

uint16_t PS2_get_typematic_delay(uint8_t rate) {
  return (((rate & 0x30) >> 5) + 1) * 250;
}

uint16_t PS2_get_typematic_period(uint8_t rate) {
  return ((8 + (rate & 0x07)) * (1 << ((rate & 0x18) >> 3)) << 2);
}

void PS2_set_debug(uint8_t b) {
  PS2_debug=b;
}


