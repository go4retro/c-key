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
#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "ps2.h"
#include "util.h"
#include "switches.h"
#include "poll.h"
#include "poll64.h"
#include "led.h"

static prog_uint8_t normal[0x84] =  { C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_SPECIAL+0,C64_KEY_SPECIAL+1,C64_KEY_SPECIAL+2,C64_KEY_SPECIAL+3,C64_KEY_UNMAPPED
                                  ,C64_KEY_NONE,C64_KEY_UNMAPPED,C64_KEY_SPECIAL+4,C64_KEY_SPECIAL+5,C64_KEY_SPECIAL+6,C64_KEY_SPECIAL+40,C64_KEY_BACKARROW,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_SPECIAL+8,C64_KEY_SPECIAL+9,C64_KEY_NONE,C64_KEY_SPECIAL+10,C64_KEY_Q,C64_KEY_1,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_Z,C64_KEY_S,C64_KEY_A,C64_KEY_W,C64_KEY_SPECIAL+11,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_C,C64_KEY_X,C64_KEY_D,C64_KEY_E,C64_KEY_4,C64_KEY_3,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_SPACE,C64_KEY_V,C64_KEY_F,C64_KEY_T,C64_KEY_R,C64_KEY_5,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_N,C64_KEY_B,C64_KEY_H,C64_KEY_G,C64_KEY_Y,C64_KEY_SPECIAL+12,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_M,C64_KEY_J,C64_KEY_U,C64_KEY_SPECIAL+13,C64_KEY_SPECIAL+14,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_COMMA,C64_KEY_K,C64_KEY_I,C64_KEY_O,C64_KEY_SPECIAL+15,C64_KEY_SPECIAL+16,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_PERIOD,C64_KEY_SLASH,C64_KEY_L,C64_KEY_SPECIAL+17,C64_KEY_P,C64_KEY_SPECIAL+18,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_SPECIAL+19,C64_KEY_NONE,C64_KEY_SPECIAL+20,C64_KEY_SPECIAL+21,C64_KEY_NONE,C64_KEY_NONE
                                  ,C64_KEY_UNMAPPED,C64_KEY_RSHIFT,C64_KEY_RETURN,C64_KEY_SPECIAL+22,C64_KEY_NONE,C64_KEY_SPECIAL+23,C64_KEY_NONE,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_DELETE,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_SPECIAL+24,C64_KEY_NONE,C64_KEY_SPECIAL+25,C64_KEY_SPECIAL+26,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE
                                  ,C64_KEY_SPECIAL+27,C64_KEY_SPECIAL+28,C64_KEY_SPECIAL+29,C64_KEY_SPECIAL+30,C64_KEY_SPECIAL+31,C64_KEY_SPECIAL+32,C64_KEY_SPECIAL+33,C64_KEY_NONE
                                  ,C64_KEY_UNMAPPED,C64_KEY_SPECIAL+34,C64_KEY_SPECIAL+35,C64_KEY_SPECIAL+36,C64_KEY_SPECIAL+37,C64_KEY_SPECIAL+38,C64_KEY_SPECIAL+39,C64_KEY_NONE
                                  ,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_NONE,C64_KEY_SPECIAL+7};
                                  
