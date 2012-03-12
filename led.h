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

    led.h: Definitions for LED indicator routines

*/
#ifndef LED_H
#define LED_H

#define LED_DDR         DDRD
#define LED_PORT        PORTD
#define LED_PIN_0       (PIN0)
#define LED_PIN_1       (PIN1)
#define LED_PIN_2       (PIN2)
#define LED_PIN_3       (PIN3)
#define LED_PIN_4       (PIN4)
#define LED_PIN_5       (PIN5)
#define LED_PIN_6       (PIN6)
#define LED_PIN_7       (PIN7)

#define LED_FLAG_NONE   0
#define LED_FLAG_END_ON 0x80
#define LED_COUNT_MASK  ((uint8_t)~LED_FLAG_END_ON)

void LED_init(uint8_t led);
void LED_on(uint8_t led);
void LED_off(uint8_t led);
void LED_blink(uint8_t led, uint8_t count, uint8_t flags);
void LED_irq(void);
 
#endif

