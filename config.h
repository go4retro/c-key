/*
    Copyright Jim Brain and Brain Innovations, 2008
  
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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "autoconf.h"

#if CONFIG_HARDWARE_VARIANT == 1

/* MODE_DETECT must return non-zero when host mode is desired */
#define MODE_DETECT       (PIND & (1 << PIN5))
#define RESET_INIT()        do {} while(0)
#define RESET_ON()        do {} while(0)
#define RESET_OFF()        do {} while(0)

#elif CONFIG_HARDWARE_VARIANT == 2

#define MODE_DETECT       (PIND & (1 << PIN4))
#define RESET_INIT()      do {PORTD |= (1 << PIN5); } while(0)
#define RESET_ON()        do {DDRD |= (1 << PIN5); PORTD &= (uint8_t)~(1 << PIN5); } while(0)
#define RESET_OFF()       do {DDRD &= (uint8_t)~(1 << PIN5); PORTD |= ~(1 << PIN5); } while(0)

#endif

#ifdef CONFIG_DEBUG_DATA
#define debug(x)          uart_putc(x)
#else
#define debug(x)          do {} while(0)
#endif

#ifndef TRUE
#define TRUE              1
#define FALSE             0
#endif


#endif /*CONFIG_H_*/
