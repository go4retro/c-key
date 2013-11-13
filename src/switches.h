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

#ifndef SWITCHES_H
#define SWITCHES_H

#ifndef SW_RX_BUFFER_SHIFT
#  define SW_RX_BUFFER_SHIFT 2     /* log2 of size */
#endif

#if !defined PORT_SW_DDR
#  error Unknown PORT_SW!
#endif

#define SW_UP            0x80

void sw_init(uint8_t mask);
uint8_t sw_data_available(void);
void sw_putc( uint8_t sw);
uint8_t sw_getc( void );
void sw_scan(void);

#endif // SWITCHES_H