static prog_uint8_t layouts[4][41][2] = {
                                          // c64 positional mapping.
                                             {
                                              {POLL_MAP_NONE | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 0 F5
                                              {POLL_MAP_NONE | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 1 F3
                                              {POLL_MAP_NONE | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 2 F1
                                              {POLL_MAP_SHIFT | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 3 F2
                                              {POLL_MAP_SHIFT | C64_KEY_F7, POLL_MAP_SHIFT | C64_KEY_F7},  // 4 - F8
                                              {POLL_MAP_SHIFT | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 5 - F6
                                              {POLL_MAP_SHIFT | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 6 - F4
                                              {POLL_MAP_NONE | C64_KEY_F7,POLL_MAP_SHIFT | C64_KEY_F7},  // 7 - F7
                                              {POLL_MAP_NONE | C64_KEY_CBM,POLL_MAP_SHIFT | C64_KEY_CBM},  // 8 - Left ALT
                                              {POLL_MAP_NONE | C64_KEY_LSHIFT,POLL_MAP_SHIFT | C64_KEY_LSHIFT},  // 9 - Left Shift
                                              {POLL_MAP_NONE | C64_KEY_CTRL,POLL_MAP_SHIFT | C64_KEY_CTRL},  // 10 Left Control
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_AT},  // 11 2
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_UPARROW},  // 12 6
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_6},  // 13 7
                                              {POLL_MAP_NONE | C64_KEY_8, POLL_MAP_NONE | C64_KEY_ASTERIX},  // 14 - 8
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_SHIFT | C64_KEY_9},  // 15 - 0
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_SHIFT | C64_KEY_8},  // 16 - 9
                                              {POLL_MAP_NONE | C64_KEY_SEMICOLON,POLL_MAP_NONE | C64_KEY_COLON},  // 17 - Semicolon
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_SHIFT | C64_KEY_MINUS},  // 18 - Minus
                                              {POLL_MAP_SHIFT | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_2},  // 19 - apostrophe
                                              {POLL_MAP_SHIFT | C64_KEY_COLON,POLL_MAP_SHIFT | C64_KEY_AT},  // 20 Left Bracket, add shift amp to end
                                              {POLL_MAP_NONE | C64_KEY_EQUALS,POLL_MAP_NONE | C64_KEY_PLUS},  // 21 Equals
                                              {POLL_MAP_SHIFT | C64_KEY_SEMICOLON,POLL_MAP_SHIFT | C64_KEY_ASTERIX},  // 22 Right Bracket, add shift * to end
                                              {POLL_MAP_NONE | C64_KEY_POUND,POLL_MAP_SHIFT | C64_KEY_POUND},  // 23 Pipe
                                              {POLL_MAP_NONE | C64_KEY_1, POLL_MAP_NONE | C64_KEY_1},  // 24 - Num 1
                                              {POLL_MAP_NONE | C64_KEY_4,POLL_MAP_NONE | C64_KEY_4},  // 25 - Num 4
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_NONE | C64_KEY_7},  // 26 - Num 7
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_NONE | C64_KEY_0},  // 27 - Num 0
                                              {POLL_MAP_NONE | C64_KEY_PERIOD,POLL_MAP_NONE | C64_KEY_PERIOD},  // 28 - Num Dot
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_2},  // 29 - Num 2
                                              {POLL_MAP_NONE | C64_KEY_5,POLL_MAP_NONE | C64_KEY_5},  // 30 Num 5
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_6},  // 31 Num 6
                                              {POLL_MAP_NONE | C64_KEY_8,POLL_MAP_NONE | C64_KEY_8},  // 32 Num 8
                                              {POLL_MAP_NONE | C64_KEY_RUNSTOP,POLL_MAP_SHIFT | C64_KEY_RUNSTOP},  // 33 Esc
                                              {POLL_MAP_NONE | C64_KEY_PLUS, POLL_MAP_NONE | C64_KEY_PLUS},  // 34 - Num +
                                              {POLL_MAP_NONE | C64_KEY_3,POLL_MAP_NONE | C64_KEY_3},  // 35 - Num 3
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_NONE | C64_KEY_MINUS},  // 36 - Num Minus
                                              {POLL_MAP_NONE | C64_KEY_ASTERIX,POLL_MAP_NONE | C64_KEY_ASTERIX},  // 37 - Num Asterix
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_NONE | C64_KEY_9},  // 38 - Num 9
                                              {POLL_MAP_NONE | C64_KEY_NONE,POLL_MAP_SHIFT | C64_KEY_NONE},  // 39 - Scroll Lock
                                              {POLL_MAP_NONE | C64_KEY_UNMAPPED,POLL_MAP_SHIFT | C64_KEY_UNMAPPED},  // 40 - TAB
                                             }
                                             ,
                                             {
                                              // c128 positional
                                              {POLL_MAP_NONE | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 0 F5
                                              {POLL_MAP_NONE | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 1 F3
                                              {POLL_MAP_NONE | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 2 F1
                                              {POLL_MAP_SHIFT | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 3 F2
                                              {POLL_MAP_SHIFT | C64_KEY_F7, POLL_MAP_SHIFT | C64_KEY_F7},  // 4 - F8
                                              {POLL_MAP_SHIFT | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 5 - F6
                                              {POLL_MAP_SHIFT | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 6 - F4
                                              {POLL_MAP_NONE | C64_KEY_F7,POLL_MAP_SHIFT | C64_KEY_F7},  // 7 - F7
                                              {POLL_MAP_NONE | C64_KEY_CBM,POLL_MAP_SHIFT | C64_KEY_CBM},  // 8 - Left ALT
                                              {POLL_MAP_NONE | C64_KEY_LSHIFT,POLL_MAP_SHIFT | C64_KEY_LSHIFT},  // 9 - Left Shift
                                              {POLL_MAP_NONE | C64_KEY_CTRL,POLL_MAP_SHIFT | C64_KEY_CTRL},  // 10 Left Control
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_AT},  // 11 2
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_UPARROW},  // 12 6
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_6},  // 13 7
                                              {POLL_MAP_NONE | C64_KEY_8, POLL_MAP_NONE | C64_KEY_ASTERIX},  // 14 - 8
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_SHIFT | C64_KEY_9},  // 15 - 0
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_SHIFT | C64_KEY_8},  // 16 - 9
                                              {POLL_MAP_NONE | C64_KEY_SEMICOLON,POLL_MAP_NONE | C64_KEY_COLON},  // 17 - Semicolon
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_SHIFT | C64_KEY_MINUS},  // 18 - Minus
                                              {POLL_MAP_SHIFT | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_2},  // 19 - apostrophe
                                              {POLL_MAP_SHIFT | C64_KEY_COLON,POLL_MAP_SHIFT | C64_KEY_AT},  // 20 Left Bracket, add shift amp to end
                                              {POLL_MAP_NONE | C64_KEY_EQUALS,POLL_MAP_NONE | C64_KEY_PLUS},  // 21 Equals
                                              {POLL_MAP_SHIFT | C64_KEY_SEMICOLON,POLL_MAP_SHIFT | C64_KEY_ASTERIX},  // 22 Right Bracket, add shift * to end
                                              {POLL_MAP_NONE | C64_KEY_POUND,POLL_MAP_SHIFT | C64_KEY_POUND},  // 23 Pipe
                                              {POLL_MAP_NONE | C128_KEY_NUM_1, POLL_MAP_SHIFT | C128_KEY_NUM_1},  // 24 - Num 1*
                                              {POLL_MAP_NONE | C128_KEY_NUM_4,POLL_MAP_SHIFT | C128_KEY_NUM_4},  // 25 - Num 4*
                                              {POLL_MAP_NONE | C128_KEY_NUM_7,POLL_MAP_SHIFT | C128_KEY_NUM_7},  // 26 - Num 7*
                                              {POLL_MAP_NONE | C128_KEY_NUM_0,POLL_MAP_SHIFT | C128_KEY_NUM_0},  // 27 - Num 0*
                                              {POLL_MAP_NONE | C128_KEY_NUM_PERIOD,POLL_MAP_SHIFT | C128_KEY_NUM_PERIOD},  // 28 - Num Dot*
                                              {POLL_MAP_NONE | C128_KEY_NUM_2,POLL_MAP_SHIFT | C128_KEY_NUM_2},  // 29 - Num 2*
                                              {POLL_MAP_NONE | C128_KEY_NUM_5,POLL_MAP_SHIFT | C128_KEY_NUM_5},  // 30 Num 5*
                                              {POLL_MAP_NONE | C128_KEY_NUM_6,POLL_MAP_SHIFT | C128_KEY_NUM_6},  // 31 Num 6*
                                              {POLL_MAP_NONE | C128_KEY_NUM_8,POLL_MAP_SHIFT | C128_KEY_NUM_8},  // 32 Num 8*
                                              {POLL_MAP_NONE | C128_KEY_ESC,POLL_MAP_SHIFT | C128_KEY_ESC},  // 33 Esc*
                                              {POLL_MAP_NONE | C128_KEY_NUM_PLUS, POLL_MAP_SHIFT | C128_KEY_NUM_PLUS},  // 34 - Num +*
                                              {POLL_MAP_NONE | C128_KEY_NUM_3,POLL_MAP_SHIFT | C128_KEY_NUM_3},  // 35 - Num 3*
                                              {POLL_MAP_NONE | C128_KEY_NUM_MINUS,POLL_MAP_SHIFT | C128_KEY_NUM_MINUS},  // 36 - Num Minus*
                                              {POLL_MAP_NONE | C64_KEY_ASTERIX,POLL_MAP_NONE | C64_KEY_ASTERIX},  // 37 - Num Asterix
                                              {POLL_MAP_NONE | C128_KEY_NUM_9,POLL_MAP_SHIFT | C128_KEY_NUM_9},  // 38 - Num 9*
                                              {POLL_MAP_NONE | C128_KEY_NOSCROLL,POLL_MAP_NONE | C128_KEY_LINEFEED},  // 39 - Scroll Lock*
                                              {POLL_MAP_NONE | C128_KEY_TAB,POLL_MAP_SHIFT | C128_KEY_TAB},  // 40 - TAB*
                                             }
                                             ,
                                             {
                                              {POLL_MAP_NONE | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 0 F5
                                              {POLL_MAP_NONE | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 1 F3
                                              {POLL_MAP_NONE | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 2 F1
                                              {POLL_MAP_SHIFT | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 3 F2
                                              {POLL_MAP_SHIFT | C64_KEY_F7, POLL_MAP_SHIFT | C64_KEY_F7},  // 4 - F8
                                              {POLL_MAP_SHIFT | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 5 - F6
                                              {POLL_MAP_SHIFT | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 6 - F4
                                              {POLL_MAP_NONE | C64_KEY_F7,POLL_MAP_SHIFT | C64_KEY_F7},  // 7 - F7
                                              {POLL_MAP_NONE | C64_KEY_CBM,POLL_MAP_SHIFT | C64_KEY_CBM},  // 8 - Left ALT
                                              {POLL_MAP_NONE | C64_KEY_LSHIFT,POLL_MAP_SHIFT | C64_KEY_LSHIFT},  // 9 - Left Shift
                                              {POLL_MAP_NONE | C64_KEY_CTRL,POLL_MAP_SHIFT | C64_KEY_CTRL},  // 10 Left Control
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_AT},  // 11 2
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_UPARROW},  // 12 6
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_6},  // 13 7
                                              {POLL_MAP_NONE | C64_KEY_8, POLL_MAP_NONE | C64_KEY_ASTERIX},  // 14 - 8
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_SHIFT | C64_KEY_9},  // 15 - 0
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_SHIFT | C64_KEY_8},  // 16 - 9
                                              {POLL_MAP_NONE | C64_KEY_SEMICOLON,POLL_MAP_NONE | C64_KEY_COLON},  // 17 - Semicolon
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_SHIFT | C64_KEY_MINUS},  // 18 - Minus
                                              {POLL_MAP_SHIFT | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_2},  // 19 - apostrophe
                                              {POLL_MAP_SHIFT | C64_KEY_COLON,POLL_MAP_SHIFT | C64_KEY_AT},  // 20 Left Bracket, add shift amp to end
                                              {POLL_MAP_NONE | C64_KEY_EQUALS,POLL_MAP_NONE | C64_KEY_PLUS},  // 21 Equals
                                              {POLL_MAP_SHIFT | C64_KEY_SEMICOLON,POLL_MAP_SHIFT | C64_KEY_ASTERIX},  // 22 Right Bracket, add shift * to end
                                              {POLL_MAP_NONE | C64_KEY_POUND,POLL_MAP_SHIFT | C64_KEY_POUND},  // 23 Pipe
                                              {POLL_MAP_NONE | C64_KEY_1, POLL_MAP_NONE | C64_KEY_1},  // 24 - Num 1
                                              {POLL_MAP_NONE | C64_KEY_4,POLL_MAP_NONE | C64_KEY_4},  // 25 - Num 4
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_NONE | C64_KEY_7},  // 26 - Num 7
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_NONE | C64_KEY_0},  // 27 - Num 0
                                              {POLL_MAP_NONE | C64_KEY_PERIOD,POLL_MAP_NONE | C64_KEY_PERIOD},  // 28 - Num Dot
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_2},  // 29 - Num 2
                                              {POLL_MAP_NONE | C64_KEY_5,POLL_MAP_NONE | C64_KEY_5},  // 30 Num 5
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_6},  // 31 Num 6
                                              {POLL_MAP_NONE | C64_KEY_8,POLL_MAP_NONE | C64_KEY_8},  // 32 Num 8
                                              {POLL_MAP_NONE | C64_KEY_RUNSTOP,POLL_MAP_SHIFT | C64_KEY_RUNSTOP},  // 33 Esc
                                              {POLL_MAP_NONE | C64_KEY_PLUS, POLL_MAP_NONE | C64_KEY_PLUS},  // 34 - Num +
                                              {POLL_MAP_NONE | C64_KEY_3,POLL_MAP_NONE | C64_KEY_3},  // 35 - Num 3
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_NONE | C64_KEY_MINUS},  // 36 - Num Minus
                                              {POLL_MAP_NONE | C64_KEY_ASTERIX,POLL_MAP_NONE | C64_KEY_ASTERIX},  // 37 - Num Asterix
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_NONE | C64_KEY_9},  // 38 - Num 9
                                              {POLL_MAP_NONE | C64_KEY_NONE,POLL_MAP_SHIFT | C64_KEY_NONE},  // 39 - Scroll Lock
                                              {POLL_MAP_NONE | C64_KEY_UNMAPPED,POLL_MAP_SHIFT | C64_KEY_UNMAPPED},  // 40 - TAB
                                             }
                                             ,
                                             {
                                              {POLL_MAP_NONE | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 0 F5
                                              {POLL_MAP_NONE | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 1 F3
                                              {POLL_MAP_NONE | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 2 F1
                                              {POLL_MAP_SHIFT | C64_KEY_F1,POLL_MAP_SHIFT | C64_KEY_F1},  // 3 F2
                                              {POLL_MAP_SHIFT | C64_KEY_F7, POLL_MAP_SHIFT | C64_KEY_F7},  // 4 - F8
                                              {POLL_MAP_SHIFT | C64_KEY_F5,POLL_MAP_SHIFT | C64_KEY_F5},  // 5 - F6
                                              {POLL_MAP_SHIFT | C64_KEY_F3,POLL_MAP_SHIFT | C64_KEY_F3},  // 6 - F4
                                              {POLL_MAP_NONE | C64_KEY_F7,POLL_MAP_SHIFT | C64_KEY_F7},  // 7 - F7
                                              {POLL_MAP_NONE | C64_KEY_CBM,POLL_MAP_SHIFT | C64_KEY_CBM},  // 8 - Left ALT
                                              {POLL_MAP_NONE | C64_KEY_LSHIFT,POLL_MAP_SHIFT | C64_KEY_LSHIFT},  // 9 - Left Shift
                                              {POLL_MAP_NONE | C64_KEY_CTRL,POLL_MAP_SHIFT | C64_KEY_CTRL},  // 10 Left Control
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_AT},  // 11 2
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_UPARROW},  // 12 6
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_6},  // 13 7
                                              {POLL_MAP_NONE | C64_KEY_8, POLL_MAP_NONE | C64_KEY_ASTERIX},  // 14 - 8
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_SHIFT | C64_KEY_9},  // 15 - 0
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_SHIFT | C64_KEY_8},  // 16 - 9
                                              {POLL_MAP_NONE | C64_KEY_SEMICOLON,POLL_MAP_NONE | C64_KEY_COLON},  // 17 - Semicolon
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_SHIFT | C64_KEY_MINUS},  // 18 - Minus
                                              {POLL_MAP_SHIFT | C64_KEY_7,POLL_MAP_SHIFT | C64_KEY_2},  // 19 - apostrophe
                                              {POLL_MAP_SHIFT | C64_KEY_COLON,POLL_MAP_SHIFT | C64_KEY_AT},  // 20 Left Bracket, add shift amp to end
                                              {POLL_MAP_NONE | C64_KEY_EQUALS,POLL_MAP_NONE | C64_KEY_PLUS},  // 21 Equals
                                              {POLL_MAP_SHIFT | C64_KEY_SEMICOLON,POLL_MAP_SHIFT | C64_KEY_ASTERIX},  // 22 Right Bracket, add shift * to end
                                              {POLL_MAP_NONE | C64_KEY_POUND,POLL_MAP_SHIFT | C64_KEY_POUND},  // 23 Pipe
                                              {POLL_MAP_NONE | C64_KEY_1, POLL_MAP_NONE | C64_KEY_1},  // 24 - Num 1
                                              {POLL_MAP_NONE | C64_KEY_4,POLL_MAP_NONE | C64_KEY_4},  // 25 - Num 4
                                              {POLL_MAP_NONE | C64_KEY_7,POLL_MAP_NONE | C64_KEY_7},  // 26 - Num 7
                                              {POLL_MAP_NONE | C64_KEY_0,POLL_MAP_NONE | C64_KEY_0},  // 27 - Num 0
                                              {POLL_MAP_NONE | C64_KEY_PERIOD,POLL_MAP_NONE | C64_KEY_PERIOD},  // 28 - Num Dot
                                              {POLL_MAP_NONE | C64_KEY_2,POLL_MAP_NONE | C64_KEY_2},  // 29 - Num 2
                                              {POLL_MAP_NONE | C64_KEY_5,POLL_MAP_NONE | C64_KEY_5},  // 30 Num 5
                                              {POLL_MAP_NONE | C64_KEY_6,POLL_MAP_NONE | C64_KEY_6},  // 31 Num 6
                                              {POLL_MAP_NONE | C64_KEY_8,POLL_MAP_NONE | C64_KEY_8},  // 32 Num 8
                                              {POLL_MAP_NONE | C64_KEY_RUNSTOP,POLL_MAP_SHIFT | C64_KEY_RUNSTOP},  // 33 Esc
                                              {POLL_MAP_NONE | C64_KEY_PLUS, POLL_MAP_NONE | C64_KEY_PLUS},  // 34 - Num +
                                              {POLL_MAP_NONE | C64_KEY_3,POLL_MAP_NONE | C64_KEY_3},  // 35 - Num 3
                                              {POLL_MAP_NONE | C64_KEY_MINUS,POLL_MAP_NONE | C64_KEY_MINUS},  // 36 - Num Minus
                                              {POLL_MAP_NONE | C64_KEY_ASTERIX,POLL_MAP_NONE | C64_KEY_ASTERIX},  // 37 - Num Asterix
                                              {POLL_MAP_NONE | C64_KEY_9,POLL_MAP_NONE | C64_KEY_9},  // 38 - Num 9
                                              {POLL_MAP_NONE | C64_KEY_NONE,POLL_MAP_SHIFT | C64_KEY_NONE},  // 39 - Scroll Lock
                                              {POLL_MAP_NONE | C64_KEY_UNMAPPED,POLL_MAP_SHIFT | C64_KEY_UNMAPPED},  // 40 - TAB
                                             }
                                          };
                                          

