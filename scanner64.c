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
#include "kb.h"
#include "switches.h"
#include "ps2.h"
#include "util.h"
#include "led.h"
#include "scanner.h"
#include "scanner64.h"

static prog_uint8_t regular[0x40] =  {0xc0,PS2_KEY_Q,0xcf,PS2_KEY_SPACE,0xd0,0xcd,0xce,PS2_KEY_1
                                        ,PS2_KEY_4,PS2_KEY_E,PS2_KEY_S,PS2_KEY_Z,PS2_KEY_LSHIFT,PS2_KEY_A,PS2_KEY_W,PS2_KEY_3
                                        ,0xc1,PS2_KEY_T,PS2_KEY_F,PS2_KEY_C,PS2_KEY_X,PS2_KEY_D,PS2_KEY_R,PS2_KEY_5
                                        ,0xc3,PS2_KEY_U,PS2_KEY_H,PS2_KEY_B,PS2_KEY_V,PS2_KEY_G,PS2_KEY_Y,0xc2
                                        ,0xc5,PS2_KEY_O,PS2_KEY_K,PS2_KEY_M,PS2_KEY_N,PS2_KEY_J,PS2_KEY_I,0xc4
                                        ,PS2_KEY_MINUS,0xc7,0xc9,PS2_KEY_PERIOD,PS2_KEY_COMMA,PS2_KEY_L,PS2_KEY_P,0xc6
                                        ,0xd1,0xcc,0xcb,PS2_KEY_RSHIFT,PS2_KEY_SLASH,0xca,0xc8,0x5d
                                        ,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,PS2_KEY_ENTER,PS2_KEY_BS
                                        };
                                        
static prog_uint8_t mappings[2][25][2][2] = {
                                          // default mappings
                                         {
                                          {{0,PS2_KEY_2},{SCAN_MAP_SHIFT,PS2_KEY_APOSTROPHE}},  //0xc0
                                          {{0,PS2_KEY_6},{SCAN_MAP_SHIFT,PS2_KEY_7}},           //0xc1
                                          {{0,PS2_KEY_7},{0,PS2_KEY_APOSTROPHE}},           //0xc2
                                          {{0,PS2_KEY_8},{SCAN_MAP_SHIFT,PS2_KEY_9}},           //0xc3
                                          {{0,PS2_KEY_9},{SCAN_MAP_SHIFT,PS2_KEY_0}},           //0xc4
                                          {{0,PS2_KEY_0},{0,0}},  // shift-0?           //0xc5
                                          {{SCAN_MAP_SHIFT,PS2_KEY_EQUALS},{SCAN_MAP_SHIFT,PS2_KEY_PAGE_UP}}, // shift+  0xc6
                                          {{SCAN_MAP_SHIFT,PS2_KEY_2},{SCAN_MAP_EXT,PS2_KEY_PAGE_UP}}, // shift @?     0xc7
                                          {{SCAN_MAP_SHIFT,PS2_KEY_8},{SCAN_MAP_EXT,PS2_KEY_PAGE_DOWN}}, // shift *?     0xc8
                                          {{SCAN_MAP_SHIFT,PS2_KEY_SEMICOLON},{0,PS2_KEY_LBRACKET}},  // 0xc9
                                          {{0,PS2_KEY_SEMICOLON},{0,PS2_KEY_RBRACKET}},     //0xca
                                          {{0,PS2_KEY_EQUALS},{0,0}}, // shift =?         0xcb
                                          {{SCAN_MAP_SHIFT,PS2_KEY_6},{SCAN_MAP_SHIFT,PS2_KEY_PAGE_DOWN}}, // shift ^?     0xcc
                                          {{0,PS2_KEY_LCTRL},{SCAN_MAP_SHIFT,PS2_KEY_LCTRL}}, // control
                                          {{0,PS2_KEY_BACKQUOTE},{SCAN_MAP_SHIFT,PS2_KEY_BACKQUOTE}}, //<-
                                          {{0,PS2_KEY_ALT},{SCAN_MAP_SHIFT,PS2_KEY_ALT}}, // C=
                                          {{0,PS2_KEY_ESC},{SCAN_MAP_SHIFT,PS2_KEY_ESC}}, // RUN-STOP
                                          {{SCAN_MAP_EXT,PS2_KEY_HOME},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_HOME}}, // CLR/HOME
                                          {{0,PS2_KEY_F7},{0,PS2_KEY_F8}},                      // f7
                                          {{0,PS2_KEY_F5},{0,PS2_KEY_F6}},                      // f5
                                          {{0,PS2_KEY_F3},{0,PS2_KEY_F4}},                      // f3
                                          {{0,PS2_KEY_F1},{0,PS2_KEY_F2}},                      // f1
                                          {{SCAN_MAP_EXT,PS2_KEY_CRSR_DOWN},{SCAN_MAP_EXT,PS2_KEY_CRSR_UP}},          // CRSR DOWN
                                          {{SCAN_MAP_EXT,PS2_KEY_CRSR_RIGHT},{SCAN_MAP_EXT,PS2_KEY_CRSR_LEFT}},       // CRSR RIGHT
                                         },
                                         // VICE mappings
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
                                          {{0,PS2_KEY_BACKSLASH},{SCAN_MAP_SHIFT,PS2_KEY_BACKSLASH}}, //=
                                          {{SCAN_MAP_EXT,PS2_KEY_DELETE},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_DELETE}},//^ = DEL
                                          {{0,PS2_KEY_TAB},{SCAN_MAP_SHIFT,PS2_KEY_TAB}},             //CTRL
                                          {{0,PS2_KEY_BACKQUOTE},{SCAN_MAP_SHIFT,PS2_KEY_BACKQUOTE}}, //<-
                                          {{0,PS2_KEY_LCTRL},{SCAN_MAP_SHIFT,PS2_KEY_LCTRL}},         // C=
                                          {{0,PS2_KEY_ESC},{SCAN_MAP_SHIFT,PS2_KEY_ESC}},              // RUN-STOP
                                          {{SCAN_MAP_EXT,PS2_KEY_HOME},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_HOME}}, // CLR/HOME
                                          {{0,PS2_KEY_F7},{0,PS2_KEY_F8}},                      // f7
                                          {{0,PS2_KEY_F5},{0,PS2_KEY_F6}},                      // f5
                                          {{0,PS2_KEY_F3},{0,PS2_KEY_F4}},                      // f3
                                          {{0,PS2_KEY_F1},{0,PS2_KEY_F2}},                      // f1
                                          {{0,PS2_KEY_CRSR_DOWN},{0,PS2_KEY_CRSR_UP}},          // CRSR DOWN
                                          {{0,PS2_KEY_CRSR_RIGHT},{0,PS2_KEY_CRSR_LEFT}},       // CRSR RIGHT
                                         }
                                        };
                                        
