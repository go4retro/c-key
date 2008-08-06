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
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "ps2.h"
#include "util.h"
#include "switches.h"
#include "poll.h"
#include "poll64.h"
#include "led.h"

static prog_uint8_t normal[0x84] =  { POLL_CBM_KEY_UNMAPPED,POLL_CBM_KEY_SPECIAL+41,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+0,POLL_CBM_KEY_SPECIAL+1,POLL_CBM_KEY_SPECIAL+2,POLL_CBM_KEY_SPECIAL+3,POLL_CBM_KEY_SPECIAL+44
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+42,POLL_CBM_KEY_SPECIAL+4,POLL_CBM_KEY_SPECIAL+5,POLL_CBM_KEY_SPECIAL+6,POLL_CBM_KEY_SPECIAL+40,POLL_C64_KEY_BACKARROW,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+8,POLL_CBM_KEY_SPECIAL+9,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+10,POLL_C64_KEY_Q,POLL_C64_KEY_1,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_C64_KEY_Z,POLL_C64_KEY_S,POLL_C64_KEY_A,POLL_C64_KEY_W,POLL_CBM_KEY_SPECIAL+11,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_C64_KEY_C,POLL_C64_KEY_X,POLL_C64_KEY_D,POLL_C64_KEY_E,POLL_C64_KEY_4,POLL_C64_KEY_3,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_C64_KEY_SPACE,POLL_C64_KEY_V,POLL_C64_KEY_F,POLL_C64_KEY_T,POLL_C64_KEY_R,POLL_C64_KEY_5,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_C64_KEY_N,POLL_C64_KEY_B,POLL_C64_KEY_H,POLL_C64_KEY_G,POLL_C64_KEY_Y,POLL_CBM_KEY_SPECIAL+12,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_C64_KEY_M,POLL_C64_KEY_J,POLL_C64_KEY_U,POLL_CBM_KEY_SPECIAL+13,POLL_CBM_KEY_SPECIAL+14,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_C64_KEY_COMMA,POLL_C64_KEY_K,POLL_C64_KEY_I,POLL_C64_KEY_O,POLL_CBM_KEY_SPECIAL+15,POLL_CBM_KEY_SPECIAL+16,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_C64_KEY_PERIOD,POLL_C64_KEY_SLASH,POLL_C64_KEY_L,POLL_CBM_KEY_SPECIAL+17,POLL_C64_KEY_P,POLL_CBM_KEY_SPECIAL+18,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+19,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+20,POLL_CBM_KEY_SPECIAL+21,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_UNMAPPED,POLL_C64_KEY_RSHIFT,POLL_C64_KEY_RETURN,POLL_CBM_KEY_SPECIAL+22,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+23,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_C64_KEY_DELETE,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+24,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+25,POLL_CBM_KEY_SPECIAL+26,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_SPECIAL+27,POLL_CBM_KEY_SPECIAL+28,POLL_CBM_KEY_SPECIAL+29,POLL_CBM_KEY_SPECIAL+30,POLL_CBM_KEY_SPECIAL+31,POLL_CBM_KEY_SPECIAL+32,POLL_CBM_KEY_SPECIAL+33,POLL_CBM_KEY_UNMAPPED
                                  ,POLL_CBM_KEY_SPECIAL+43,POLL_CBM_KEY_SPECIAL+34,POLL_CBM_KEY_SPECIAL+35,POLL_CBM_KEY_SPECIAL+36,POLL_CBM_KEY_SPECIAL+37,POLL_CBM_KEY_SPECIAL+38,POLL_CBM_KEY_SPECIAL+39,POLL_CBM_KEY_NONE
                                  ,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_NONE,POLL_CBM_KEY_SPECIAL+7};
                                  
