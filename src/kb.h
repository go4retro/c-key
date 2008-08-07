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
#ifndef KB_H
#define KB_H 1

#define KB_ST_PREP            1
#define KB_ST_READ            2
#define KB_ST_READ_PORTS      3

#define KB_KEY_UP             0x80

#define KB_NO_REPEAT          0xff

#define KB_RX_BUFFER_SIZE     16     /* 2,4,8,16,32,64,128 or 256 bytes */
#define KB_RX_BUFFER_MASK     (KB_RX_BUFFER_SIZE - 1)
#if (KB_RX_BUFFER_SIZE & KB_RX_BUFFER_MASK)
#  error KB RX buffer size is not a power of 2
#endif

void kb_init(void);
void kb_set_repeat_delay(uint16_t ms);
void kb_set_repeat_period(uint16_t period);
void kb_set_repeat_code(uint8_t code);
uint8_t kb_get_repeat_code(void);
uint8_t kb_data_available( void );
uint8_t kb_recv( void );
void kb_scan(void);

#endif

