/*
    C=Key - Commodore <-> PS/2 Interface
    Copyright Jim Brain and RETRO Innovations, 2004-2011

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

    switches.h: Definitions for non-matrixed keyboard switches

*/

#ifndef _SWITCHES_H
#define _SWITCHES_H

#define SW_RX_BUFFER_SIZE 4     /* 2,4,8,16,32,64,128 or 256 bytes */
#define SW_RX_BUFFER_MASK ( SW_RX_BUFFER_SIZE - 1 )
#if ( SW_RX_BUFFER_SIZE & SW_RX_BUFFER_MASK )
  #error SW RX buffer size is not a power of 2
#endif

#define SW_TYPE_OUTPUT   0
#define SW_TYPE_INPUT    1

#if defined __AVR_ATmega162__
#define PORT_SW_OUT      PORTE
#define PORT_SW_IN       PINE
#define PORT_SW_DDR      DDRE
#elif defined __AVR_ATmega16__
#define PORT_SW_OUT      PORTD
#define PORT_SW_IN       PIND
#define PORT_SW_DDR      DDRD
#else
#  error Unknown CPU!
#endif

#define SW_UP             0x80

void SW_init(uint8_t type,uint8_t mask);
uint8_t SW_data_available( void );
void SW_send( uint8_t sw);
uint8_t SW_recv( void );
void SW_scan(void);

#endif //_SWITCHES_H_
