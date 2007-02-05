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
#include "poll.h"
#include <avr/io.h>
#include "kb.h"
#include "ps2.h"

#define FALSE 0
#define TRUE 1

void poll(void) {
  unsigned char ext=00;
  unsigned char brk=FALSE;
  unsigned char key;
  
  unsigned char shift=FALSE;
  unsigned char cmdr=FALSE;
  unsigned char ctrl=FALSE;

  unsigned char row_low_save=0xff;
  unsigned char row_high_save=0xff;
  
  unsigned char out;
  
  unsigned char array[16]={0};
  
  // put all pins in open collector state with pullups.
  KB_PORT_ROW_LOW_OUT=0xff;
  KB_DDR_ROW_LOW=0;
  KB_PORT_ROW_HIGH_OUT=0xff;
  KB_DDR_ROW_HIGH=0;
  KB_PORT_COL_OUT=0xff;
  KB_DDR_COL=0;
  // loop checking...
  // need to check every 15 cycles...
  for(;;) {
    if(KB_PORT_COL_IN != row_low_save) {
      // do stuff
      out=0x00;
      if(KB_PORT_COL_IN & 0x80)
        out|=array[0];
      if(KB_PORT_COL_IN & 0x40)
        out|=array[1];
      if(KB_PORT_COL_IN & 0x20)
        out|=array[2];
      if(KB_PORT_COL_IN & 0x10)
        out|=array[3];
      if(KB_PORT_COL_IN & 0x08)
        out|=array[4];
      if(KB_PORT_COL_IN & 0x04)
        out|=array[5];
      if(KB_PORT_COL_IN & 0x02)
        out|=array[6];
      if(KB_PORT_COL_IN & 0x01)
        out|=array[7];
      // all on bits need to be inverted.
      // set columns.
      KB_PORT_ROW_LOW_OUT=~out;
      KB_DDR_ROW_LOW=out;
      // save for next time.
      row_low_save=KB_PORT_ROW_LOW_IN;
    } 
    if(PS2_data_available() != 0) {
      // kb sent data...
      key=PS2_recv();
      if(key == 0xe0 && ext == 0) {
        // turn ext on;
        ext=0xe0;
      } else if(key == 0xe1 && ext == 0) {
        // for pause key;
        ext=0xe1;
      } else if(key == 0xf0) {
        // key up
        brk=TRUE;
      } else {
        // regular key
        if(brk == TRUE) {
          array[1]=0;
        } else {
          array[1]=8;
        }
        // reset flags
        ext=0;
        brk=FALSE;
      }
    }
  }
}