static prog_uint8_t joy_table[5]= {JOY_UP,JOY_DOWN,JOY_RIGHT,JOY_LEFT,JOY_FIRE};
static uint8_t joy_mapping[2][9][2]= {
                                               {
                                                 {SCAN_MAP_NONE,PS2_KEY_E},
                                                 {SCAN_MAP_NONE,PS2_KEY_W},
                                                 {SCAN_MAP_NONE,PS2_KEY_S},
                                                 {SCAN_MAP_NONE,PS2_KEY_X},
                                                 {SCAN_MAP_NONE,PS2_KEY_C},
                                                 {SCAN_MAP_NONE,PS2_KEY_V},
                                                 {SCAN_MAP_NONE,PS2_KEY_F},
                                                 {SCAN_MAP_NONE,PS2_KEY_R},
                                                 {SCAN_MAP_NONE,PS2_KEY_SPACE}
                                                },
                                               {
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_8},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_7},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_4},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_1},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_2},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_3},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_6},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_9},
                                                 {SCAN_MAP_NONE,PS2_KEY_NUM_0}
                                                }
                                               };
                                          
static volatile uint8_t flags;
static volatile uint8_t mapping;
static volatile uint8_t joy0;
static volatile uint8_t joy1;
static uint8_t shift_override=FALSE;
static uint8_t shift_override_state;
static uint8_t shift_override_key;

void scan_init(void) {
  // init keyboard matrix scanning engine
  KB_init();
  SW_init(SW_TYPE_INPUT,(1<<SW_RESTORE) | (1<<SW_CAPSENSE) | (1<<SW_4080));
  
  OCR2=31; //32 counts * 256 cycles/count * 16 times per run * 120 runs/sec  
  TCNT2=0;
  
  // Set OC2 clk  to SYSCLK/256 and Compare Timer Mode
  TCCR2 = (1<<CS22) | (1<<CS21) | (1<<WGM21);
  // set up OC2 IRQ
  TIMSK |= (1<<OCIE2);
}