static prog_uint8_t layouts[4][45][2] = {
                                      // c64 positional mapping.
                                         {
                                          {POLL_MAP_NONE | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                 // 0 F5 ->  F5/Sh+F5
                                          {POLL_MAP_NONE | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                 // 1 F3 ->  F3/Sh+F3 
                                          {POLL_MAP_NONE | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                 // 2 F1 ->  F1/Sh+F1
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                // 3 F2 ->  Sh+F1/Sh+F1
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F7, POLL_MAP_SHIFT | POLL_C64_KEY_F7},               // 4 F8 ->  Sh+F7/Sh+F7  
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                // 5 F6 ->  Sh+F5/Sh+F5
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                // 6 F4 ->  Sh+F3/Sh+F3
                                          {POLL_MAP_NONE | POLL_C64_KEY_F7,POLL_MAP_SHIFT | POLL_C64_KEY_F7},                 // 7 F7 ->  F7/Sh+F7
                                          {POLL_MAP_NONE | POLL_C64_KEY_CBM,POLL_MAP_SHIFT | POLL_C64_KEY_CBM},               // 8 Left ALT ->  CBM/Sh+CBM
                                          {POLL_MAP_NONE | POLL_C64_KEY_LSHIFT,POLL_MAP_SHIFT | POLL_C64_KEY_LSHIFT},         // 9 Left Shift -> LeftSh/LeftSh
                                          {POLL_MAP_NONE | POLL_C64_KEY_CTRL,POLL_MAP_SHIFT | POLL_C64_KEY_CTRL},             // 10 Left Control -> CTRL/SH+CTRL
                                          {POLL_MAP_NONE | POLL_C64_KEY_2,POLL_MAP_NONE | POLL_C64_KEY_AT},                   // 11 2 ->  2/@
                                          {POLL_MAP_NONE | POLL_C64_KEY_6,POLL_MAP_NONE | POLL_C64_KEY_UPARROW},              // 12 6 ->  6/^
                                          {POLL_MAP_NONE | POLL_C64_KEY_7,POLL_MAP_SHIFT | POLL_C64_KEY_6},                   // 13 7 ->  7/&
                                          {POLL_MAP_NONE | POLL_C64_KEY_8, POLL_MAP_NONE | POLL_C64_KEY_ASTERIX},             // 14 8 ->  8/*
                                          {POLL_MAP_NONE | POLL_C64_KEY_0,POLL_MAP_SHIFT | POLL_C64_KEY_9},                   // 15 0 ->  0/)
                                          {POLL_MAP_NONE | POLL_C64_KEY_9,POLL_MAP_SHIFT | POLL_C64_KEY_8},                   // 16 9 ->  9/(
                                          {POLL_MAP_NONE | POLL_C64_KEY_SEMICOLON,POLL_MAP_NONE | POLL_C64_KEY_COLON},        // 17 ; ->  ;/:
                                          {POLL_MAP_NONE | POLL_C64_KEY_MINUS,POLL_MAP_SHIFT | POLL_C64_KEY_MINUS},           // 18 - ->  -/Sh+-
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_7,POLL_MAP_SHIFT | POLL_C64_KEY_2},                  // 19 ' ->  '/"
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_COLON,POLL_MAP_SHIFT | POLL_C64_KEY_AT},             // 20 [ ->  [/Sh+@
                                          {POLL_MAP_NONE | POLL_C64_KEY_EQUALS,POLL_MAP_NONE | POLL_C64_KEY_PLUS},            // 21 = ->  =/+
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_SEMICOLON,POLL_MAP_SHIFT | POLL_C64_KEY_ASTERIX},    // 22 ] ->  ]/Sh+*
                                          {POLL_MAP_NONE | POLL_C64_KEY_POUND,POLL_MAP_SHIFT | POLL_C64_KEY_POUND},           // 23 \ ->  pound/Sh+pound
                                          {POLL_MAP_NONE | POLL_C64_KEY_1, POLL_MAP_NONE | POLL_C64_KEY_1},                   // 24 Num 1 ->  1/1
                                          {POLL_MAP_NONE | POLL_C64_KEY_4,POLL_MAP_NONE | POLL_C64_KEY_4},                    // 25 Num 4 ->  4/4
                                          {POLL_MAP_NONE | POLL_C64_KEY_7,POLL_MAP_NONE | POLL_C64_KEY_7},                    // 26 Num 7 ->  7/7
                                          {POLL_MAP_NONE | POLL_C64_KEY_0,POLL_MAP_NONE | POLL_C64_KEY_0},                    // 27 Num 0 ->  0/0
                                          {POLL_MAP_NONE | POLL_C64_KEY_PERIOD,POLL_MAP_NONE | POLL_C64_KEY_PERIOD},          // 28 Num . ->  ./.
                                          {POLL_MAP_NONE | POLL_C64_KEY_2,POLL_MAP_NONE | POLL_C64_KEY_2},                    // 29 Num 2 ->  2/2
                                          {POLL_MAP_NONE | POLL_C64_KEY_5,POLL_MAP_NONE | POLL_C64_KEY_5},                    // 30 Num 5 ->  5/5
                                          {POLL_MAP_NONE | POLL_C64_KEY_6,POLL_MAP_NONE | POLL_C64_KEY_6},                    // 31 Num 6 ->  6/6
                                          {POLL_MAP_NONE | POLL_C64_KEY_8,POLL_MAP_NONE | POLL_C64_KEY_8},                    // 32 Num 8 ->  8/8
                                          {POLL_MAP_NONE | POLL_C64_KEY_RUNSTOP,POLL_MAP_SHIFT | POLL_C64_KEY_RUNSTOP},       // 33 Esc ->  RunStop/Sh+RunStop
                                          {POLL_MAP_NONE | POLL_C64_KEY_PLUS, POLL_MAP_NONE | POLL_C64_KEY_PLUS},             // 34 Num + ->  +/+
                                          {POLL_MAP_NONE | POLL_C64_KEY_3,POLL_MAP_NONE | POLL_C64_KEY_3},                    // 35 Num 3 ->  3/3
                                          {POLL_MAP_NONE | POLL_C64_KEY_MINUS,POLL_MAP_NONE | POLL_C64_KEY_MINUS},            // 36 Num - ->  -/-
                                          {POLL_MAP_NONE | POLL_C64_KEY_ASTERIX,POLL_MAP_NONE | POLL_C64_KEY_ASTERIX},        // 37 Num * ->  */*
                                          {POLL_MAP_NONE | POLL_C64_KEY_9,POLL_MAP_NONE | POLL_C64_KEY_9},                    // 38 Num 9 ->  9/9
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_CBM_KEY_UNMAPPED},     // 39 Scroll Lock -> unmapped/Sh+unmapped
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_CBM_KEY_UNMAPPED},     // 40 TAB ->  unmapped/Sh+unmapped
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_0},            // 41 F9 ->  /Sh+0
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_PLUS},         // 42 F10 ->  /Sh++
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_EQUALS},       // 43 F11 ->  /Sh+=
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_UPARROW},      // 44 F12 ->  /Sh+^
                                         }
                                         ,
                                         {
                                          // c128 positional
                                          {POLL_MAP_NONE | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                 // 0 F5 ->  F5/Sh+F5
                                          {POLL_MAP_NONE | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                 // 1 F3 ->  F3/Sh+F3
                                          {POLL_MAP_NONE | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                 // 2 F1 ->  F1/Sh+F1
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                // 3 F2 ->  Sh+F1/Sh+F1
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F7, POLL_MAP_SHIFT | POLL_C64_KEY_F7},               // 4 F8 ->  Sh+F7/Sh+F7
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                // 5 F6 ->  Sh+F5/Sh+F5
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                // 6 F4 ->  Sh+F3/Sh+F3
                                          {POLL_MAP_NONE | POLL_C64_KEY_F7,POLL_MAP_SHIFT | POLL_C64_KEY_F7},                 // 7 F7 ->  F7/Sh+F7
                                          {POLL_MAP_NONE | POLL_C64_KEY_CBM,POLL_MAP_SHIFT | POLL_C64_KEY_CBM},               // 8 Left ALT -> CBM/Sh+CBM 
                                          {POLL_MAP_NONE | POLL_C64_KEY_LSHIFT,POLL_MAP_SHIFT | POLL_C64_KEY_LSHIFT},         // 9 Left Shift -> LShift/LShift
                                          {POLL_MAP_NONE | POLL_C64_KEY_CTRL,POLL_MAP_SHIFT | POLL_C64_KEY_CTRL},             // 10 Left Control -> CTRL/Sh+CTRL
                                          {POLL_MAP_NONE | POLL_C64_KEY_2,POLL_MAP_NONE | POLL_C64_KEY_AT},                   // 11 2 ->  2/@
                                          {POLL_MAP_NONE | POLL_C64_KEY_6,POLL_MAP_NONE | POLL_C64_KEY_UPARROW},              // 12 6 ->  6/^
                                          {POLL_MAP_NONE | POLL_C64_KEY_7,POLL_MAP_SHIFT | POLL_C64_KEY_6},                   // 13 7 ->  7/&
                                          {POLL_MAP_NONE | POLL_C64_KEY_8, POLL_MAP_NONE | POLL_C64_KEY_ASTERIX},             // 14 8 ->  8/*
                                          {POLL_MAP_NONE | POLL_C64_KEY_0,POLL_MAP_SHIFT | POLL_C64_KEY_9},                   // 15 0 ->  0/)
                                          {POLL_MAP_NONE | POLL_C64_KEY_9,POLL_MAP_SHIFT | POLL_C64_KEY_8},                   // 16 9 ->  9/(
                                          {POLL_MAP_NONE | POLL_C64_KEY_SEMICOLON,POLL_MAP_NONE | POLL_C64_KEY_COLON},        // 17 ; ->  ;/:
                                          {POLL_MAP_NONE | POLL_C64_KEY_MINUS,POLL_MAP_SHIFT | POLL_C64_KEY_MINUS},           // 18 - ->  -/Sh+-
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_7,POLL_MAP_SHIFT | POLL_C64_KEY_2},                  // 19 ' ->  '/"
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_COLON,POLL_MAP_SHIFT | POLL_C64_KEY_AT},             // 20 [ ->  [/Sh+@
                                          {POLL_MAP_NONE | POLL_C64_KEY_EQUALS,POLL_MAP_NONE | POLL_C64_KEY_PLUS},            // 21 = ->  =/+
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_SEMICOLON,POLL_MAP_SHIFT | POLL_C64_KEY_ASTERIX},    // 22 ] ->  ]/Sh+*
                                          {POLL_MAP_NONE | POLL_C64_KEY_POUND,POLL_MAP_SHIFT | POLL_C64_KEY_POUND},           // 23 | ->  pound/Sh+pound
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_1, POLL_MAP_SHIFT | POLL_C128_KEY_NUM_1},        // 24 Num 1 -> Num 1/Sh+Num 1
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_4,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_4},         // 25 Num 4 -> Num 4/Sh+Num 4
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_7,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_7},         // 26 Num 7 ->  Num 7/Sh+Num 7
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_0,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_0},         // 27 Num 0 -> Num 0/Sh+Num 0
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_PERIOD,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_PERIOD},// 28 Num . -> Num ./Sh+Num .
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_2,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_2},         // 29 Num 2 -> Num 2/Sh+Num 2
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_5,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_5},         // 30 Num 5 -> Num 5/Sh+Num 5
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_6,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_6},         // 31 Num 6 -> Num 6/Sh+Num 6
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_8,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_8},         // 32 Num 8 -> Num 8/Sh+Num 8
                                          {POLL_MAP_NONE | POLL_C128_KEY_ESC,POLL_MAP_SHIFT | POLL_C128_KEY_ESC},             // 33 Esc -> ESC/Sh+ESC
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_PLUS, POLL_MAP_SHIFT | POLL_C128_KEY_NUM_PLUS},  // 34 Num + -> Num +/Sh+Num +
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_3,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_3},         // 35 Num 3 -> Num 3/Sh+Num 3
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_MINUS,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_MINUS}, // 36 Num - -> Num -/Sh+Num - 
                                          {POLL_MAP_NONE | POLL_C64_KEY_ASTERIX,POLL_MAP_NONE | POLL_C64_KEY_ASTERIX},        // 37 Num * -> Num */Sh+Num *
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_9,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_9},         // 38 Num 9 -> Num 9/Sh+Num 9
                                          {POLL_MAP_NONE | POLL_C128_KEY_NO_SCROLL,POLL_MAP_NONE | POLL_C128_KEY_LINEFEED},   // 39 Scroll Lock -> NoScroll/LineFeed
                                          {POLL_MAP_NONE | POLL_C128_KEY_TAB,POLL_MAP_SHIFT | POLL_C128_KEY_TAB},             // 40 TAB -> TAB/Sh+TAB
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_0},            // 41 F9 ->  /Sh+0
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_PLUS},         // 42 F10 ->  /Sh++
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_EQUALS},       // 43 F11 ->  /Sh+=
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_UPARROW},      // 44 F12 ->  /Sh+^
                                         }
                                         ,
                                         {
                                          {POLL_MAP_NONE | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                 // 0 F5 ->  F5/Sh+F5
                                          {POLL_MAP_NONE | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                 // 1 F3 ->  F3/Sh+F3
                                          {POLL_MAP_NONE | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                 // 2 F1 ->  F1/Sh+F1
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                // 3 F2 ->  Sh+F1/Sh+F2
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F7, POLL_MAP_SHIFT | POLL_C64_KEY_F7},               // 4 F8 ->  Sh+F7/Sh+F7
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                // 5 F6 ->  Sh+F5/Sh+F5
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                // 6 F4 ->  Sh+F3/Sh+F3  
                                          {POLL_MAP_NONE | POLL_C64_KEY_F7,POLL_MAP_SHIFT | POLL_C64_KEY_F7},                 // 7 F7 ->  F7/Sh+F7
                                          {POLL_MAP_NONE | POLL_CBM_KEY_NONE,POLL_MAP_SHIFT | POLL_CBM_KEY_NONE},             // 8 Left ALT -> Nothing/Sh+Nothing
                                          {POLL_MAP_NONE | POLL_C64_KEY_LSHIFT,POLL_MAP_SHIFT | POLL_C64_KEY_LSHIFT},         // 9 Left Shift -> Left Shift/Sh+Left Shift
                                          {POLL_MAP_NONE | POLL_C64_KEY_CBM,POLL_MAP_SHIFT | POLL_C64_KEY_CBM},               // 10 Left  Control -> CBM/Sh+CBM
                                          {POLL_MAP_NONE | POLL_C64_KEY_2,POLL_MAP_SHIFT | POLL_C64_KEY_2},                   // 11 2 ->  2/Sh+2
                                          {POLL_MAP_NONE | POLL_C64_KEY_6,POLL_MAP_SHIFT | POLL_C64_KEY_6},                   // 12 6 ->  6/Sh+6
                                          {POLL_MAP_NONE | POLL_C64_KEY_7,POLL_MAP_SHIFT | POLL_C64_KEY_7},                   // 13 7 ->  7/Sh+7
                                          {POLL_MAP_NONE | POLL_C64_KEY_8, POLL_MAP_SHIFT | POLL_C64_KEY_8},                  // 14 8 ->  8/Sh+8
                                          {POLL_MAP_NONE | POLL_C64_KEY_0,POLL_MAP_SHIFT | POLL_C64_KEY_0},                   // 15 0 ->  0/Sh+0
                                          {POLL_MAP_NONE | POLL_C64_KEY_9,POLL_MAP_SHIFT | POLL_C64_KEY_9},                   // 16 9 ->  9/Sh+9
                                          {POLL_MAP_NONE | POLL_C64_KEY_SEMICOLON,POLL_MAP_SHIFT | POLL_C64_KEY_SEMICOLON},   // 17 ; ->  ;/Sh+;
                                          {POLL_MAP_NONE | POLL_C64_KEY_PLUS,POLL_MAP_SHIFT | POLL_C64_KEY_PLUS},             // 18 - ->  +/Sh++
                                          {POLL_MAP_NONE | POLL_C64_KEY_SEMICOLON,POLL_MAP_SHIFT | POLL_C64_KEY_SEMICOLON},   // 19 ' ->  ;/Sh+;
                                          {POLL_MAP_NONE | POLL_C64_KEY_AT,POLL_MAP_SHIFT | POLL_C64_KEY_AT},                 // 20 [ ->  @/Sh+@
                                          {POLL_MAP_NONE | POLL_C64_KEY_MINUS,POLL_MAP_SHIFT | POLL_C64_KEY_MINUS},           // 21 = ->  -/Sh+-
                                          {POLL_MAP_NONE | POLL_C64_KEY_ASTERIX,POLL_MAP_SHIFT | POLL_C64_KEY_ASTERIX},       // 22 ] ->  */Sh+*
                                          {POLL_MAP_NONE | POLL_C64_KEY_POUND,POLL_MAP_SHIFT | POLL_C64_KEY_POUND},           // 23 | ->  pound/Sh+pound
                                          {POLL_MAP_NONE | POLL_C64_KEY_1, POLL_MAP_NONE | POLL_C64_KEY_1},                   // 24 Num 1 ->  1/1
                                          {POLL_MAP_NONE | POLL_C64_KEY_4,POLL_MAP_NONE | POLL_C64_KEY_4},                    // 25 Num 4 ->  4/4
                                          {POLL_MAP_NONE | POLL_C64_KEY_7,POLL_MAP_NONE | POLL_C64_KEY_7},                    // 26 Num 7 ->  7/7
                                          {POLL_MAP_NONE | POLL_C64_KEY_0,POLL_MAP_NONE | POLL_C64_KEY_0},                    // 27 Num 0 ->  0/0
                                          {POLL_MAP_NONE | POLL_C64_KEY_PERIOD,POLL_MAP_NONE | POLL_C64_KEY_PERIOD},          // 28 Num Dot ->  ./.
                                          {POLL_MAP_NONE | POLL_C64_KEY_2,POLL_MAP_NONE | POLL_C64_KEY_2},                    // 29 Num 2 ->  2/2
                                          {POLL_MAP_NONE | POLL_C64_KEY_5,POLL_MAP_NONE | POLL_C64_KEY_5},                    // 30 Num 5 ->  5/5
                                          {POLL_MAP_NONE | POLL_C64_KEY_6,POLL_MAP_NONE | POLL_C64_KEY_6},                    // 31 Num 6 ->  6/6
                                          {POLL_MAP_NONE | POLL_C64_KEY_8,POLL_MAP_NONE | POLL_C64_KEY_8},                    // 32 Num 8 ->  8/8
                                          {POLL_MAP_NONE | POLL_C64_KEY_RUNSTOP,POLL_MAP_SHIFT | POLL_C64_KEY_RUNSTOP},       // 33 Esc -> RunStop/Sh+RunStop
                                          {POLL_MAP_NONE | POLL_C64_KEY_PLUS, POLL_MAP_NONE | POLL_C64_KEY_PLUS},             // 34 Num + ->  +/+
                                          {POLL_MAP_NONE | POLL_C64_KEY_3,POLL_MAP_NONE | POLL_C64_KEY_3},                    // 35 Num 3 ->  3/3
                                          {POLL_MAP_NONE | POLL_C64_KEY_MINUS,POLL_MAP_NONE | POLL_C64_KEY_MINUS},            // 36 - Num - ->  -/-
                                          {POLL_MAP_NONE | POLL_C64_KEY_ASTERIX,POLL_MAP_NONE | POLL_C64_KEY_ASTERIX},        // 37 - Num * ->  */*
                                          {POLL_MAP_NONE | POLL_C64_KEY_9,POLL_MAP_NONE | POLL_C64_KEY_9},                    // 38 - Num 9 ->  9/9
                                          {POLL_MAP_NONE | POLL_CBM_KEY_NONE,POLL_MAP_SHIFT | POLL_CBM_KEY_NONE},             // 39 - Scroll Lock -> None/Sh+None
                                          {POLL_MAP_NONE | POLL_C64_KEY_CTRL,POLL_MAP_SHIFT | POLL_C64_KEY_CTRL},             // 40 - TAB ->  CTRL/Sh+CTRL
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_0},            // 41 F9 ->  /Sh+0
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_PLUS},         // 42 F10 ->  /Sh++
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_EQUALS},       // 43 F11 ->  /Sh+=
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_UPARROW},      // 44 F12 ->  /Sh+^
                                         }
                                         ,
                                         {
                                          {POLL_MAP_NONE | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                 // 0 F5 ->  F5/Sh+F5
                                          {POLL_MAP_NONE | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                 // 1 F3 ->  F3/Sh+F3
                                          {POLL_MAP_NONE | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                 // 2 F1 ->  F1/Sh+F1
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F1,POLL_MAP_SHIFT | POLL_C64_KEY_F1},                // 3 F2 ->  Sh+F1/Sh+F2
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F7, POLL_MAP_SHIFT | POLL_C64_KEY_F7},               // 4 F8 ->  Sh+F7/Sh+F7
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F5,POLL_MAP_SHIFT | POLL_C64_KEY_F5},                // 5 F6 ->  Sh+F5/Sh+F5
                                          {POLL_MAP_SHIFT | POLL_C64_KEY_F3,POLL_MAP_SHIFT | POLL_C64_KEY_F3},                // 6 F4 ->  Sh+F3/Sh+F3  
                                          {POLL_MAP_NONE | POLL_C64_KEY_F7,POLL_MAP_SHIFT | POLL_C64_KEY_F7},                 // 7 F7 ->  F7/Sh+F7
                                          {POLL_MAP_NONE | POLL_CBM_KEY_NONE,POLL_MAP_SHIFT | POLL_CBM_KEY_NONE},             // 8 Left ALT -> Nothing/Sh+Nothing
                                          {POLL_MAP_NONE | POLL_C64_KEY_LSHIFT,POLL_MAP_SHIFT | POLL_C64_KEY_LSHIFT},         // 9 Left Shift -> Left Shift/Sh+Left Shift
                                          {POLL_MAP_NONE | POLL_C64_KEY_CBM,POLL_MAP_SHIFT | POLL_C64_KEY_CBM},               // 10 Left  Control -> CBM/Sh+CBM
                                          {POLL_MAP_NONE | POLL_C64_KEY_2,POLL_MAP_SHIFT | POLL_C64_KEY_2},                   // 11 2 ->  2/Sh+2
                                          {POLL_MAP_NONE | POLL_C64_KEY_6,POLL_MAP_SHIFT | POLL_C64_KEY_6},                   // 12 6 ->  6/Sh+6
                                          {POLL_MAP_NONE | POLL_C64_KEY_7,POLL_MAP_SHIFT | POLL_C64_KEY_7},                   // 13 7 ->  7/Sh+7
                                          {POLL_MAP_NONE | POLL_C64_KEY_8, POLL_MAP_SHIFT | POLL_C64_KEY_8},                  // 14 8 ->  8/Sh+8
                                          {POLL_MAP_NONE | POLL_C64_KEY_0,POLL_MAP_SHIFT | POLL_C64_KEY_0},                   // 15 0 ->  0/Sh+0
                                          {POLL_MAP_NONE | POLL_C64_KEY_9,POLL_MAP_SHIFT | POLL_C64_KEY_9},                   // 16 9 ->  9/Sh+9
                                          {POLL_MAP_NONE | POLL_C64_KEY_SEMICOLON,POLL_MAP_SHIFT | POLL_C64_KEY_SEMICOLON},   // 17 ; ->  ;/Sh+;
                                          {POLL_MAP_NONE | POLL_C64_KEY_PLUS,POLL_MAP_SHIFT | POLL_C64_KEY_PLUS},             // 18 - ->  +/Sh++
                                          {POLL_MAP_NONE | POLL_C64_KEY_SEMICOLON,POLL_MAP_SHIFT | POLL_C64_KEY_SEMICOLON},   // 19 ' ->  ;/Sh+;
                                          {POLL_MAP_NONE | POLL_C64_KEY_AT,POLL_MAP_SHIFT | POLL_C64_KEY_AT},                 // 20 [ ->  @/Sh+@
                                          {POLL_MAP_NONE | POLL_C64_KEY_MINUS,POLL_MAP_SHIFT | POLL_C64_KEY_MINUS},           // 21 = ->  -/Sh+-
                                          {POLL_MAP_NONE | POLL_C64_KEY_ASTERIX,POLL_MAP_SHIFT | POLL_C64_KEY_ASTERIX},       // 22 ] ->  */Sh+*
                                          {POLL_MAP_NONE | POLL_C64_KEY_POUND,POLL_MAP_SHIFT | POLL_C64_KEY_POUND},           // 23 | ->  pound/Sh+pound
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_1, POLL_MAP_SHIFT | POLL_C128_KEY_NUM_1},        // 24 Num 1 -> Num 1/Sh+Num 1
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_4,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_4},         // 25 Num 4 -> Num 4/Sh+Num 4
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_7,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_7},         // 26 Num 7 ->  Num 7/Sh+Num 7
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_0,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_0},         // 27 Num 0 -> Num 0/Sh+Num 0
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_PERIOD,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_PERIOD},// 28 Num . -> Num ./Sh+Num .
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_2,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_2},         // 29 Num 2 -> Num 2/Sh+Num 2
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_5,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_5},         // 30 Num 5 -> Num 5/Sh+Num 5
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_6,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_6},         // 31 Num 6 -> Num 6/Sh+Num 6
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_8,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_8},         // 32 Num 8 -> Num 8/Sh+Num 8
                                          {POLL_MAP_NONE | POLL_C64_KEY_RUNSTOP,POLL_MAP_SHIFT | POLL_C64_KEY_RUNSTOP},       // 33 Esc -> RunStop/Sh+RunStop
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_PLUS, POLL_MAP_SHIFT | POLL_C128_KEY_NUM_PLUS},  // 34 Num + -> Num +/Sh+Num +
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_3,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_3},         // 35 Num 3 -> Num 3/Sh+Num 3
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_MINUS,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_MINUS}, // 36 Num - -> Num -/Sh+Num - 
                                          {POLL_MAP_NONE | POLL_C64_KEY_ASTERIX,POLL_MAP_NONE | POLL_C64_KEY_ASTERIX},        // 37 Num * -> Num */Sh+Num *
                                          {POLL_MAP_NONE | POLL_C128_KEY_NUM_9,POLL_MAP_SHIFT | POLL_C128_KEY_NUM_9},         // 38 Num 9 -> Num 9/Sh+Num 9
                                          {POLL_MAP_NONE | POLL_CBM_KEY_NONE,POLL_MAP_SHIFT | POLL_CBM_KEY_NONE},             // 39 - Scroll Lock -> None/Sh+None
                                          {POLL_MAP_NONE | POLL_C64_KEY_CTRL,POLL_MAP_SHIFT | POLL_C64_KEY_CTRL},             // 40 - TAB ->  CTRL/Sh+CTRL
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_0},            // 41 F9 ->  /Sh+0
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_PLUS},         // 42 F10 ->  /Sh++
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_EQUALS},       // 43 F11 ->  /Sh+=
                                          {POLL_MAP_NONE | POLL_CBM_KEY_UNMAPPED,POLL_MAP_SHIFT | POLL_C64_KEY_UPARROW},      // 44 F12 ->  /Sh+^
                                         }
                                      };
                                          

