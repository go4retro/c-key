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
#include "kb.h"
#include "ps2.h"
#include "util.h"
#include "led.h"
#include "scanner.h"

#define JOY_NONE  0
#define JOY_UP    1
#define JOY_DOWN  2
#define JOY_LEFT  4
#define JOY_RIGHT 8
#define JOY_FIRE  16

#define KEY_UP    0
#define KEY_DOWN  1
#define KEY_HIT   2

#define SCAN_ESC_NONE   0
#define SCAN_ESC_1      1
#define SCAN_ESC_2      2
#define SCAN_ESC_3      4
#define SCAN_ESC        7

static unsigned char regular[0x48] =  {0xc0,PS2_KEY_Q,0xcf,PS2_KEY_SPACE,0xd0,0xcd,0xce,PS2_KEY_1
                                        ,PS2_KEY_4,PS2_KEY_E,PS2_KEY_S,PS2_KEY_Z,PS2_KEY_LSHIFT,PS2_KEY_A,PS2_KEY_W,PS2_KEY_3
                                        ,0xc1,PS2_KEY_T,PS2_KEY_F,PS2_KEY_C,PS2_KEY_X,PS2_KEY_D,PS2_KEY_R,PS2_KEY_5
                                        ,0xc3,PS2_KEY_U,PS2_KEY_H,PS2_KEY_B,PS2_KEY_V,PS2_KEY_G,PS2_KEY_Y,0xc2
                                        ,0xc5,PS2_KEY_O,PS2_KEY_K,PS2_KEY_M,PS2_KEY_N,PS2_KEY_J,PS2_KEY_I,0xc4
                                        ,PS2_KEY_MINUS,0xc7,0xc9,PS2_KEY_PERIOD,PS2_KEY_COMMA,PS2_KEY_L,PS2_KEY_P,0xc6
                                        ,0xd2,0xcc,0xcb,PS2_KEY_LSHIFT,PS2_KEY_SLASH,0xca,0xc8,0xff
                                        ,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,PS2_KEY_ENTER,PS2_KEY_BS
                                        ,0xd1,0xff,0xff,0xff,0xff,0xff,0xff,0xff
                                        };
                                        