static uint8_t layout;

static uint8_t meta;
static uint8_t shift_override_key;
static uint8_t shift_override;
static uint8_t shift_override_state;

static uint8_t led_state=0;

static uint8_t state=POLL_ST_IDLE;

inline void delay_jiffy(void) {
  // 1/60 second delay for funky shifting settling.
  TCNT2=0;
  // Set OC2 clk  to SYSCLK/256 and Compare Timer Mode
  TCCR2 = (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21);
  while(!(TIFR & (1<<OCF2)));
  TIFR=(1<<OCF2);
  TCCR2=0;
}

void _set_switch(uint8_t sw, uint8_t state) {
  switch(sw) {
    case C64_PKEY_RESTORE:
      SW_send(SW_RESTORE | (state?SW_UP:0));
      break;
    case C128_PKEY_4080:
      SW_send(SW_4080 | (state?SW_UP:0));
      break;
    case C128_PKEY_CAPSENSE:
      SW_send(SW_CAPSENSE | (state?SW_UP:0));
      break;
    default:
    //if(state)
    //debug2('+');
    //else
    //debug2('-');
    //printHex(sw);
    XPT_PORT_DATA_OUT=((state!=FALSE) | sw);
    // strobe STROBE PIN
    XPT_PORT_STROBE_OUT|=XPT_PIN_STROBE;
    // bring low
    XPT_PORT_STROBE_OUT&=(uint8_t)~XPT_PIN_STROBE;
    break;
  }
}