static uint8_t layout;

static uint8_t meta;
static uint8_t config;
static uint8_t shift_override_key;
static uint8_t shift_override;
static uint8_t shift_override_state;

static uint8_t sw_state=0;
static uint8_t led_state=0;
static uint8_t led_counter=0;
static uint8_t debug=0;



static uint8_t state=POLL_ST_IDLE;

void poll_irq(void) {
  led_counter++;
  if(led_counter==POLL_LED_IRQ_DIVIDER) {
    led_counter=0;
    LED_irq();
  }
}

inline void delay_jiffy(void) {
  uint8_t i=TCNT2-1;
  // 1/60 second delay for funky shifting settling.
  //TCNT2=0;
  // Set OC2 clk  to SYSCLK/1024 and Compare Timer Mode
  //TCCR2 = (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21);
  while(TCNT2!=i);
  //while(!(TIFR & (1<<OCF2)));
  //TIFR=(1<<OCF2);
  //TCCR2=0;
}

void set_switch(uint8_t sw, uint8_t state) {
  if(debug) {
    debug(state?'d':'u');
    printHex(sw);
  }
  switch(sw) {
    case POLL_C64_PKEY_RESTORE:
      SW_send(SW_RESTORE | (state?0:SW_UP));
      break;
    case POLL_C128_PKEY_4080:
      SW_send(SW_4080 | (state?0:SW_UP));
      break;
    case POLL_C128_PKEY_CAPSENSE:
      SW_send(SW_CAPSENSE | (state?0:SW_UP));
      break;
    default:
      XPT_PORT_DATA_OUT=((state!=FALSE) | sw);
      // strobe STROBE PIN
      XPT_PORT_STROBE_OUT|=XPT_PIN_STROBE;
      // bring low
      XPT_PORT_STROBE_OUT&=(uint8_t)~XPT_PIN_STROBE;
      break;
  }
}

