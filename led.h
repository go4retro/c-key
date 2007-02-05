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
#ifndef LED_H
#define LED_H 1

#define LED_DDR      DDRD
#define LED_PORT     PORTD
#define LED_PIN_0      0
#define LED_PIN_1      1
#define LED_PIN_2      2
#define LED_PIN_3      3
#define LED_PIN_4      4
#define LED_PIN_5      5
#define LED_PIN_6      6
#define LED_PIN_7      7

#define LED_DELAY    14745600/2

void led_on(unsigned char led);
void led_off(unsigned char led);
void led_blink(unsigned char times, unsigned char led);
 
#endif