void set_switch(uint8_t sw, uint8_t state) {
  if(state)
    debug2('d');
  else
    debug2('u');
  printHex(sw);
  _set_switch(sw,state);
}


inline void reset_shift_override(uint8_t sw, uint8_t brk) {
  // if it is not a repeat, not a meta key, then reset.
  if(shift_override 
     && ((sw != C64_KEY_LSHIFT  // if it is a shift, keep override
          && sw != C64_KEY_RSHIFT
          && sw != C64_KEY_CTRL    // if it is ctrl or cbm, keep.
          && sw != C64_KEY_CBM
          && sw != shift_override_key // if it is a repeat, keep
         )
         || (sw == shift_override_key && brk)
        )
     ) {
    //debug2('o');
    // turn off key.
    set_switch(shift_override_key,FALSE);
    set_switch(C64_KEY_LSHIFT,meta & POLL_FLAG_LSHIFT);
    set_switch(C64_KEY_RSHIFT,meta & POLL_FLAG_RSHIFT);
    shift_override=FALSE;
  }
}

void set_matrix(uint8_t sh, uint8_t sw, uint8_t brk) {
  // sh is intended shift state.
  // sw is matrix code
  // brk is TRUE for key up, false for key down.
  
  // reset override, if applicable.
  reset_shift_override(sw,brk);
  if(sw==C64_KEY_LSHIFT) {
    //if(brk)
    //  shift&=(uint8_t)~POLL_FLAG_LSHIFT;
    //else
    //  shift|=(uint8_t)POLL_FLAG_LSHIFT;
    meta=(meta&(uint8_t)~POLL_FLAG_LSHIFT) | (brk?0:POLL_FLAG_LSHIFT);
    set_switch(C64_KEY_LSHIFT,!brk);
  } else if(sw==C64_KEY_RSHIFT) {
    //if(brk)
    //  shift&=(uint8_t)~POLL_FLAG_RSHIFT;
    //else
    //  shift|=(uint8_t)POLL_FLAG_RSHIFT;
    meta=(meta&(uint8_t)~POLL_FLAG_RSHIFT) | (brk?0:POLL_FLAG_RSHIFT);
    set_switch(C64_KEY_RSHIFT,!brk);
  } else if(!brk) {
    // key down
    if(!(shift_override && sw == shift_override_key)) {
      if(!(meta & POLL_FLAG_SHIFT) && sh) {
        //debug2('S');
        // we need to do a virtual shift.
        set_switch(C64_KEY_LSHIFT,TRUE);
        // set special key.
        shift_override=TRUE;
        shift_override_key=sw;
        shift_override_state=sh;
      } else if((meta&POLL_FLAG_SHIFT) && !sh) {
        //debug2('U');
        // we need to take off the shift keys...
        // set special key.
        set_switch(C64_KEY_LSHIFT,FALSE);
        set_switch(C64_KEY_RSHIFT,FALSE);
        shift_override=TRUE;
        shift_override_key=sw;
        shift_override_state=sh;
        delay_jiffy();
      }
      set_switch(sw,!brk);
    }
  } else if(((meta&POLL_FLAG_SHIFT) && sh) || (!(meta&POLL_FLAG_SHIFT) && !sh)) {
    // key up, and we were not in shift override...
    set_switch(sw,!brk);
  }
}