inline void reset_shift_override(uint8_t sw, uint8_t state) {
  // if it is not a repeat, not a meta key, then reset.
  if(shift_override 
     && ((sw != POLL_C64_KEY_LSHIFT  // if it is a shift, keep override
          && sw != POLL_C64_KEY_RSHIFT
          && sw != POLL_C64_KEY_CTRL    // if it is ctrl or cbm, keep.
          && sw != POLL_C64_KEY_CBM
          && sw != shift_override_key // if it is a repeat, keep
         )
         || (sw == shift_override_key && !state)
        )
     ) {
    // turn off key.
    set_switch(shift_override_key,FALSE);
    set_switch(POLL_C64_KEY_LSHIFT,meta & POLL_FLAG_LSHIFT);
    set_switch(POLL_C64_KEY_RSHIFT,meta & POLL_FLAG_RSHIFT);
    shift_override=FALSE;
  }
}

void set_matrix(uint8_t sh, uint8_t sw, uint8_t state) {
  // sh is intended shift state.
  // sw is matrix code
  // state is TRUE for key down, false for key up.
  
  // reset override, if applicable.
  reset_shift_override(sw,state);
  if(sw==POLL_C64_KEY_LSHIFT) {
    //if(!state)
    //  shift&=(uint8_t)~POLL_FLAG_LSHIFT;
    //else
    //  shift|=(uint8_t)POLL_FLAG_LSHIFT;
    meta=(meta&(uint8_t)~POLL_FLAG_LSHIFT) | (state?POLL_FLAG_LSHIFT:0);
    set_switch(POLL_C64_KEY_LSHIFT,state);
  } else if(sw==POLL_C64_KEY_RSHIFT) {
    //if(!state)
    //  shift&=(uint8_t)~POLL_FLAG_RSHIFT;
    //else
    //  shift|=(uint8_t)POLL_FLAG_RSHIFT;
    meta=(meta&(uint8_t)~POLL_FLAG_RSHIFT) | (state?POLL_FLAG_RSHIFT:0);
    set_switch(POLL_C64_KEY_RSHIFT,state);
  } else if(state) {
    // key down
    if(!(shift_override && sw == shift_override_key)) {
      if(!(meta & POLL_FLAG_SHIFT) && sh) {
        //debug2('S');
        // we need to do a virtual shift.
        set_switch(POLL_C64_KEY_LSHIFT,TRUE);
        // set special key.
        shift_override=TRUE;
        shift_override_key=sw;
        shift_override_state=sh;
      } else if((meta&POLL_FLAG_SHIFT) && !sh) {
        //debug2('U');
        // we need to take off the shift keys...
        // set special key.
        set_switch(POLL_C64_KEY_LSHIFT,FALSE);
        set_switch(POLL_C64_KEY_RSHIFT,FALSE);
        shift_override=TRUE;
        shift_override_key=sw;
        shift_override_state=sh;
        delay_jiffy();
      }
      set_switch(sw,state);
    }
  } else if(((meta&POLL_FLAG_SHIFT) && sh) || (!(meta&POLL_FLAG_SHIFT) && !sh)) {
    // key up, and we were not in shift override...
    set_switch(sw,state);
  }
}