static unsigned char mappings[2][25][2][2] = {
                                          // default mappings
                                         {
                                          {{0,PS2_KEY_2},{SCAN_MAP_SHIFT,PS2_KEY_APOSTROPHE}},  //0xc0
                                          {{0,PS2_KEY_6},{SCAN_MAP_SHIFT,PS2_KEY_7}},           //0xc1
                                          {{0,PS2_KEY_7},{0,PS2_KEY_APOSTROPHE}},           //0xc2
                                          {{0,PS2_KEY_8},{SCAN_MAP_SHIFT,PS2_KEY_9}},           //0xc3
                                          {{0,PS2_KEY_9},{SCAN_MAP_SHIFT,PS2_KEY_0}},           //0xc4
                                          {{0,PS2_KEY_0},{0,0}},  // shift-0?           //0xc5
                                          {{SCAN_MAP_SHIFT,PS2_KEY_EQUALS},{0,0}}, // shift+  0xc6
                                          {{SCAN_MAP_SHIFT,PS2_KEY_2},{0,0}}, // shift @?     0xc7
                                          {{SCAN_MAP_SHIFT,PS2_KEY_8},{0,0}}, // shift *?     0xc8
                                          {{SCAN_MAP_SHIFT,PS2_KEY_SEMICOLON},{0,PS2_KEY_LBRACKET}},  // 0xc9
                                          {{0,PS2_KEY_SEMICOLON},{0,PS2_KEY_RBRACKET}},     //0xca
                                          {{0,PS2_KEY_EQUALS},{0,0}}, // shift =?         0xcb
                                          {{SCAN_MAP_SHIFT,PS2_KEY_6},{0,0}}, // shift ^?     0xcc
                                          {{0,PS2_KEY_LCTRL},{SCAN_MAP_SHIFT,PS2_KEY_LCTRL}}, // control
                                          {{0,PS2_KEY_ESC},{SCAN_MAP_SHIFT,PS2_KEY_ESC}}, // <-
                                          {{0,PS2_KEY_ALT},{SCAN_MAP_SHIFT,PS2_KEY_ALT}}, // C=
                                          {{0,PS2_KEY_TAB},{SCAN_MAP_SHIFT,PS2_KEY_TAB}}, // RUN-STOP
                                          {{SCAN_MAP_EXT,PS2_KEY_DEL},{SCAN_MAP_EXT | SCAN_MAP_SHIFT,PS2_KEY_DEL}},// RESTORE
                                          {{SCAN_MAP_EXT,PS2_KEY_HOME},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_HOME}}, // CLR/HOME
                                          {{0,PS2_KEY_F7},{0,PS2_KEY_F8}},                      // f7
                                          {{0,PS2_KEY_F5},{0,PS2_KEY_F6}},                      // f5
                                          {{0,PS2_KEY_F3},{0,PS2_KEY_F4}},                      // f3
                                          {{0,PS2_KEY_F1},{0,PS2_KEY_F2}},                      // f1
                                          {{0,PS2_KEY_CRSR_DOWN},{0,PS2_KEY_CRSR_UP}},          // CRSR DOWN
                                          {{0,PS2_KEY_CRSR_RIGHT},{0,PS2_KEY_CRSR_LEFT}},       // CRSR RIGHT
                                         },
                                         // VICE mappins
                                         {
                                          {{0,PS2_KEY_2},{SCAN_MAP_SHIFT,PS2_KEY_2}},                 //2
                                          {{0,PS2_KEY_6},{SCAN_MAP_SHIFT,PS2_KEY_6}},                 //6
                                          {{0,PS2_KEY_7},{SCAN_MAP_SHIFT,PS2_KEY_7}},                 //7
                                          {{0,PS2_KEY_8},{SCAN_MAP_SHIFT,PS2_KEY_8}},                 //8
                                          {{0,PS2_KEY_9},{SCAN_MAP_SHIFT,PS2_KEY_9}},                 //9
                                          {{0,PS2_KEY_0},{SCAN_MAP_SHIFT,PS2_KEY_0}},                 //0
                                          {{0,PS2_KEY_MINUS},{SCAN_MAP_SHIFT,PS2_KEY_MINUS}},         //+
                                          {{0,PS2_KEY_LBRACKET},{SCAN_MAP_SHIFT,PS2_KEY_LBRACKET}},   //@
                                          {{0,PS2_KEY_RBRACKET},{SCAN_MAP_SHIFT,PS2_KEY_RBRACKET}},   //*
                                          {{0,PS2_KEY_SEMICOLON},{SCAN_MAP_SHIFT,PS2_KEY_SEMICOLON}}, //:
                                          {{0,PS2_KEY_RBRACKET},{SCAN_MAP_SHIFT,PS2_KEY_RBRACKET}},  //;
                                          {{0,PS2_KEY_BACKSLASH},{0,PS2_KEY_BACKSLASH}}, //=
                                          {{SCAN_MAP_EXT,PS2_KEY_DEL},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_DEL}},//^ = DEL
                                          {{0,PS2_KEY_TAB},{SCAN_MAP_SHIFT,PS2_KEY_TAB}},             //CTRL
                                          {{0,PS2_KEY_BACKQUOTE},{SCAN_MAP_SHIFT,PS2_KEY_BACKQUOTE}}, //<-
                                          {{0,PS2_KEY_LCTRL},{SCAN_MAP_SHIFT,PS2_KEY_LCTRL}},         // C=
                                          {{0,PS2_KEY_ESC},{SCAN_MAP_SHIFT,PS2_KEY_ESC}},              // RUN-STOP
                                          {{SCAN_MAP_EXT,PS2_KEY_DEL},{SCAN_MAP_EXT | SCAN_MAP_SHIFT,PS2_KEY_DEL}},// RESTORE
                                          {{SCAN_MAP_EXT,PS2_KEY_HOME},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_HOME}}, // CLR/HOME
                                          {{0,PS2_KEY_F7},{0,PS2_KEY_F8}},                      // f7
                                          {{0,PS2_KEY_F5},{0,PS2_KEY_F6}},                      // f5
                                          {{0,PS2_KEY_F3},{0,PS2_KEY_F4}},                      // f3
                                          {{0,PS2_KEY_F1},{0,PS2_KEY_F2}},                      // f1
                                          {{0,PS2_KEY_CRSR_DOWN},{0,PS2_KEY_CRSR_UP}},          // CRSR DOWN
                                          {{0,PS2_KEY_CRSR_RIGHT},{0,PS2_KEY_CRSR_LEFT}},       // CRSR RIGHT
                                         }
                                        };
                                        
