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
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "util.h"
#include "ps2.h"

#define PS2_HALF_CYCLE 50
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

void PS2_init() {
  PS2_enable_IRQ_DATA_Fall();
	PS2_disable_IRQ_timer0();
	PS2_set_CLK();
	PS2_set_DATA();
  PS2_State=PS2_ST_DEV_IDLE;
  PS2_TxHead=0;
  PS2_TxTail=0;
  PS2_send(PS2_CMD_BAT);
}


void PS2_set_CLK() {
	// set pin HI
	PS2_PORT_CLK_OUT |= (1<< PS2_PIN_CLK);
	// set to input (1 above brings pull up resistor online.)
	PS2_PORT_DDR_CLK &= ~(1<<PS2_PIN_CLK);
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


void PS2_enable_IRQ_DATA_Rise() {
	//GICR &= ~(1 << INT0);
	GIFR |= (1<<INTF0);
	// rising edge
	MCUCR |= (1 << ISC01) | (1<< ISC00);
	// turn on
	GICR |= (1 << INT0);
}


void PS2_enable_IRQ_DATA_Fall() {
	//GICR &= ~(1 << INT0);
	GIFR |= (1<<INTF0);
	// rising edge
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1<<ISC00);
	// turn on
	GICR |= (1 << INT0);
}


void PS2_disable_IRQ_DATA() {
	GICR &= ~(1 << INT0);
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
	//OCR0=(char)(us);
	// sync prescaler...
	SFIOR |= (1<<PSR10);
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


unsigned char PS2_recv( void ) {
	unsigned char tmptail;
	
	while ( PS2_RxHead == PS2_RxTail ) {
    ;
	}
	tmptail = ( PS2_RxTail + 1 ) & PS2_RX_BUFFER_MASK;/* Calculate buffer index */
	
	PS2_RxTail = tmptail;                /* Store new index */
	
	return PS2_RxBuf[tmptail];           /* Return data */
}

void PS2_send( unsigned char data ) {
	unsigned char tmphead;
	/* Calculate buffer index */
	tmphead = ( PS2_TxHead + 1 ) & PS2_TX_BUFFER_MASK; /* Wait for free space in buffer */
	while ( tmphead == PS2_TxTail ) ;

	PS2_TxBuf[tmphead] = data;           /* Store data in buffer */
	PS2_TxHead = tmphead;                /* Store new index */

  cli();
  if(PS2_State == PS2_ST_DEV_IDLE) {
    // start transmission;
    PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);  // could be any amount.
  }
  sei();
}

