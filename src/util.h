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
#ifndef UTIL_H
#define UTIL_H 1

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#define debug(x) uart_putc(x)

// a= DDR, b= PORT, C = PIN#
#define PIN_SET_HIZ(a,b,c) { a&=(uint8_t)~(1<<c) ; b|=(1<<c); } 
#define PIN_SET_LOW(a,b,c) { a|=(1<<c) ; b&=(uint8_t)~(1<<c); } 
#define PIN_SET_HI(a,b,c) { a|=(1<<c) ; b|=(1<<c); } 

#endif