void reset_matrix(void) {
  uint8_t i=0;

  // reset switches...
  do {
    _set_switch(i+=2,FALSE);
  } while (i!=0);
}

inline void set_LED(uint8_t led) {
  led_state|=led;
  PS2_send(PS2_CMD_LEDS);
  PS2_send(led_state);
}

inline void clear_LED(uint8_t led) {
  led_state&=(uint8_t)~led;
  PS2_send(PS2_CMD_LEDS);
  PS2_send(led_state);
}

inline void load_defaults(void) {
  uint8_t data;
  
  led_state=0;
  while(!eeprom_is_ready());
  data=eeprom_read_byte((void*)0);
  layout=data;
  while(!eeprom_is_ready());
  data=eeprom_read_byte((void*)1);
  if(data)
    set_LED(PS2_LED_NUM_LOCK);
  
  meta=0;
  shift_override=FALSE;
}

void poll_init(void) {
  led_init(LED_PIN_7);
  PIN_SET_HIZ(DDRD,PORTD,PIN5);
  XPT_PORT_STROBE_OUT&=(uint8_t)~XPT_PIN_STROBE;
  XPT_DDR_STROBE|=XPT_PIN_STROBE;
  XPT_DDR_DATA=0xff;
  SW_init(SW_TYPE_OUTPUT,(1<<SW_RESTORE) | (1<<SW_CAPSENSE) | (1<<SW_4080));
  while(!eeprom_is_ready());
  eeprom_write_byte((void*)0,0);
  while(!eeprom_is_ready());
  eeprom_write_byte((void*)1,0);
  while(!eeprom_is_ready());
  eeprom_write_byte((void*)2,0);
  reset_matrix();
  // for delays
  OCR2=255; 
  
}