void reset_matrix(void) {
  uint8_t i=0;

  // reset switches...
  do {
    set_switch(i+=2,FALSE);
  } while (i!=0);
  // turn off all LEDs but NumLock
  led_state&=(PS2_LED_NUM_LOCK);

  // turn off all but 4080 
  sw_state&=(1<<SW_4080);
  // now, set 4080
  set_switch(POLL_C128_PKEY_4080,sw_state&(1<<SW_4080));
}

inline void set_LED(uint8_t led) {
  //debug2('L');
  led_state|=led;
  PS2_send(PS2_CMD_LEDS);
  PS2_send(led_state);
}

inline void clear_LED(uint8_t led) {
  led_state&=(uint8_t)~led;
  PS2_send(PS2_CMD_LEDS);
  PS2_send(led_state);
}

inline void toggle_num_lock(void) {
  if(led_state&PS2_LED_NUM_LOCK)
    clear_LED(PS2_LED_NUM_LOCK);
  else
    set_LED(PS2_LED_NUM_LOCK);
}

void toggle_4080_key(uint8_t state) {
  if(state) {
    if(sw_state&(1<<SW_4080)) {
      // turn it off
      set_switch(POLL_C128_PKEY_4080,FALSE);
      sw_state&=(uint8_t)~(1<<SW_4080);
    } else {
      // turn it on
      set_switch(POLL_C128_PKEY_4080,TRUE);
      sw_state|=(1<<SW_4080);
    }
  }
}
void toggle_caps_lock_key(uint8_t state) {
  if(state) {
    if(sw_state&(1<<SW_CAPSENSE)) {
      // turn it off
      set_switch(POLL_C128_PKEY_CAPSENSE,FALSE);
      sw_state&=(uint8_t)~(1<<SW_CAPSENSE);
      clear_LED(PS2_LED_CAPS_LOCK);
    } else {
      // turn it on
      set_switch(POLL_C128_PKEY_CAPSENSE,TRUE);
      sw_state|=(1<<SW_CAPSENSE);
      set_LED(PS2_LED_CAPS_LOCK);
    }
  }
}