static unsigned char joy_table[5]= {JOY_UP,JOY_DOWN,JOY_LEFT,JOY_RIGHT,JOY_FIRE};
static unsigned char joy_mapping[2][9][2]= {
                                               {
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_8},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_7},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_4},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_1},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_2},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_3},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_6},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_9},
                                                 {SCAN_MAP_EXT,PS2_KEY_RCTRL}
                                                },
                                               {
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_8},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_7},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_4},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_1},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_2},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_3},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_6},
                                                 {SCAN_MAP_EXT,PS2_KEY_NUM_9},
                                                 {SCAN_MAP_EXT,PS2_KEY_RCTRL}
                                                }
                                               };
                                          
static volatile unsigned char flags;
static volatile unsigned char mapping;
static volatile unsigned char joy0;
static volatile unsigned char joy1;

void handle_cmds(void) {
  unsigned char data;
  
  while(PS2_data_available() != 0) {
    data=PS2_recv();
    PS2_handle_cmds(data);
    switch(data) {
      case PS2_CMD_SET_RATE:
        PS2_send(PS2_CMD_ACK);
        data=PS2_recv();
        KB_set_repeat_delay(PS2_get_typematic_delay(data));
        KB_set_repeat_period(PS2_get_typematic_period(data));
        break;
    }
  }
}

void send_key_code(unsigned char keydef[2],unsigned char action) {
  if(keydef[1] != 0) {
    // we have a valid key
    if((flags & SCAN_FLAG_SHIFT) == SCAN_FLAG_SHIFT && (keydef[0] & SCAN_MAP_SHIFT) == 0) {
      // we are in shift mode and this key needs shift off.
      PS2_send(PS2_KEY_UP);
      PS2_send(PS2_KEY_LSHIFT);
    } else if((flags & SCAN_FLAG_SHIFT) == 0 && (keydef[0] & SCAN_MAP_SHIFT) == SCAN_MAP_SHIFT) {
      // we are not in shift mode and we need a shift.
      PS2_send(PS2_KEY_LSHIFT);
    }
    if((keydef[0] & SCAN_MAP_EXT) == SCAN_MAP_EXT)
      PS2_send(PS2_KEY_EXT);
    switch(action) {
      case KEY_HIT:
        PS2_send(keydef[1]);
        if((keydef[0] & SCAN_MAP_EXT) == SCAN_MAP_EXT)
          PS2_send(PS2_KEY_EXT);
      case KEY_UP:
        PS2_send(PS2_KEY_UP);
      case KEY_DOWN:
        PS2_send(keydef[1]);
        break;
    }
    if((flags & SCAN_FLAG_SHIFT) == SCAN_FLAG_SHIFT && (keydef[0] & SCAN_MAP_SHIFT) == 0) {
      // need to get back to shift mode
      PS2_send(PS2_KEY_LSHIFT);
    } else if((flags & SCAN_FLAG_SHIFT) == 0 && (keydef[0] & SCAN_MAP_SHIFT) == SCAN_MAP_SHIFT) {
      // let off shift key
      PS2_send(PS2_KEY_UP);
      PS2_send(PS2_KEY_LSHIFT);
    }
  }
}
  
unsigned char get_joy_direction(unsigned char *joy) {
  unsigned char i=9;
  switch (*joy & 0x0f) {
    case JOY_UP:              // 0
      i=0;
      break;
    case JOY_UP | JOY_LEFT:   // 1
      i=1;
      break;
    case JOY_LEFT:            // 2
      i=2;
      break;
    case JOY_DOWN | JOY_LEFT: // 3
      i=3;
      break;
    case JOY_DOWN:            // 4
      i=4;
      break;
    case JOY_DOWN | JOY_RIGHT:// 5
      i=5;
      break;
    case JOY_RIGHT:           // 6
      i=6;
      break;
    case JOY_RIGHT | JOY_UP:  // 7
      i=7;
      break;
  }
  return i;
}

void do_joy(unsigned char *joy, unsigned char data, unsigned char table[9][2]) {
  unsigned char i;
  if((data & 0x80) != 0) {
    if((data & 0x7) == 4) {
      // fire not pressed
      i=8;
    } else {
      // direction change
      i=get_joy_direction(joy);
    }
    if(i<9) {
      send_key_code(table[i],KEY_UP);
      KB_set_repeat_code(0);
    }
    (*joy)&=~(joy_table[data & 0x7]);
  } else {
    (*joy)|=(joy_table[data & 0x7]);
    if((data & 0x7) == 4) {
      // fire pressed
      i=8;
    } else {
      // direction change
      i=get_joy_direction(joy);
    }
    if(i<9) {
      send_key_code(table[i],KEY_DOWN);
      KB_set_repeat_code(data);
    }
  }
}

