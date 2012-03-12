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

    ps2_kb.c: Convenience functions for PS/2 Keyboards

*/

#include <inttypes.h>
#include <avr/interrupt.h>
#include "config.h"
#include "ps2.h"
#include "ps2_int.h"

static uint8_t ps2_leds = 0;
static uint8_t ps2_codeset = 2;


void ps2_handle_cmds(uint8_t data) {
  uint8_t i;

    switch(data) {
      case PS2_CMD_ACK:
        //ignore.
        break;
      case PS2_CMD_RESET:
        ps2_putc(PS2_CMD_ACK);
        ps2_putc(PS2_CMD_BAT);
        break;
      case PS2_CMD_DISABLE:
        // we should disable sending output if we receive this command.
      case PS2_CMD_ENABLE:
        //clear out KB buffers
        cli();
        ps2_clear_buffers();
        sei();
        ps2_putc(PS2_CMD_ACK);
        break;
      default:
        ps2_putc(PS2_CMD_ACK);
        break;
      case PS2_CMD_ECHO:
        ps2_putc(PS2_CMD_ECHO);
        break;
      case PS2_CMD_SET_CODE_SET:
        ps2_putc(PS2_CMD_ACK);
        i = ps2_getc();
        if(i == 0) {
          ps2_putc(ps2_codeset);
        } else {
          ps2_codeset = i;
        }
        break;
      case PS2_CMD_SET_RATE:
        // this should to be caught in another area, ignore if received here.
        break;
      case PS2_CMD_READ_ID:
        ps2_putc(PS2_CMD_ACK);
        ps2_putc(0xab);
        ps2_putc(0x83);
        break;
      case PS2_CMD_LEDS:
        ps2_putc(PS2_CMD_ACK);
        ps2_leds = ps2_getc()&0x07;
        ps2_putc(PS2_CMD_ACK);
        break;
      case PS2_CMD_RESEND:
        break;
    }
}


/* this returns the initial delay in ms */
uint16_t ps2_get_typematic_delay(uint8_t rate) {
  return (((rate & 0x60) >> 5) + 1) * 250;
}


/* this returns the rate in CPS */
uint16_t ps2_get_typematic_period(uint8_t rate) {
  return ((8 + (rate & 0x07)) * (1 << ((rate & 0x18) >> 3)) << 2);
}