inline void init_keyboard(void) {
  meta=0;
  config=FALSE;
  shift_override=FALSE;

  // need to set KB LEDs to match internal state
  PS2_send(PS2_CMD_LEDS);
  PS2_send(led_state);
}

void poll_init(void) {
  LED_init(LED_PIN_7);
  PIN_SET_HIZ(DDRD,PORTD,PIN5);
  XPT_PORT_STROBE_OUT&=(uint8_t)~XPT_PIN_STROBE;
  XPT_DDR_STROBE|=XPT_PIN_STROBE;
  XPT_DDR_DATA=0xff;
  SW_init(SW_TYPE_OUTPUT,(1<<SW_RESTORE) | (1<<SW_CAPSENSE) | (1<<SW_4080));

  // initially, load defaults from EEPROM
  while(!eeprom_is_ready());
  layout=eeprom_read_byte(POLL_ADDR_LAYOUT)%POLL_LAYOUT_NUM;
  while(!eeprom_is_ready());
  led_state=eeprom_read_byte(POLL_ADDR_LED_STATE);
  while(!eeprom_is_ready());
  sw_state=eeprom_read_byte(POLL_ADDR_SW_STATE)&(1<<SW_4080);

  reset_matrix();

  OCR2=POLL_IRQ_DIVIDER; 
  //TCNT2=0;
  // Set OC2 clk  to SYSCLK/1024 and Compare Timer Mode
  TCCR2 = (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21);
  // set up OC2 IRQ
  TIMSK |= (1<<OCIE2);
  //meta=POLL_FLAG_NONE;
  //config=FALSE;
  //shift_override=FALSE;
  
}