unsigned char PS2_peek( void ) {
	return ( PS2_RxHead != PS2_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

void PS2_prep_bit() {
  // bring CLK hi
  PS2_set_CLK();
  PS2_State=PS2_ST_DEV_PREP_BIT;
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

void PS2_send_bit() {
	PS2_clear_CLK();
  PS2_State=PS2_ST_DEV_SEND_BIT;
}

SIGNAL(SIG_INTERRUPT1) {
	// turn off CLK IRQ
	PS2_disable_IRQ_CLK();
	switch(PS2_State) {
    case PS2_ST_DEV_WAIT_CLK_R:
      debug('A' + PS2_State);
      // CLK is HI, so clock
      PS2_check_CLK();
      break;
    case PS2_ST_DEV_IDLE:
      // no longer inhibited, send data.
      PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
      break;
    default:
      debug('&');
      debug('a' + PS2_State);
      break;

	}
}

void PS2_check_CLK() {
  if(PS2_read_CLK() == 0) {
    PS2_State=PS2_ST_DEV_WAIT_CLK_R;
    PS2_enable_IRQ_CLK_Rise();
  } else {
    PS2_enable_IRQ_timer0(PS2_HALF_CYCLE);
    PS2_State=PS2_ST_DEV_PREP_START_R;
  }
}

SIGNAL(SIG_INTERRUPT0) {
	PS2_disable_IRQ_DATA();
	switch (PS2_State) {
    case PS2_ST_DEV_IDLE:
      debug('A' + PS2_State);
      // DATA is low, so wait for CLK HI
      PS2_check_CLK();
      break;
    default:
      debug('*');
      debug('a' + PS2_State);
      break;
      
	}
}

SIGNAL(SIG_OUTPUT_COMPARE0) {
	unsigned char tmp;
  
	switch (PS2_State) {
    case PS2_ST_DEV_IDLE:
      PS2_disable_IRQ_DATA();
      debug('0' + PS2_State);
      if(PS2_read_CLK() == 1 && PS2_read_DATA() == 1) {
      // start bit (low)
        PS2_clear_DATA();
        PS2_State=PS2_ST_DEV_PREP_START;
      } else if(PS2_read_DATA() == 0) {
        // someone wants to send data to us.
        // DATA is low, so wait for CLK HI
        PS2_check_CLK();
        //debug2('+'); 
      } else if(PS2_read_CLK() == 0) {
        // keyboard is inhibiting us
        PS2_enable_IRQ_CLK_Rise();
        //debug2('-'); 
      }
      break;
    case PS2_ST_DEV_PREP_START:
      debug('0' + PS2_State);
      PS2_clear_CLK();
      PS2_State=PS2_ST_DEV_SEND_START;
      break;
    case PS2_ST_DEV_SEND_START:
      debug('0' + PS2_State);
      PS2_Bit_Count=0;
      PS2_One_Count=0;
      tmp = ( PS2_TxTail + 1 ) & PS2_TX_BUFFER_MASK;
      PS2_Byte = PS2_TxBuf[tmp];  /* Start transmition */
      debug2('d');
      printHex(PS2_Byte);
      PS2_prep_bit();
      break;
    case PS2_ST_DEV_PREP_BIT:
      debug('0' + PS2_State);
      PS2_send_bit();
      break;
    case PS2_ST_DEV_SEND_BIT:
      debug('0' + PS2_State);
      if(PS2_Bit_Count == 8) {
        // we are done..., do parity
        PS2_set_CLK();
        if((PS2_One_Count & 1) == 1) {
          PS2_clear_DATA();
        } else {
          PS2_set_DATA();
        }
        PS2_State=PS2_ST_DEV_PREP_PARITY;
      } else {
        PS2_prep_bit();
      }
      break;
    case PS2_ST_DEV_PREP_PARITY:
      debug('0' + PS2_State);
      // clock parity
      PS2_clear_CLK();
      PS2_State=PS2_ST_DEV_SEND_PARITY;
      break;
    case PS2_ST_DEV_SEND_PARITY:
      debug('0' + PS2_State);
      PS2_set_CLK();
      PS2_set_DATA();
      PS2_State=PS2_ST_DEV_PREP_STOP;
      break;
    case PS2_ST_DEV_PREP_STOP:
      debug('0' + PS2_State);
      PS2_clear_CLK();
      PS2_State=PS2_ST_DEV_SEND_STOP;
      break;
    case PS2_ST_DEV_SEND_STOP:
      debug('0' + PS2_State);
      PS2_set_CLK();
      PS2_State=PS2_ST_DEV_IDLE;
      PS2_enable_IRQ_DATA_Fall();
      tmp = ( PS2_TxTail + 1 ) & PS2_TX_BUFFER_MASK;
      PS2_TxTail = tmp;      /* Store new index */
      if ( PS2_TxHead != PS2_TxTail ) {
        // send another
      } else {
        PS2_disable_IRQ_timer0();
      }
      break;
    case PS2_ST_DEV_PREP_START_R:
      debug('A' + PS2_State);
      // set CLK lo
      PS2_clear_CLK();
      PS2_Byte = 0;
      PS2_Bit_Count=0;
      PS2_One_Count=0;
      PS2_State=PS2_ST_DEV_GET_START_R;
      break;
    case PS2_ST_DEV_GET_START_R:
      debug('A' + PS2_State);
      PS2_set_CLK();
      PS2_State=PS2_ST_DEV_PREP_BIT_R;
      break;
    case PS2_ST_DEV_PREP_BIT_R:
      debug('A' + PS2_State);
      PS2_clear_CLK();
      PS2_State=PS2_ST_DEV_GET_BIT_R;
      break;
    case PS2_ST_DEV_GET_BIT_R:
      debug('A' + PS2_State);
      PS2_set_CLK();
      // read in bit
      PS2_Byte=PS2_Byte<<1;
      PS2_Bit_Count++;
      if(PS2_read_DATA() == 1) {
        PS2_Byte|=1;
        PS2_One_Count++;
      }
      if(PS2_Bit_Count == 8) {
        // done, do Parity bit
        PS2_State=PS2_ST_DEV_PREP_PARITY_R;
      } else {
        PS2_State=PS2_ST_DEV_PREP_BIT_R;
      }
      break;
    case PS2_ST_DEV_PREP_PARITY_R:
      debug('A' + PS2_State);
      PS2_clear_CLK();
      PS2_State=PS2_ST_DEV_GET_PARITY_R;
      break;
    case PS2_ST_DEV_GET_PARITY_R:
      debug('A' + PS2_State);
      PS2_set_CLK();
      // ignore for now.
      PS2_State=PS2_ST_DEV_PREP_TURN_R;
      break;
    case PS2_ST_DEV_PREP_TURN_R:
      debug('A' + PS2_State);
      PS2_clear_CLK();
      PS2_State=PS2_ST_DEV_GET_TURN_R;
      break;
    case PS2_ST_DEV_GET_TURN_R:
      debug('A' + PS2_State);
      PS2_set_CLK();
      if(PS2_read_DATA() == 0)
        PS2_State=PS2_ST_DEV_PREP_TURN_R;
      else
        PS2_State=PS2_ST_DEV_PREP_ACK_R;
      break;
    case PS2_ST_DEV_PREP_ACK_R:
      debug('A' + PS2_State);
      PS2_set_CLK();
      PS2_clear_DATA();
      PS2_State=PS2_ST_DEV_SEND_ACK_R;
      break;
    case PS2_ST_DEV_SEND_ACK_R:
      debug('A' + PS2_State);
      PS2_clear_CLK();
      debug2('x');
      printHex(PS2_Byte);
      /* Calculate buffer index */
      tmp = ( PS2_RxHead + 1 ) & USART_RX_BUFFER_MASK;
      PS2_RxHead = tmp;      /* Store new index */
    
      if ( tmp == PS2_RxTail ) {
        /* ERROR! Receive buffer overflow */
      }
	
      PS2_RxBuf[tmp] = PS2_Byte; /* Store received data in buffer */
      PS2_State=PS2_ST_DEV_FINISH_R;
      break;
    case PS2_ST_DEV_FINISH_R:  
      debug('A' + PS2_State);
      PS2_set_CLK();
      PS2_set_DATA();
      if(PS2_read_DATA() == 0) {
        // DATA low, clock another byte in.
        PS2_State=PS2_ST_DEV_PREP_START_R;
      } else {
        PS2_State=PS2_ST_DEV_IDLE;
        // do we have data to send?
        if ( PS2_TxHead != PS2_TxTail ) {
          // send another
        } else {
          PS2_enable_IRQ_DATA_Fall();
          PS2_disable_IRQ_timer0();
          debug('D'); 
        }
      }
      break;
    default:
      debug2('#');
      debug2('a' + PS2_State);
      PS2_disable_IRQ_timer0();
      break;

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