SIGNAL(SIG_OUTPUT_COMPARE2) {
  KB_scan();
  SW_scan();
}


void handle_cmds(void) {
  uint8_t data;
  
  while(PS2_data_available()) {
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

void check_override(void) {
  if(shift_override) {
    if((shift_override_state & SCAN_MAP_EXT) == SCAN_MAP_EXT)
      PS2_send(PS2_KEY_EXT);
    PS2_send(PS2_KEY_UP);
    PS2_send(shift_override_key);
    
    if((shift_override_state & SCAN_MAP_SHIFT) == 0) {
      // we need to apply shifts now.
      if(flags&SCAN_FLAG_LSHIFT)
        PS2_send(PS2_KEY_LSHIFT);
      if(flags&SCAN_FLAG_RSHIFT)
        PS2_send(PS2_KEY_RSHIFT);
    } else {
      // let off shift key
      PS2_send(PS2_KEY_UP);
      PS2_send(PS2_KEY_LSHIFT);
    }
    shift_override=FALSE;
  }
}

void send_key_code(uint8_t state,uint8_t key,uint8_t action) {
  if(key != 0) {
    if(action==KEY_UP && shift_override && shift_override_key==key && shift_override_state==state) {
      // it's us, we're unpressing the key.
      check_override();
    } else {
      if(shift_override && (shift_override_key!=key || shift_override_state!=state)) {
        // we are a new keypress.
        check_override();
      }
      if(!shift_override) {
        // we have a new valid key
        if(action == KEY_DOWN && (flags & SCAN_FLAG_SHIFT) && (state & SCAN_MAP_SHIFT) == 0) {
          // we are in shift mode and this key needs shift off.
          if(flags&SCAN_FLAG_LSHIFT) {
            PS2_send(PS2_KEY_UP);
            PS2_send(PS2_KEY_LSHIFT);
          }
          if(flags&SCAN_FLAG_RSHIFT) {
            PS2_send(PS2_KEY_UP);
            PS2_send(PS2_KEY_RSHIFT);
          }
          shift_override=TRUE;
          shift_override_key=key;
          shift_override_state=state;
        } else if(action == KEY_DOWN && !(flags & SCAN_FLAG_SHIFT)&& (state & SCAN_MAP_SHIFT) == SCAN_MAP_SHIFT) {
          // we are not in shift mode and we need a shift.
          PS2_send(PS2_KEY_LSHIFT);
          shift_override=TRUE;
          shift_override_key=key;
          shift_override_state=state;
        }
      }
      if((state & SCAN_MAP_EXT) == SCAN_MAP_EXT)
        PS2_send(PS2_KEY_EXT);
      switch(action) {
        case KEY_UP:
          PS2_send(PS2_KEY_UP);
        case KEY_DOWN:
          PS2_send(key);
          break;
      }
    }
  }
}

uint8_t get_joy_direction(uint8_t *joy) {
  uint8_t i=9;
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

void do_joy(uint8_t *joy, uint8_t data, uint8_t table[9][2]) {
  uint8_t i;
  //printHex(data);
  if((data & 0x80) != 0) {
    if((data & 0x7) == 4) {
      // fire not pressed
      i=8;
    } else {
      // direction change
      i=get_joy_direction(joy);
    }
    if(i<9) {
      send_key_code(table[i][0],table[i][1],KEY_UP);
      KB_set_repeat_code(KB_NO_REPEAT);
    }
    (*joy)&=(uint8_t)~(pgm_read_byte(&joy_table[data & 0x7]));
  } else {
    (*joy)|=(pgm_read_byte(&joy_table[data & 0x7]));
    if((data & 0x7) == 4) {
      // fire pressed
      i=8;
    } else {
      // direction change
      i=get_joy_direction(joy);
    }
    if(i<9) {
      send_key_code(table[i][0],table[i][1],KEY_DOWN);
      KB_set_repeat_code(data);
    }
  }
}

inline void parse_key(uint8_t data) {
  uint8_t key;
  uint8_t key_action=(data & 0x80?KEY_UP:KEY_DOWN);
  uint8_t code=data & (uint8_t)~(0x80);
  
  if(code < 0x58) {
    key=pgm_read_byte(&regular[code]);
    if(key < 0xc0) {
      // cancel override, if in effect.
      check_override();
      if(key_action==KEY_UP) {
        PS2_send(PS2_KEY_UP);
      }
      PS2_send(key);
    } else if(key != 0xff) {
      key&=0x3f;
      if((flags & SCAN_FLAG_SHIFT)) {
        // pressing a shifted char
        send_key_code(pgm_read_byte(&mappings[mapping][key][1][0]),pgm_read_byte(&mappings[mapping][key][1][1]),key_action);
      } else {
        send_key_code(pgm_read_byte(&mappings[mapping][key][0][0]),pgm_read_byte(&mappings[mapping][key][0][1]),key_action);
      }
    }
    // if we pressed a key, make it the new repeat key.
    if(key_action==KEY_DOWN)
      KB_set_repeat_code(code);
  } else if(code > 0x57 && code < 0x5d) {
    // joy1
    do_joy(&joy0,data,joy_mapping[1]);
  } else if(code > 0x5f && code < 0x65) {
    // joy0
    do_joy(&joy1,data,joy_mapping[0]);
  }
}

void scan(void) {
  uint8_t data;
  uint8_t esc_flags=SCAN_ESC_NONE;
 
  led_on(LED_PIN_7);
  flags=SCAN_FLAG_NONE;
  mapping=MAP_NORMAL;
  joy0=JOY_NONE;
  joy1=JOY_NONE;
  
  for(;;) {
    handle_cmds();
    if(KB_data_available()) {
      data = KB_recv();
      //printHex(data);
      //debug2(data);
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
          case (0x80 | 0x3e): // enter key up
            esc_flags=SCAN_ESC_NONE;
            led_blink(10,LED_PIN_7);
            led_on(LED_PIN_7);
            break;
        }
      } else {
        // transform keypresses.
        switch(data) {
          case 0x33:  // right shift
            esc_flags|=SCAN_ESC_1;
            flags|=SCAN_FLAG_RSHIFT;
            break;
          case 0xb3:
            esc_flags&=(uint8_t)~SCAN_ESC_1;
            flags &= (uint8_t)~SCAN_FLAG_RSHIFT;
            break;
          case 0x0c:  // left shift
            esc_flags|=SCAN_ESC_2;
            flags|=SCAN_FLAG_LSHIFT;
            break;
          case 0x8c:
            esc_flags&=(uint8_t)~SCAN_ESC_2;
            flags &= (uint8_t)~SCAN_FLAG_LSHIFT;
            break;
          case 0x02:  // commodore down
            esc_flags|=SCAN_ESC_3;
            break;
          case 0x82:  // commodore up
            esc_flags&=(uint8_t)~SCAN_ESC_3;
            break;
        }
        //printHex(data);
        parse_key(data);
        // if KEY_UP AND data is the key repeating, stop.
        if((data & 0x80) && (data & 0x7f) == KB_get_repeat_code()) {
          KB_set_repeat_code(KB_NO_REPEAT);
        }
        if(esc_flags == SCAN_ESC) {
          // probably should do this when they release the keys...
          // turn off KEYS...
          parse_key(0x80 | 0x02); // CMDR
          parse_key(0x80 | 0x0c); // LSHIFT
          parse_key(0x80 | 0x33); // RSHIFT
          flags &= (uint8_t)~(SCAN_FLAG_LSHIFT | SCAN_FLAG_RSHIFT);
          KB_set_repeat_code(KB_NO_REPEAT);
          // note cmd mode.
          led_blink(2,LED_PIN_7);
        }
      }
    }
    if(SW_data_available()) {
      // handle special switches.
      data=SW_recv();
      printHex(data);
      switch(data) {
        case SW_RESTORE:
          switch(mapping) {
            case MAP_VICE:
              PS2_send(PS2_KEY_EXT);
              PS2_send(PS2_KEY_PAGE_UP);
              break;
            default:
              // send Pause/Break...
              PS2_send(PS2_KEY_EXT_2);
              PS2_send(PS2_KEY_BREAK_1);
              PS2_send(PS2_KEY_BREAK_2);
              PS2_send(PS2_KEY_EXT_2);
              PS2_send(PS2_KEY_UP);
              PS2_send(PS2_KEY_BREAK_1);
              PS2_send(PS2_KEY_UP);
              PS2_send(PS2_KEY_BREAK_2);
              break;
          }
          break;
        case (SW_RESTORE | 0x80):
          switch(mapping) {
            case MAP_VICE:
              PS2_send(PS2_KEY_EXT);
              PS2_send(PS2_KEY_UP);
              PS2_send(PS2_KEY_PAGE_UP);
              break;
          }
      }    
    }
  }
}