void parse_key(unsigned char data) {
  unsigned char key;
  
  if(data < 0x48) {
    key=regular[data];
    if(key < 0xc0) {
      PS2_send(key);
      PS2_send(PS2_KEY_UP);
      PS2_send(key);
    } else if(key != 0xff) {
      key&=0x3f;
      if((flags & SCAN_FLAG_SHIFT) == SCAN_FLAG_SHIFT) {
        // pressing a shifted char
        send_key_code(mappings[mapping][key][1],KEY_HIT);
      } else {
        send_key_code(mappings[mapping][key][0],KEY_HIT);
      }
    }
  }
  KB_set_repeat_code(data);
}

void scan(void) {
  unsigned char data;
  unsigned char esc_flags=SCAN_ESC_NONE;
 
  led_on(LED_PIN_7);
  flags=SCAN_FLAG_NONE;
  mapping=0;
  joy0=JOY_NONE;
  joy1=JOY_NONE;
  
  for(;;) {
    handle_cmds();
    data = KB_recv();
    debug2(data);
    if(esc_flags == SCAN_ESC) {
      switch(data) {
        case 0x07: //1
          // regular
          mapping=0;
          led_blink(1,LED_PIN_7);
          break;
        case 0x00: //2
          // VICE
          mapping=1;
          led_blink(2,LED_PIN_7);
          break;
        case 0x3e: // enter
          esc_flags=SCAN_ESC_NONE;
          led_blink(10,LED_PIN_7);
          led_on(LED_PIN_7);
          break;
      }
    } else {
      switch(data) {
        case 0x33:  // right shift
          esc_flags|=SCAN_ESC_1;
          flags|=SCAN_FLAG_SHIFT;
          PS2_send(PS2_KEY_LSHIFT);
          KB_set_repeat_code(0);
          
          break;
        case 0x0c:  // left shift
          esc_flags|=SCAN_ESC_2;
          flags|=SCAN_FLAG_SHIFT;
          PS2_send(PS2_KEY_LSHIFT);
          KB_set_repeat_code(0);
          break;
        case 0xb3:
          esc_flags&=~SCAN_ESC_1;
          flags &= ~SCAN_FLAG_SHIFT;
          PS2_send(PS2_KEY_UP);
          PS2_send(PS2_KEY_LSHIFT);
          KB_set_repeat_code(0);
          break;
        case 0x8c:
          esc_flags&=~SCAN_ESC_2;
          flags &= ~SCAN_FLAG_SHIFT;
          PS2_send(PS2_KEY_UP);
          PS2_send(PS2_KEY_LSHIFT);
          KB_set_repeat_code(0);
          break;
        case 0x02:  // commodore down
          esc_flags|=SCAN_ESC_3;
          flags|=SCAN_FLAG_CMDR;
          PS2_send(PS2_KEY_ALT);
          KB_set_repeat_code(0);
          break;
        case 0x82:  // commodore up
          esc_flags&=~SCAN_ESC_3;
          flags&=~SCAN_FLAG_CMDR;
          PS2_send(PS2_KEY_UP);
          PS2_send(PS2_KEY_ALT);
          KB_set_repeat_code(0);
          break;
        case 0x05:  // control down
          flags|=SCAN_FLAG_CTRL;
          PS2_send(PS2_KEY_LCTRL);
          KB_set_repeat_code(0);
          break;
        case 0x85:  // control up
          flags&=~SCAN_FLAG_CTRL;
          PS2_send(PS2_KEY_UP);
          PS2_send(PS2_KEY_LCTRL);
          KB_set_repeat_code(0);
          break;
        default:
          if((data & 0x7f) > 0x47 && (data & 0x7f) < 0x4d) {
            // joy0
            do_joy(&joy0,data,joy_mapping[0]);
          } else if((data & 0x7f) > 0x4f && (data & 0x7f) < 0x55) {
            // joy1
            do_joy(&joy1,data,joy_mapping[1]);
          } else if((data & 0x80) == 0) {
            parse_key(data);
          } else if((data & 0x7f) == KB_get_repeat_code()) {
            KB_set_repeat_code(0);
          }
          break;
      }
      if(esc_flags == SCAN_ESC) {
        // turn off ALT and SHIFT...
        PS2_send(PS2_KEY_UP);
        PS2_send(PS2_KEY_ALT);
        PS2_send(PS2_KEY_UP);
        PS2_send(PS2_KEY_LSHIFT);
        // note cmd mode.
        led_blink(2,LED_PIN_7);
      }
    }
  }
}