#define PS2_PKEY_NONE           0x00
#define PS2_PKEY_PAUSE          0x7f
#define PS2_PKEY_PRINT_SCREEN   0x7e

inline void map_positional_c64(uint8_t sh, uint8_t code, uint8_t brk) {
  // TODO I really would like to get rid of these functions.
  switch(code) {
    case 0x80| PS2_PKEY_PAUSE:
      set_matrix(sh,C64_PKEY_RESTORE,brk);
      break;
    case PS2_KEY_CAPS_LOCK:
  // soft caps lock?
      break;
    case 0x80 | PS2_KEY_NUM_SLASH:
      set_matrix(FALSE,C64_KEY_SLASH,brk);
      break;
    case 0x80 | PS2_KEY_INSERT:
      set_matrix(TRUE,C64_KEY_DELETE,brk);
      break;
    case 0x80 | PS2_KEY_DELETE:
      set_matrix(FALSE,C64_KEY_DELETE,brk);
      break;
    case 0x80 | PS2_KEY_HOME:
      set_matrix(sh,C64_KEY_HOME,brk);
      break;
    case 0x80 | PS2_KEY_NUM_ENTER:
      set_matrix(sh,C64_KEY_RETURN,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_DOWN:
      set_matrix(FALSE,C64_KEY_CRSR_DOWN,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_UP:
      set_matrix(TRUE,C64_KEY_CRSR_DOWN,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_RIGHT:
      set_matrix(FALSE,C64_KEY_CRSR_RIGHT,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_LEFT:
      set_matrix(TRUE,C64_KEY_CRSR_RIGHT,brk);
      break;
    case PS2_KEY_ESC:
    case 0x80 | PS2_KEY_END:
      set_matrix(sh,C64_KEY_RUNSTOP,brk);
      break;
    case 0x80 | PS2_KEY_RCTRL:
      set_matrix(sh,C64_KEY_CTRL,brk);
      break;
    case 0x80 | PS2_KEY_RALT:
      set_matrix(sh,C64_KEY_CBM,brk);
      break;
  }
}

inline void map_positional_c128(uint8_t sh, uint8_t code, uint8_t brk) {
  // TODO I really would like to get rid of these functions.
  switch(code) {
    case 0x80| PS2_PKEY_PAUSE:
      set_matrix(sh,C64_PKEY_RESTORE,brk);
      break;
    case 0x80 | PS2_KEY_NUM_SLASH:
      set_matrix(FALSE,C64_KEY_SLASH,brk);
      break;
    case 0x80 | PS2_KEY_INSERT:
      set_matrix(TRUE,C64_KEY_DELETE,brk);
      break;
    case 0x80 | PS2_KEY_DELETE:
      set_matrix(FALSE,C64_KEY_DELETE,brk);
      break;
    case 0x80 | PS2_KEY_HOME:
      set_matrix(sh,C64_KEY_HOME,brk);
      break;
    case 0x80 | PS2_KEY_END:
      set_matrix(sh,C64_KEY_RUNSTOP,brk);
      break;
    case 0x80 | PS2_KEY_RCTRL:
      set_matrix(sh,C64_KEY_CTRL,brk);
      break;
    // changed stuff
    case PS2_KEY_ESC:
      set_matrix(sh,C128_KEY_ESC,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_DOWN:
      set_matrix(sh,C128_KEY_CRSR_DOWN,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_UP:
      set_matrix(sh,C128_KEY_CRSR_UP,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_RIGHT:
      set_matrix(sh,C128_KEY_CRSR_RIGHT,brk);
      break;
    case 0x80 | PS2_KEY_CRSR_LEFT:
      set_matrix(sh,C128_KEY_CRSR_LEFT,brk);
      break;
    case 0x80 | PS2_KEY_RALT:
      set_matrix(sh,C128_KEY_ALT,brk);
      break;
    case PS2_KEY_CAPS_LOCK:
      set_matrix(sh,C128_PKEY_CAPSENSE,brk);
      break;
    case 0x80 | PS2_KEY_NUM_ENTER:
      set_matrix(sh,C128_KEY_ENTER,brk);
      break;
    // new stuff.
    case 0x80 | PS2_KEY_PAGE_UP:
      set_matrix(sh,C64_KEY_CRSR_DOWN,brk);
      break;
    case 0x80 | PS2_KEY_PAGE_DOWN:
      set_matrix(sh,C64_KEY_CRSR_RIGHT,brk);
      break;
    case 0x80 | PS2_PKEY_PRINT_SCREEN:
      if(sh)
        set_matrix(sh,C128_PKEY_4080,brk);
      else
        set_matrix(FALSE,C128_KEY_HELP,brk);
      break;
  }
}

inline void map_symbolic_c64(uint8_t sh, uint8_t code, uint8_t brk) {
  // TODO I really would like to get rid of these functions.
}

inline void map_symbolic_c128(uint8_t sh, uint8_t code, uint8_t brk) {
  // TODO I really would like to get rid of these functions.
}

inline void remap_personal(uint8_t map, uint8_t* shift, uint8_t* code) {
  // this is where we'd put the user's overrides...
}

inline void remap_keypad(uint8_t* sh,uint8_t* code) {
  if(!(led_state & PS2_LED_NUM_LOCK)) {
    // need to swap out keys for cursor and pageup/down etc.
    switch(*code) {
      case PS2_KEY_NUM_0:
      case PS2_KEY_NUM_1:
      case PS2_KEY_NUM_2:
      case PS2_KEY_NUM_3:
      case PS2_KEY_NUM_4:
      case PS2_KEY_NUM_5:
      case PS2_KEY_NUM_6:
      case PS2_KEY_NUM_7:
      case PS2_KEY_NUM_8:
      case PS2_KEY_NUM_9:
      case PS2_KEY_NUM_PERIOD:
        *code |= 0x80;
        break;
    }
  }
}

inline void map_key(uint8_t sh, uint8_t code,uint8_t brk) {
  uint8_t map;
  
  if(code==PS2_KEY_NUM_LOCK) {
    // do nothing, as we already handled this.
  } else if(code!= PS2_KEY_CAPS_LOCK && code<=PS2_KEY_F7) {
    // look it up.
    map=pgm_read_byte(&normal[code]);
    if(map<C64_KEY_SPECIAL) {
      set_matrix(sh,map,brk);
    } else if(map<0xf0) {
      // special key.
      map=pgm_read_byte(&layouts[layout][map-C64_KEY_SPECIAL][(meta&POLL_FLAG_SHIFT)!=0]);
      set_matrix(map&0x01,map&0xfe,brk);
    }
  } else {
    // extended key.
    // map to layout.
    switch(layout) {
      case POLL_LAYOUT_SYMBOLIC_C64:
        map_symbolic_c64(sh,code,brk);
        break;
      case POLL_LAYOUT_SYMBOLIC_C128:
        map_symbolic_c128(sh,code,brk);
        break;
      case POLL_LAYOUT_POSITIONAL_C64:
        map_positional_c64(sh,code,brk);
        break;
      case POLL_LAYOUT_POSITIONAL_C128:
        map_positional_c64(sh,code,brk);
        break;
    }
  }
}

/* 
 * In this function, all keys are mapped to single char.  Relationship:
 * 
 * normal = 0x00 - 0x84
 * ext    = 0x80 - 0xfd  (might be an issue if there is e0 (80-84)
 * print screen = 0xfe
 * pause/break  = 0xff
 */
 
inline void poll_parse_key(uint8_t code, uint8_t brk) {
  uint8_t sh=meta&POLL_FLAG_SHIFT;
  
  // this is also where we can check for Ctrl/Alt/Del
  if((code&0x7f)==PS2_KEY_ALT) {
    // turn on or off the ALT META flag
    meta=(meta&(uint8_t)~POLL_FLAG_ALT) | (brk?0:POLL_FLAG_ALT);
  } else if((code&0x7f)==PS2_KEY_LCTRL) {
    // turn on or off the CTRL META flag
    meta=(meta&(uint8_t)~POLL_FLAG_CONTROL) | (brk?0:POLL_FLAG_CONTROL);
  } 
  if((meta&POLL_FLAG_CTRL_ALT)==POLL_FLAG_CTRL_ALT && code==(0x80 | PS2_KEY_DELETE) && !brk) {
    // CTRL/ALT/DEL is pressed.
    // bring RESET line low
    //debug2('^');
    PIN_SET_LOW(DDRD,PORTD,PIN5);
    PIN_SET_LOW(DDRD,PORTD,PIN5);
    PIN_SET_LOW(DDRD,PORTD,PIN5);
    PIN_SET_HIZ(DDRD,PORTD,PIN5);
    //goto *0x0000;
  } else if((meta&POLL_FLAG_CTRL_ALT)==POLL_FLAG_CTRL_ALT && code==PS2_KEY_BS) {
    // ALT + Backspace.
    debug2('|');
    // enter config
  } else {
    // now, apply user preferences
    remap_personal(layout,&sh,&code);
    // handle num_lock/joystick...
    remap_keypad(&sh,&code);
    // do we still have a code, or did joystick driver EAT it?
    if(code) {
      // map key.
      map_key(sh,code,brk);
    }
  }
}

void poll(void) {
  uint8_t key;
  led_on(LED_PIN_7);
  
  for(;;) {
    if(PS2_data_available() != 0) {
      // kb sent data...
      key=PS2_recv();
      //printHex(key);
      if(key==PS2_CMD_BAT) {
        load_defaults();
        reset_matrix();
        state=PS2_ST_IDLE;
      }
      switch(state) {
        case POLL_ST_IDLE:
          switch(key) {
            case PS2_KEY_EXT:
              // we got E0
              state=POLL_ST_GET_X_KEY;
              break;
            case PS2_KEY_UP:
              // get normal key up.
              state=POLL_ST_GET_KEY_UP;
              break;
            case PS2_KEY_EXT_2:
              state=POLL_ST_GET_PAUSE_1;
              // start on the Pause/Break sequence.
              break;
            case PS2_KEY_NUM_LOCK:
              // handle NUM_LOCK pressed.
              if(led_state&PS2_LED_NUM_LOCK)
                clear_LED(PS2_LED_NUM_LOCK);
              else
                set_LED(PS2_LED_NUM_LOCK);
              state=PS2_ST_IDLE;
              break;
            case PS2_CMD_ACK:
            case PS2_CMD_ECHO:
            case PS2_CMD_ERROR:
            case PS2_CMD_OVERFLOW:
              break;
            default:
              if(key<=PS2_KEY_F7) {
                poll_parse_key(key,FALSE);
              }
              state=POLL_ST_IDLE;
              break;
          }
          break;
        case POLL_ST_GET_KEY_UP:
          poll_parse_key(key,TRUE);
          state=POLL_ST_IDLE;
          break;
        case POLL_ST_GET_X_KEY:
          if(key==PS2_KEY_UP) {
            state=POLL_ST_GET_X_KEY_UP;
          } else if(key==PS2_KEY_LSHIFT) {
            // start of Print Screen?
            state=POLL_ST_PRINT_1;
          } else {
            poll_parse_key(0x80 | key,FALSE);
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_X_KEY_UP:
          if(key==PS2_KEY_PRINT_SCREEN) {
            //PrintScr up?
            state=POLL_ST_PRINT_UP_1;
          } else {
            poll_parse_key(0x80 | key,TRUE);
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_PRINT_1:
          if(key==PS2_KEY_EXT) {
            state=POLL_ST_PRINT_2;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_PRINT_2:
          if(key==PS2_KEY_PRINT_SCREEN) {
            //debug('P');
            poll_parse_key(0x80|PS2_PKEY_PRINT_SCREEN,FALSE);
          }
          state=POLL_ST_IDLE;
          break;
        case POLL_ST_PRINT_UP_1:
          if(key==PS2_KEY_EXT) {
            state=POLL_ST_PRINT_UP_2;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_PRINT_UP_2:
          if(key==PS2_KEY_UP) {
            state=POLL_ST_PRINT_UP_3;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_PRINT_UP_3:
          if(key==PS2_KEY_LSHIFT) {
            //debug('p');
            poll_parse_key(0x80|PS2_PKEY_PRINT_SCREEN,TRUE);
          }
          state=POLL_ST_IDLE;
          break;
        case POLL_ST_GET_PAUSE_1:
          // we get 14
          if(key==PS2_KEY_PCTRL) {
            state=POLL_ST_GET_PAUSE_2;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_PAUSE_2:
          // we got 77
          if(key==PS2_KEY_PAUSE) {
            state=POLL_ST_GET_PAUSE_3;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_PAUSE_3:
          // we get E1
          if(key==PS2_KEY_EXT_2) {
            state=POLL_ST_GET_PAUSE_4;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_PAUSE_4:
          // we got F0
          if(key==PS2_KEY_UP) {
            state=POLL_ST_GET_PAUSE_5;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_PAUSE_5:
          // we got 14
          if(key==PS2_KEY_PCTRL) {
            state=POLL_ST_GET_PAUSE_6;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_PAUSE_6:
          // we got F0
          if(key==PS2_KEY_UP) {
            state=POLL_ST_GET_PAUSE_7;
          } else {
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_PAUSE_7:
          // we got 14
          if(key==PS2_KEY_PAUSE) {
            //debug('R');
            // we received a complete Pause/Break, do something about it.
            poll_parse_key(0x80|PS2_PKEY_PAUSE,FALSE);
            poll_parse_key(0x80|PS2_PKEY_PAUSE,TRUE);
          }
          state=POLL_ST_IDLE;
          break;
          
      }
    }
  }
}
