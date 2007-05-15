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
#ifndef LED_H
#define LED_H 1

#define LED_DDR         DDRD
#define LED_PORT        PORTD
#define LED_PIN_0       (1<<PIN0)
#define LED_PIN_1       (1<<PIN1)
#define LED_PIN_2       (1<<PIN2)
#define LED_PIN_3       (1<<PIN3)
#define LED_PIN_4       (1<<PIN4)
#define LED_PIN_5       (1<<PIN5)
#define LED_PIN_6       (1<<PIN6)
#define LED_PIN_7       (1<<PIN7)

#define LED_DELAY    14745600/2

void led_init(uint8_t led);
void led_on(uint8_t led);
void led_off(uint8_t led);
void led_blink(uint8_t times, uint8_t led);
 
#endif