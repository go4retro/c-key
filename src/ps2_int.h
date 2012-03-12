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

    ps2_int.h: internal PS/2 helper functions and enums

*/

#ifndef PS2_INT_H
#define PS2_INT_H

#define PS2_RX_BUFFER_MASK   (_BV(PS2_RX_BUFFER_SHIFT) - 1)
#define PS2_TX_BUFFER_MASK   (_BV(PS2_TX_BUFFER_SHIFT) - 1)
/*
 * After a device sends a byte to host, it has to holdoff for a while
 * before doing anything else.  One KB I tested this is 2.14mS.
 */

/* PS2 Clock INT */
#if defined __AVR_ATmega8__ ||  defined __AVR_ATmega16__ || defined __AVR_ATmega32__ || defined __AVR_ATmega162__
#  define CLK_INTDR     MCUCR     // INT Direction Register
#  define CLK_INTCR     GICR      // INT Control Register
#  define CLK_INTFR     GIFR      // INT Flag Register
#  if PS2_PIN_CLK == _BV(PD3)
#    define CLK_ISC0      ISC10
#    define CLK_ISC1      ISC11
#    define CLK_INT       INT1
#    define CLK_INTF      INTF1
#    define CLK_INT_vect  INT1_vect
#  else 
#    define CLK_ISC0      ISC00
#    define CLK_ISC1      ISC01
#    define CLK_INT       INT0
#    define CLK_INTF      INTF0
#    define CLK_INT_vect  INT0_vect
#  endif
#else
#  define CLK_INTDR     EICRA     // INT Direction Register
#  define CLK_INTCR     EIMSK     // INT Control Register
#  define CLK_INTFR     EIFR      // INT Flag Register
#  if PS2_PIN_CLK == _BV(PD3)
#    define CLK_ISC0      ISC10
#    define CLK_ISC1      ISC11
#    define CLK_INT       INT1
#    define CLK_INTF      INTF1
#    define CLK_INT_vect  INT1_vect
#  else
#    define CLK_ISC0      ISC00
#    define CLK_ISC1      ISC01
#    define CLK_INT       INT0
#    define CLK_INTF      INTF0
#    define CLK_INT_vect  INT0_vect
#  endif
#endif

/* PS2 Timer */
#if defined __AVR_ATmega8__

#  define PS2_TIMER_COMP_vect   TIMER2_COMP_vect
#  define PS2_OCR               OCR2
#  define PS2_TCNT              TCNT2
#  define PS2_TCCR              TCCR2
#  define PS2_TCCR_DATA         _BV(CS21)
#  define PS2_TIFR              TIFR
#  define PS2_TIFR_DATA         _BV(OCF2)
#  define PS2_TIMSK             TIMSK
#  define PS2_TIMSK_DATA        _BV(OCIE2)

#elif defined __AVR_ATmega28__ || defined __AVR_ATmega48__ || defined __AVR_ATmega88__ || defined __AVR_ATmega168__

#  define PS2_TIMER_COMP_vect   TIMER2_COMPA_vect
#  define PS2_OCR               OCR2A
#  define PS2_TCNT              TCNT2
#  define PS2_TCCR              TCCR2B
#  define PS2_TCCR_DATA         _BV(CS21)
#  define PS2_TIFR              TIFR2
#  define PS2_TIFR_DATA         _BV(OCF2A)
#  define PS2_TIMSK             TIMSK2
#  define PS2_TIMSK_DATA        _BV(OCIE2A)

#elif defined __AVR_ATmega16__ || defined __AVR_ATmega32__ || defined __AVR_ATmega162__

#  define PS2_TIMER_COMP_vect   TIMER0_COMP_vect
#  define PS2_OCR               OCR0
#  define PS2_TCNT              TCNT0
#  define PS2_TCCR              TCCR0
#  define PS2_TCCR_DATA         (_BV(CS01) | _BV(WGM01))
#  define PS2_TIFR              TIFR
#  define PS2_TIFR_DATA         _BV(OCF0)
#  define PS2_TIMSK             TIMSK
#  define PS2_TIMSK_DATA        _BV(OCIE0)

#else
#  error Unknown chip!
#endif



#define PS2_HALF_CYCLE 40 // ~42 uS when all is said and done.
#define PS2_SEND_HOLDOFF_COUNT  ((uint8_t)(2140/PS2_HALF_CYCLE))

typedef enum {PS2_ST_IDLE
             ,PS2_ST_PREP_START
             ,PS2_ST_SEND_START
             ,PS2_ST_PREP_BIT
             ,PS2_ST_SEND_BIT
             ,PS2_ST_PREP_PARITY
             ,PS2_ST_SEND_PARITY
             ,PS2_ST_PREP_STOP
             ,PS2_ST_SEND_STOP
             ,PS2_ST_HOLDOFF
             ,PS2_ST_WAIT_START
             ,PS2_ST_GET_START
             ,PS2_ST_WAIT_BIT
             ,PS2_ST_GET_BIT
             ,PS2_ST_WAIT_PARITY
             ,PS2_ST_GET_PARITY
             ,PS2_ST_WAIT_STOP
             ,PS2_ST_GET_STOP
             ,PS2_ST_GET_ACK
             ,PS2_ST_WAIT_ACK
             ,PS2_ST_WAIT_ACK2
             ,PS2_ST_HOST_INHIBIT
             ,PS2_ST_WAIT_RESPONSE
             } ps2state_t;

#define PS2_SET_CLK()     do { PS2_PORT_CLK_OUT |= ( PS2_PIN_CLK); PS2_PORT_DDR_CLK &= (uint8_t)~(PS2_PIN_CLK); } while(0)
#define PS2_CLEAR_CLK()   do { PS2_PORT_DDR_CLK |= (PS2_PIN_CLK); PS2_PORT_CLK_OUT &= (uint8_t)~( PS2_PIN_CLK); } while(0)
#define PS2_READ_CLK()    (PS2_PORT_CLK_IN & (PS2_PIN_CLK))

#define PS2_SET_DATA()    do { PS2_PORT_DATA_OUT |= ( PS2_PIN_DATA); PS2_PORT_DDR_DATA &= (uint8_t)~(PS2_PIN_DATA); } while(0)
#define PS2_CLEAR_DATA()  do { PS2_PORT_DDR_DATA |= (PS2_PIN_DATA); PS2_PORT_DATA_OUT &= (uint8_t)~( PS2_PIN_DATA); } while(0)
#define PS2_READ_DATA()   (PS2_PORT_DATA_IN & (PS2_PIN_DATA))


#if defined PS2_USE_HOST && defined PS2_USE_DEVICE
#define PS2_CALL(dev,host) \
  switch(ps2_mode) {\
  case PS2_MODE_DEVICE: \
    dev; \
    break; \
  case PS2_MODE_HOST: \
    host; \
    break; \
  }
#else
#  if defined PS2_USE_DEVICE
#    define PS2_CALL(dev,host) dev
#  else
#    define PS2_CALL(dev,host) host
#  endif
#endif

void ps2_clear_buffers(void);

#endif

