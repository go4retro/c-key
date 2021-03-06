/*
    C=Key - Commodore <-> PS/2 Interface
    Copyright Jim Brain and RETRO Innovations, 2004-2011

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

    poll64.h: Definitions for PS/2 Keyboard to CBM computer scanning routine

*/

#ifndef _POLL64_H
#define _POLL64_H

#define POLL_IRQ_DIVIDER (240)
#define POLL_LED_IRQ_DIVIDER (F_CPU/1024/2/POLL_IRQ_DIVIDER/3)   // 3/s

#define POLL_ADDR_LAYOUT         (void*)0
#define POLL_ADDR_LED_STATE      (void*)1
#define POLL_ADDR_SW_STATE       (void*)2

#define POLL_CBM_KEY_NONE        0xff
#define POLL_CBM_KEY_UNMAPPED    0xfe
#define POLL_CBM_KEY_SPECIAL     0xc0

#define POLL_MAP_NONE       0
#define POLL_MAP_SHIFT      1

#define POLL_LAYOUT_POSITIONAL_C64  0
#define POLL_LAYOUT_POSITIONAL_C128 1
#define POLL_LAYOUT_SYMBOLIC_C64    2
#define POLL_LAYOUT_SYMBOLIC_C128   3
#define POLL_LAYOUT_NUM             4

#define POLL_ST_IDLE          0
#define POLL_ST_GET_X_KEY     1
#define POLL_ST_GET_KEY_UP    2
#define POLL_ST_GET_X_KEY_UP  3
#define POLL_ST_GET_PAUSE_1   4
#define POLL_ST_GET_PAUSE_2   5
#define POLL_ST_GET_PAUSE_3   6
#define POLL_ST_GET_PAUSE_4   7
#define POLL_ST_GET_PAUSE_5   8
#define POLL_ST_GET_PAUSE_6   9
#define POLL_ST_GET_PAUSE_7   10

#define POLL_C64_KEY_RUNSTOP     0x7A
#define POLL_C64_KEY_LSHIFT      0x72
#define POLL_C64_KEY_X           0x7E
#define POLL_C64_KEY_V           0x76  
#define POLL_C64_KEY_N           0xBE
#define POLL_C64_KEY_COMMA       0xB6
#define POLL_C64_KEY_SLASH       0x3E
#define POLL_C64_KEY_CRSR_DOWN   0x36
#define POLL_C64_KEY_CTRL        0x5A
#define POLL_C64_KEY_A           0x52
#define POLL_C64_KEY_D           0x5E
#define POLL_C64_KEY_G           0x56
#define POLL_C64_KEY_J           0x9E
#define POLL_C64_KEY_L           0x96
#define POLL_C64_KEY_SEMICOLON   0x1E
#define POLL_C64_KEY_CRSR_RIGHT  0x16
#define POLL_C64_KEY_BACKARROW   0x6A
#define POLL_C64_KEY_W           0x62
#define POLL_C64_KEY_R           0x6E
#define POLL_C64_KEY_Y           0x66
#define POLL_C64_KEY_I           0xAE
#define POLL_C64_KEY_P           0xA6
#define POLL_C64_KEY_ASTERIX     0x2E
#define POLL_C64_KEY_RETURN      0x26
#define POLL_C64_KEY_1           0x4A
#define POLL_C64_KEY_3           0x42
#define POLL_C64_KEY_5           0x4E
#define POLL_C64_KEY_7           0x46
#define POLL_C64_KEY_9           0x8E
#define POLL_C64_KEY_PLUS        0x86
#define POLL_C64_KEY_POUND       0x0E
#define POLL_C64_KEY_DELETE      0x06
#define POLL_C64_KEY_2           0x78
#define POLL_C64_KEY_4           0x70
#define POLL_C64_KEY_6           0x7C
#define POLL_C64_KEY_8           0x74
#define POLL_C64_KEY_0           0xBC
#define POLL_C64_KEY_MINUS       0xB4
#define POLL_C64_KEY_HOME        0x3C
#define POLL_C64_KEY_F7          0x34
#define POLL_C64_KEY_Q           0x58
#define POLL_C64_KEY_E           0x50
#define POLL_C64_KEY_T           0x5C
#define POLL_C64_KEY_U           0x54
#define POLL_C64_KEY_O           0x9C
#define POLL_C64_KEY_AT          0x94
#define POLL_C64_KEY_UPARROW     0x1C
#define POLL_C64_KEY_F5          0x14
#define POLL_C64_KEY_CBM         0x68
#define POLL_C64_KEY_S           0x60
#define POLL_C64_KEY_F           0x6C
#define POLL_C64_KEY_H           0x64
#define POLL_C64_KEY_K           0xAC
#define POLL_C64_KEY_COLON       0xA4
#define POLL_C64_KEY_EQUALS      0x2C
#define POLL_C64_KEY_F3          0x24
#define POLL_C64_KEY_SPACE       0x48
#define POLL_C64_KEY_Z           0x40
#define POLL_C64_KEY_C           0x4C
#define POLL_C64_KEY_B           0x44
#define POLL_C64_KEY_M           0x8C
#define POLL_C64_KEY_PERIOD      0x84
#define POLL_C64_KEY_RSHIFT      0x0C
#define POLL_C64_KEY_F1          0x04

#define POLL_C128_KEY_NO_SCROLL  0x3A
#define POLL_C128_KEY_NUM_3      0xB2
#define POLL_C128_KEY_NUM_1      0xBA
#define POLL_C128_KEY_NUM_PERIOD 0x1A
#define POLL_C128_KEY_NUM_MINUS  0x92
#define POLL_C128_KEY_NUM_5      0x9A
#define POLL_C128_KEY_NUM_0      0x2A
#define POLL_C128_KEY_NUM_PLUS   0xA2
#define POLL_C128_KEY_NUM_8      0xAA
#define POLL_C128_KEY_CRSR_UP    0x38
#define POLL_C128_KEY_LINEFEED   0xB0
#define POLL_C128_KEY_TAB        0xB8
#define POLL_C128_KEY_CRSR_RIGHT 0x18
#define POLL_C128_KEY_NUM_9      0x90
#define POLL_C128_KEY_NUM_7      0x98
#define POLL_C128_KEY_CRSR_LEFT  0x28
#define POLL_C128_KEY_NUM_6      0xA0
#define POLL_C128_KEY_NUM_4      0xA8
#define POLL_C128_KEY_CRSR_DOWN  0x08
#define POLL_C128_KEY_ENTER      0x80
#define POLL_C128_KEY_NUM_2      0x88
#define POLL_C128_KEY_ALT        0x0A
#define POLL_C128_KEY_ESC        0x82
#define POLL_C128_KEY_HELP       0x8A

// pseudo keys:

#define POLL_C128_PKEY_4080      0x00
#define POLL_C128_PKEY_CAPSENSE  0x10
#define POLL_C64_PKEY_RESTORE    0x20

#define POLL_FLAG_LSHIFT 1
#define POLL_FLAG_RSHIFT 2
#define POLL_FLAG_SHIFT (POLL_FLAG_LSHIFT | POLL_FLAG_RSHIFT)
#define POLL_FLAG_ALT    4
#define POLL_FLAG_CONTROL 8
#define POLL_FLAG_CTRL_ALT     (POLL_FLAG_CONTROL | POLL_FLAG_ALT)


#endif //_POLL64_H_
