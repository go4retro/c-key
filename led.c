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
#include "led.h"

void led_on(unsigned char led) {
  LED_PORT |=(1<<led);
}

void led_off(unsigned char led) {
  LED_PORT &=~(1<<led);
}

void led_blink(unsigned char times, unsigned char led) {
  unsigned int i;
  unsigned long delay;
  
  for(i=0;i<times;i++) {
    led_on(led);
    for(delay=0;delay<LED_DELAY;delay++) { ; }
    led_off(led);
    for(delay=0;delay<LED_DELAY;delay++) { ; }
  }
}