inline void map_positional_c64(uint8_t sh, uint8_t code, uint8_t state) {
  // TODO I really would like to get rid of these functions.
  switch(code) {
    case 0x80| PS2_KEY_PAUSE:
      set_matrix(sh,POLL_C64_PKEY_RESTORE,state);
      break;
    case PS2_KEY_CAPS_LOCK:
      // soft caps lock?
      break;
    case 0x80 | PS2_KEY_NUM_SLASH:
      set_matrix(FALSE,POLL_C64_KEY_SLASH,state);
      break;
    case 0x80 | PS2_KEY_INSERT:
      set_matrix(TRUE,POLL_C64_KEY_DELETE,state);
      break;
    case 0x80 | PS2_KEY_DELETE:
      set_matrix(FALSE,POLL_C64_KEY_DELETE,state);
      break;
    case 0x80 | PS2_KEY_HOME:
      set_matrix(sh,POLL_C64_KEY_HOME,state);
      break;
    case 0x80 | PS2_KEY_NUM_ENTER:
      set_matrix(sh,POLL_C64_KEY_RETURN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_DOWN:
      set_matrix(FALSE,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_UP:
      set_matrix(TRUE,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_RIGHT:
      set_matrix(FALSE,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_CRSR_LEFT:
      set_matrix(TRUE,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
    case PS2_KEY_ESC:
    case 0x80 | PS2_KEY_END:
      set_matrix(sh,POLL_C64_KEY_RUNSTOP,state);
      break;
    case 0x80 | PS2_KEY_RCTRL:
      set_matrix(sh,POLL_C64_KEY_CTRL,state);
      break;
    case 0x80 | PS2_KEY_RALT:
      set_matrix(sh,POLL_C64_KEY_CBM,state);
      break;
    case 0x80 | PS2_KEY_PAGE_UP:
      set_matrix(sh,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_PAGE_DOWN:
      set_matrix(sh,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
  }
}

inline void map_positional_c128(uint8_t sh, uint8_t code, uint8_t state) {
  // TODO I really would like to get rid of these functions.
  switch(code) {
    case 0x80| PS2_KEY_PAUSE:
      set_matrix(sh,POLL_C64_PKEY_RESTORE,state);
      break;
    case 0x80 | PS2_KEY_NUM_SLASH:
      set_matrix(FALSE,POLL_C64_KEY_SLASH,state);
      break;
    case 0x80 | PS2_KEY_INSERT:
      set_matrix(TRUE,POLL_C64_KEY_DELETE,state);
      break;
    case 0x80 | PS2_KEY_DELETE:
      set_matrix(FALSE,POLL_C64_KEY_DELETE,state);
      break;
    case 0x80 | PS2_KEY_HOME:
      set_matrix(sh,POLL_C64_KEY_HOME,state);
      break;
    case 0x80 | PS2_KEY_END:
      set_matrix(sh,POLL_C64_KEY_RUNSTOP,state);
      break;
    case 0x80 | PS2_KEY_RCTRL:
      set_matrix(sh,POLL_C64_KEY_CTRL,state);
      break;
    // changed stuff
    case 0x80 | PS2_KEY_CRSR_DOWN:
      set_matrix(sh,POLL_C128_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_UP:
      set_matrix(sh,POLL_C128_KEY_CRSR_UP,state);
      break;
    case 0x80 | PS2_KEY_CRSR_RIGHT:
      set_matrix(sh,POLL_C128_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_CRSR_LEFT:
      set_matrix(sh,POLL_C128_KEY_CRSR_LEFT,state);
      break;
    case 0x80 | PS2_KEY_RALT:
      set_matrix(sh,POLL_C128_KEY_ALT,state);
      break;
    case PS2_KEY_CAPS_LOCK:
      toggle_caps_lock_key(state);
      break;
    case 0x80 | PS2_KEY_NUM_ENTER:
      set_matrix(sh,POLL_C128_KEY_ENTER,state);
      break;
    // new stuff.
    case 0x80 | PS2_KEY_PAGE_UP:
      set_matrix(sh,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_PAGE_DOWN:
      set_matrix(sh,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_PRINT_SCREEN:
      if(sh) {
        toggle_4080_key(state);
      } else
        set_matrix(FALSE,POLL_C128_KEY_HELP,state);
      break;
  }
}

inline void map_symbolic_c64(uint8_t sh, uint8_t code, uint8_t state) {
  // TODO I really would like to get rid of these functions.
  switch(code) {
    case 0x80| PS2_KEY_PAUSE:
      //set_matrix(sh,C64_PKEY_RESTORE,state);
      break;
    case 0x80 | PS2_KEY_NUM_SLASH:
      set_matrix(FALSE,POLL_C64_KEY_SLASH,state);
      break;
    case 0x80 | PS2_KEY_INSERT:
      set_matrix(TRUE,POLL_C64_KEY_POUND,state);
      break;
    case 0x80 | PS2_KEY_DELETE:
      set_matrix(FALSE,POLL_C64_KEY_UPARROW,state);
      break;
    case 0x80 | PS2_KEY_HOME:
      set_matrix(sh,POLL_C64_KEY_HOME,state);
      break;
    case 0x80 | PS2_KEY_NUM_ENTER:
      set_matrix(sh,POLL_C64_KEY_RETURN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_DOWN:
      set_matrix(FALSE,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_UP:
      set_matrix(TRUE,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_RIGHT:
      set_matrix(FALSE,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_CRSR_LEFT:
      set_matrix(TRUE,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_END:
      //set_matrix(sh,POLL_C64_KEY_RUNSTOP,state);
      break;
    case 0x80 | PS2_KEY_RCTRL:
      // joystick fire...
      //set_matrix(sh,POLL_C64_KEY_CTRL,state);
      break;
    // new stuff.
    case 0x80 | PS2_KEY_PAGE_UP:
      set_matrix(sh,POLL_C64_PKEY_RESTORE,state);
      break;
    case 0x80 | PS2_KEY_PAGE_DOWN:
      //set_matrix(sh,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
   }
}

inline void map_symbolic_c128(uint8_t sh, uint8_t code, uint8_t state) {
  // TODO I really would like to get rid of these functions.
  switch(code) {
    case 0x80| PS2_KEY_PAUSE:
      set_matrix(sh,POLL_C64_PKEY_RESTORE,state);
      break;
    case 0x80 | PS2_KEY_NUM_SLASH:
      set_matrix(FALSE,POLL_C64_KEY_SLASH,state);
      break;
    case 0x80 | PS2_KEY_INSERT:
      set_matrix(TRUE,POLL_C64_KEY_DELETE,state);
      break;
    case 0x80 | PS2_KEY_DELETE:
      set_matrix(FALSE,POLL_C64_KEY_DELETE,state);
      break;
    case 0x80 | PS2_KEY_HOME:
      set_matrix(sh,POLL_C64_KEY_HOME,state);
      break;
    case 0x80 | PS2_KEY_END:
      set_matrix(sh,POLL_C64_KEY_RUNSTOP,state);
      break;
    case 0x80 | PS2_KEY_RCTRL:
      set_matrix(sh,POLL_C64_KEY_CTRL,state);
      break;
    // changed stuff
    case 0x80 | PS2_KEY_CRSR_DOWN:
      set_matrix(sh,POLL_C128_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_CRSR_UP:
      set_matrix(sh,POLL_C128_KEY_CRSR_UP,state);
      break;
    case 0x80 | PS2_KEY_CRSR_RIGHT:
      set_matrix(sh,POLL_C128_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_CRSR_LEFT:
      set_matrix(sh,POLL_C128_KEY_CRSR_LEFT,state);
      break;
    case 0x80 | PS2_KEY_RALT:
      set_matrix(sh,POLL_C128_KEY_ALT,state);
      break;
    case PS2_KEY_CAPS_LOCK:
      toggle_caps_lock_key(state);
      break;
    case 0x80 | PS2_KEY_NUM_ENTER:
      set_matrix(sh,POLL_C128_KEY_ENTER,state);
      break;
    // new stuff.
    case 0x80 | PS2_KEY_PAGE_UP:
      set_matrix(sh,POLL_C64_KEY_CRSR_DOWN,state);
      break;
    case 0x80 | PS2_KEY_PAGE_DOWN:
      set_matrix(sh,POLL_C64_KEY_CRSR_RIGHT,state);
      break;
    case 0x80 | PS2_KEY_PRINT_SCREEN:
      if(sh)
        toggle_4080_key(state);
      else
        set_matrix(FALSE,POLL_C128_KEY_HELP,state);
      break;
  }
}

inline void remap_personal(uint8_t map, uint8_t* shift, uint8_t* code) {
  // this is where we'd put the user's overrides...
}

inline void remap_keypad(uint8_t* sh,uint8_t* code) {
  if(!(led_state & PS2_LED_NUM_LOCK)) {
    // mappings for cursor and paging keys.
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
        // Switch into the extended key versions of the codes.
        *code |= 0x80;
        break;
    }
  }
}

inline void map_key(uint8_t sh, uint8_t code,uint8_t state) {
  uint8_t map=POLL_CBM_KEY_UNMAPPED;
  
  if(!state && code==PS2_KEY_NUM_LOCK) {
      // handle NUM_LOCK pressed.
      toggle_num_lock();
      // turn off trigger for extended keys.
      map=0;
  } else if(code<=PS2_KEY_F7) {
    // look it up.
    map=pgm_read_byte(&normal[code]);
    if(map<POLL_CBM_KEY_SPECIAL) {
      set_matrix(sh,map,state);
    } else if(map<POLL_CBM_KEY_UNMAPPED) {
      // special key.
      map=pgm_read_byte(&layouts[layout][map-POLL_CBM_KEY_SPECIAL][(meta&POLL_FLAG_SHIFT)!=0]);
      if(map<POLL_CBM_KEY_UNMAPPED)
        set_matrix(map&0x01,map&0xfe,state);
    }
  }
  if(map==POLL_CBM_KEY_UNMAPPED) {
    // extended key.
    // map to layout.
    switch(layout) {
      case POLL_LAYOUT_POSITIONAL_C64:
        map_positional_c64(sh,code,state);
        break;
      case POLL_LAYOUT_POSITIONAL_C128:
        map_positional_c128(sh,code,state);
        break;
      case POLL_LAYOUT_SYMBOLIC_C64:
        map_symbolic_c64(sh,code,state);
        break;
      case POLL_LAYOUT_SYMBOLIC_C128:
        map_symbolic_c128(sh,code,state);
        break;
    }
  }
}

inline void set_options(uint8_t code, uint8_t state) {
  if(state) {
    switch(code) {
      case PS2_KEY_1:
        layout=0;
        LED_blink(LED_PIN_7,1,LED_FLAG_NONE);
        break;
      case PS2_KEY_2:
        layout=1;
        LED_blink(LED_PIN_7,2,LED_FLAG_NONE);
        break;
      case PS2_KEY_3:
        layout=2;
        LED_blink(LED_PIN_7,3,LED_FLAG_NONE);
        break;
      case PS2_KEY_4:
        layout=3;
        LED_blink(LED_PIN_7,4,LED_FLAG_NONE);
        break;
      case PS2_KEY_EQUALS:
        debug=!debug;
        PS2_set_debug(debug);
        break;
      case PS2_KEY_NUM_LOCK:
        toggle_num_lock();
        break;
        
    }
  }
}

/* 
 * In this function, all keys are mapped to single char.  Relationship:
 * 
 * normal = 0x00 - 0x84
 * ext    = 0x80 - 0xfd  (might be an issue if there is e0 (80-84)
 */
 
void poll_parse_key(uint8_t code, uint8_t state) {
  uint8_t sh=meta&POLL_FLAG_SHIFT;
  
  // this is also where we can check for Ctrl/Alt/Del
  if((code&0x7f)==PS2_KEY_ALT) {
    // turn on or off the ALT META flag
    meta=(meta&(uint8_t)~POLL_FLAG_ALT) | (state?POLL_FLAG_ALT:0);
  } else if((code&0x7f)==PS2_KEY_LCTRL) {
    // turn on or off the CTRL META flag
    meta=(meta&(uint8_t)~POLL_FLAG_CONTROL) | (state?POLL_FLAG_CONTROL:0);
  } 
  if(!config && (meta&POLL_FLAG_CTRL_ALT)==POLL_FLAG_CTRL_ALT && code==(0x80 | PS2_KEY_DELETE) && state) {
    // CTRL/ALT/DEL is pressed.
    // bring RESET line low
    //debug2('^');
    // repeat this a few times so the pulse will be long enough to trigger the NMOS ICs.
    PIN_SET_LOW(DDRD,PORTD,PIN5);
    PIN_SET_LOW(DDRD,PORTD,PIN5);
    PIN_SET_LOW(DDRD,PORTD,PIN5);
    PIN_SET_HIZ(DDRD,PORTD,PIN5);
    //goto *0x0000;
  } else if((meta&POLL_FLAG_CTRL_ALT)==POLL_FLAG_CTRL_ALT && code==PS2_KEY_BS) {
    // CTRL/ALT/Backspace.
    if(!state) { // only check on key up.
      if(!config) {
        // go into config mode
        LED_blink(LED_PIN_7,10,LED_FLAG_NONE);
        reset_matrix();
      } else {
        LED_blink(LED_PIN_7,10,LED_FLAG_END_ON);
        reset_matrix();
        // write layout to EEPROM
        update_eeprom(POLL_ADDR_LAYOUT,layout);
        // update NUM LOCK initial state.
        update_eeprom(POLL_ADDR_LED_STATE,led_state&PS2_LED_NUM_LOCK);
        // update 40/80 column switch
        update_eeprom(POLL_ADDR_SW_STATE,sw_state&(1<<SW_4080));
      }
      config=!config;
    }
  } else if (config) {
    // do config operations.
    set_options(code,state);
  } else {
    // now, apply user preferences
    remap_personal(layout,&sh,&code);
    // handle num_lock/joystick...
    remap_keypad(&sh,&code);
    // do we still have a code, or did joystick driver EAT it?
    if(code) {
      // map key.
      map_key(sh,code,state);
    }
  }
}

// This function, normalizes the PS2 keycodes in
void poll(void) {
  uint8_t key;
  LED_blink(LED_PIN_7,layout+1,LED_FLAG_END_ON);
  
  for(;;) {
    if(PS2_data_available() != 0) {
      // kb sent data...
      key=PS2_recv();
      if(key==PS2_CMD_BAT) {
        reset_matrix();
        init_keyboard();
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
              // we got an E1
              state=POLL_ST_GET_PAUSE_1;
              // start on the Pause/Break sequence.
              break;
            case PS2_CMD_ACK:
            case PS2_CMD_ECHO:
            case PS2_CMD_ERROR:
            case PS2_CMD_OVERFLOW:
              break;
            default:
              poll_parse_key(key,TRUE);
              state=POLL_ST_IDLE;
              break;
          }
          break;
        case POLL_ST_GET_KEY_UP:
          poll_parse_key(key,FALSE);
          state=POLL_ST_IDLE;
          break;
        case POLL_ST_GET_X_KEY:
          if(key==PS2_KEY_UP) {
            state=POLL_ST_GET_X_KEY_UP;
          } else if(key==PS2_KEY_LSHIFT) {
            // when NumLock is pressed, INS and DEL prepend with EO 12 (extended shift), but we don't care, so eat code.
            // Also, when PrintScreen is pressed, it too sends an E0 12
            state=POLL_ST_IDLE;
          } else {
            poll_parse_key(0x80 | key,TRUE);
            state=POLL_ST_IDLE;
          }
          break;
        case POLL_ST_GET_X_KEY_UP:
          if(key==PS2_KEY_LSHIFT) {
            // when NumLock is pressed, INS and DEL prepend with EO 12 (extended shift), but we don't care, so eat code.
            // Also, when PrintScreen is pressed, it too sends an E0 12
            state=POLL_ST_IDLE;
          } else {
            poll_parse_key(0x80 | key,FALSE);
            state=POLL_ST_IDLE;
          }
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
          // we got 77
          if(key==PS2_KEY_PAUSE) {
            //debug('R');
            // we received a complete Pause/Break, do something about it.
            poll_parse_key(0x80|PS2_KEY_PAUSE,TRUE);
            delay_jiffy();
            poll_parse_key(0x80|PS2_KEY_PAUSE,FALSE);
          }
          state=POLL_ST_IDLE;
          break;
          
      }
    }
  }
}
