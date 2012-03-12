/*
    C=Key - Commodore <-> PS/2 interface
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

    eeprom.c: Persistent configuration storage

*/

#include <avr/eeprom.h>
#include <avr/io.h>
#include "config.h"
#include "eeprom.h"

void update_eeprom(void* address,uint8_t data) {
  uint8_t tmp;
  
  while(!eeprom_is_ready());
  tmp=eeprom_read_byte(address);
  if(tmp!=data)
  while(!eeprom_is_ready());
  eeprom_write_byte(address,data);
}

