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
#include "kb.h"
#include "switches.h"
#include "ps2.h"
#include "util.h"
#include "led.h"
#include "scanner.h"
#include "scanner64.h"

// TODO I need to clean up this code and make it more like poll64.c

static prog_uint8_t regular[0x58] =  { SCAN_CBM_KEY_SPECIAL+0,PS2_KEY_Q,SCAN_CBM_KEY_SPECIAL+0x0f,PS2_KEY_SPACE,SCAN_CBM_KEY_SPECIAL+0x10,SCAN_CBM_KEY_SPECIAL+0x0d,SCAN_CBM_KEY_SPECIAL+0x0e,PS2_KEY_1
                                      ,PS2_KEY_4,PS2_KEY_E,PS2_KEY_S,PS2_KEY_Z,PS2_KEY_LSHIFT,PS2_KEY_A,PS2_KEY_W,PS2_KEY_3
                                      ,SCAN_CBM_KEY_SPECIAL+1,PS2_KEY_T,PS2_KEY_F,PS2_KEY_C,PS2_KEY_X,PS2_KEY_D,PS2_KEY_R,PS2_KEY_5
                                      ,SCAN_CBM_KEY_SPECIAL+3,PS2_KEY_U,PS2_KEY_H,PS2_KEY_B,PS2_KEY_V,PS2_KEY_G,PS2_KEY_Y,SCAN_CBM_KEY_SPECIAL+2
                                      ,SCAN_CBM_KEY_SPECIAL+5,PS2_KEY_O,PS2_KEY_K,PS2_KEY_M,PS2_KEY_N,PS2_KEY_J,PS2_KEY_I,SCAN_CBM_KEY_SPECIAL+4
                                      ,PS2_KEY_MINUS,SCAN_CBM_KEY_SPECIAL+7,SCAN_CBM_KEY_SPECIAL+9,PS2_KEY_PERIOD,PS2_KEY_COMMA,PS2_KEY_L,PS2_KEY_P,SCAN_CBM_KEY_SPECIAL+6
                                      ,SCAN_CBM_KEY_SPECIAL+0x11,SCAN_CBM_KEY_SPECIAL+0x0c,SCAN_CBM_KEY_SPECIAL+0x0b,PS2_KEY_RSHIFT,PS2_KEY_SLASH,SCAN_CBM_KEY_SPECIAL+0x0a,SCAN_CBM_KEY_SPECIAL+8,PS2_KEY_BACKSLASH
                                      ,SCAN_CBM_KEY_SPECIAL+0x12,SCAN_CBM_KEY_SPECIAL+0x13,SCAN_CBM_KEY_SPECIAL+0x14,SCAN_CBM_KEY_SPECIAL+0x15,SCAN_CBM_KEY_SPECIAL+0x16,SCAN_CBM_KEY_SPECIAL+0x17,PS2_KEY_ENTER,PS2_KEY_BS
                                      ,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE
                                      ,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE
                                      ,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE
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
                                        
static prog_uint8_t joy_table[2][8]={
                                    {POLL_JOY_RIGHT,0,0,POLL_JOY_FIRE,0,POLL_JOY_LEFT,POLL_JOY_DOWN,POLL_JOY_UP},
                                    {0,POLL_JOY_DOWN,POLL_JOY_LEFT,POLL_JOY_RIGHT,POLL_JOY_FIRE,0,0,POLL_JOY_UP}
                                   };
static prog_uint8_t joy_mapping[2][9][2]= {
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
                                          
static uint8_t meta;
static uint8_t layout;
static uint8_t joy0;
static uint8_t joy1;

static uint8_t shift_override=FALSE;
static uint8_t shift_override_state;
static uint8_t shift_override_key;

static uint8_t led_divider=0;

void scan_init(void) {
  // init keyboard matrix scanning engine
  KB_init();
  SW_init(SW_TYPE_INPUT,(1<<SW_RESTORE) | (1<<SW_CAPSENSE) | (1<<SW_4080));
  LED_init(LED_PIN_7);
  meta=SCAN_FLAG_NONE;
  layout=SCAN_LAYOUT_SYMBOLIC;
  joy0=POLL_JOY_NONE;
  joy1=POLL_JOY_NONE;
  
  // initially, load defaults from EEPROM
  while(!eeprom_is_ready());
  layout=eeprom_read_byte(SCAN_ADDR_LAYOUT)%SCAN_LAYOUT_NUM;
  
#ifdef PORT_KEYS
#define IRQ_DIVIDER 48
  OCR2=20; //21 counts * 256 cycles/count * 24 times per run * 120 runs/sec
#else  
#define IRQ_DIVIDER 32
  OCR2=31; //32 counts * 256 cycles/count * 16 times per run * 120 runs/sec
#endif
  TCNT2=0;
  // Set OC2 clk  to SYSCLK/256 and Compare Timer Mode
  TCCR2 = (1<<CS22) | (1<<CS21) | (1<<WGM21);
  // set up OC2 IRQ
  TIMSK |= (1<<OCIE2);
}

void scan_irq(void) {
  KB_scan();
  SW_scan();
  led_divider++;
  if(led_divider==IRQ_DIVIDER) {
    LED_irq();
    led_divider=0;
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
      if(meta&SCAN_FLAG_LSHIFT)
        PS2_send(PS2_KEY_LSHIFT);
      if(meta&SCAN_FLAG_RSHIFT)
        PS2_send(PS2_KEY_RSHIFT);
    } else {
      // let off shift key
      PS2_send(PS2_KEY_UP);
      PS2_send(PS2_KEY_LSHIFT);
    }
    shift_override=FALSE;
  }
}

void send_key_code(uint8_t sh,uint8_t key,uint8_t state) {
  if(key != 0) {
    if(state==FALSE && shift_override && shift_override_key==key && shift_override_state==sh) {
      // it's us, we're unpressing the key.
      check_override();
    } else {
      if(shift_override && (shift_override_key!=key || shift_override_state!=sh)) {
        // we are a new keypress.
        check_override();
      }
      if(!shift_override) {
        // we have a new valid key
        if(state && (meta & SCAN_FLAG_SHIFT) && (sh & SCAN_MAP_SHIFT) == 0) {
          // we are in shift mode and this key needs shift off.
          if(meta&SCAN_FLAG_LSHIFT) {
            PS2_send(PS2_KEY_UP);
            PS2_send(PS2_KEY_LSHIFT);
          }
          if(meta&SCAN_FLAG_RSHIFT) {
            PS2_send(PS2_KEY_UP);
            PS2_send(PS2_KEY_RSHIFT);
          }
          shift_override=TRUE;
          shift_override_key=key;
          shift_override_state=sh;
        } else if(state && !(meta & SCAN_FLAG_SHIFT)&& (sh & SCAN_MAP_SHIFT) == SCAN_MAP_SHIFT) {
          // we are not in shift mode and we need a shift.
          PS2_send(PS2_KEY_LSHIFT);
          shift_override=TRUE;
          shift_override_key=key;
          shift_override_state=sh;
        }
      }
      if((sh & SCAN_MAP_EXT) == SCAN_MAP_EXT)
        PS2_send(PS2_KEY_EXT);
      switch(state) {
        case FALSE:
          PS2_send(PS2_KEY_UP);
        case TRUE:
          PS2_send(key);
          break;
      }
    }
  }
}

uint8_t get_joy_direction(uint8_t *joy) {
  uint8_t i=9;
  switch (*joy & 0x0f) {
    case POLL_JOY_UP:              // 0
      i=0;
      break;
    case POLL_JOY_UP | POLL_JOY_LEFT:   // 1
      i=1;
      break;
    case POLL_JOY_LEFT:            // 2
      i=2;
      break;
    case POLL_JOY_DOWN | POLL_JOY_LEFT: // 3
      i=3;
      break;
    case POLL_JOY_DOWN:            // 4
      i=4;
      break;
    case POLL_JOY_DOWN | POLL_JOY_RIGHT:// 5
      i=5;
      break;
    case POLL_JOY_RIGHT:           // 6
      i=6;
      break;
    case POLL_JOY_RIGHT | POLL_JOY_UP:  // 7
      i=7;
      break;
  }
  return i;
}

void do_joy(uint8_t *joy, uint8_t data, uint8_t map[8],uint8_t table[9][2]) {
  uint8_t i;
  uint8_t new;
  
  // get new button
  new=pgm_read_byte(&map[data&0x07]);
  if((data & KB_KEY_UP) != 0) {
    
    if(new == POLL_JOY_FIRE) {
      // fire not pressed
      i=8;
    } else {
      // direction change
      i=get_joy_direction(joy);
    }
    if(i<9) {
      KB_set_repeat_code(KB_NO_REPEAT);
      send_key_code(pgm_read_byte(&table[i][0]),pgm_read_byte(&table[i][1]),FALSE);
    }
    (*joy)&=(uint8_t)~new;
  } else {
    (*joy)|=new;
    if(new == POLL_JOY_FIRE) {
      // fire pressed
      i=8;
    } else {
      // direction change
      i=get_joy_direction(joy);
    }
    if(i<9) {
      send_key_code(pgm_read_byte(&table[i][0]),pgm_read_byte(&table[i][1]),TRUE);
      KB_set_repeat_code(data);
    }
  }
}

inline void parse_key(uint8_t data) {
  uint8_t key;
  uint8_t state=(data & KB_KEY_UP?FALSE:TRUE);
  uint8_t code=data & (uint8_t)~(KB_KEY_UP);
  
  if(code < 0x58) {
    key=pgm_read_byte(&regular[code]);
    if(key < SCAN_CBM_KEY_SPECIAL) {
      // cancel override, if in effect.
      check_override();
      if(!state) {
        PS2_send(PS2_KEY_UP);
      }
      PS2_send(key);
    } else if(key != SCAN_CBM_KEY_NONE) {
      key&=0x3f;
      if((meta & SCAN_FLAG_SHIFT)) {
        // pressing a shifted char
        send_key_code(pgm_read_byte(&mappings[layout][key][1][0]),pgm_read_byte(&mappings[layout][key][1][1]),state);
      } else {
        send_key_code(pgm_read_byte(&mappings[layout][key][0][0]),pgm_read_byte(&mappings[layout][key][0][1]),state);
      }
    }
    // if we pressed a key, make it the new repeat key.
    if(state)
      KB_set_repeat_code(code);
  } else if(code > 0x57 && code < 0x5d) {
    // joy0, when JP4 is set to scan.
    do_joy(&joy0,data,joy_table[0],joy_mapping[0]);
  } else if(code >= 0x70 && code < 0x78) {
    // joy0 new
    do_joy(&joy0,data,joy_table[0],joy_mapping[0]);
  } else if(code >= 0x78 && code < 0x80) {
    // joy1 new
    do_joy(&joy1,data,joy_table[1],joy_mapping[1]);
  }
}

void scan(void) {
  uint8_t data;
  uint8_t state;
  LED_on(LED_PIN_7);
  
  for(;;) {
    while(PS2_data_available()) {
      data=PS2_recv();
      //debug('%');
      //printHex(data);
      switch(data) {
        case PS2_CMD_SET_RATE:
          PS2_send(PS2_CMD_ACK);
          data=PS2_recv();
          KB_set_repeat_delay(PS2_get_typematic_delay(data));
          KB_set_repeat_period(PS2_get_typematic_period(data));
          break;
        case PS2_CMD_LEDS:
          PS2_send(PS2_CMD_ACK);
          data=PS2_recv()&0x07;
          PS2_send(PS2_CMD_ACK);
          if(data&PS2_LED_CAPS_LOCK) {
            // shift lock is pressed.
            debug('^');
          }
          if(data&PS2_LED_NUM_LOCK) {
            // shift lock is pressed.
            debug('#');
          }
          break;
        default:
          PS2_handle_cmds(data);
          break;
      }
    }
    if(KB_data_available()) {
      data = KB_recv();
      state=(data&KB_KEY_UP?FALSE:TRUE);
      if((meta & SCAN_FLAG_CONFIG) == SCAN_FLAG_CONFIG) {
        switch(data) {
          case SCAN_C64_KEY_1:
            // symbolic
            layout=SCAN_LAYOUT_SYMBOLIC;
            LED_blink(LED_PIN_7,1,LED_FLAG_NONE);
            break;
          case SCAN_C64_KEY_2: 
            // positional
            layout=SCAN_LAYOUT_POSITIONAL;
            LED_blink(LED_PIN_7,2,LED_FLAG_NONE);
            break;
          case (KB_KEY_UP | SCAN_C64_KEY_RETURN):
            meta&=(uint8_t)~SCAN_FLAG_CONFIG;
            // write layout to EEPROM
            update_eeprom(SCAN_ADDR_LAYOUT,layout);
            LED_blink(LED_PIN_7,10,LED_FLAG_END_ON);
            break;
        }
      } else {
        if((data&0x7f) ==SCAN_C64_KEY_RSHIFT)
          meta=(meta&(uint8_t)~SCAN_FLAG_RSHIFT) | (state?SCAN_FLAG_RSHIFT:0);
        else if((data&0x7f) ==SCAN_C64_KEY_LSHIFT)
          meta=(meta&(uint8_t)~SCAN_FLAG_LSHIFT) | (state?SCAN_FLAG_LSHIFT:0);
        else if((data&0x7f) ==SCAN_C64_KEY_CMDR)
          meta=(meta&(uint8_t)~SCAN_FLAG_CMDR) | (state?SCAN_FLAG_CMDR:0);

        /*switch(data) {  // the above is a shorter codepath.
          case SCAN_C64_KEY_RSHIFT:
            meta|=SCAN_FLAG_RSHIFT;
            break;
          case KB_KEY_UP | SCAN_C64_KEY_RSHIFT:
            meta &= (uint8_t)~SCAN_FLAG_RSHIFT;
            break;
          case SCAN_C64_KEY_LSHIFT:
            meta|=SCAN_FLAG_LSHIFT;
            break;
          case KB_KEY_UP | SCAN_C64_KEY_LSHIFT:
            meta &= (uint8_t)~SCAN_FLAG_LSHIFT;
            break;
          case SCAN_C64_KEY_CMDR:
            meta|=SCAN_FLAG_CMDR;
            break;
          case KB_KEY_UP | SCAN_C64_KEY_CMDR:
            meta &= (uint8_t)~SCAN_FLAG_CMDR;
            break;
        }*/
        //printHex(data);
        parse_key(data);
        // if KEY_UP AND data is the key repeating, stop.
        if((data & KB_KEY_UP) && (data & 0x7f) == KB_get_repeat_code()) {
          KB_set_repeat_code(KB_NO_REPEAT);
        }
        // if we match config keypresses, switch into config mode.
        if((meta & SCAN_FLAG_CONFIG) == SCAN_FLAG_CONFIG) {
          // turn off KEYS...
          parse_key(KB_KEY_UP | SCAN_C64_KEY_CMDR);
          parse_key(KB_KEY_UP | SCAN_C64_KEY_LSHIFT);
          parse_key(KB_KEY_UP | SCAN_C64_KEY_RSHIFT);
          KB_set_repeat_code(KB_NO_REPEAT);
          // note cmd mode.
          LED_blink(LED_PIN_7,2,LED_FLAG_NONE);
        }
      }
    }
    if(SW_data_available()) {
      
      // handle special switches.
      data=SW_recv();
      //debug('@');
      //printHex(data);
      switch(data) {
        case SW_CAPSENSE:
          // need to send make for capslock
          PS2_send(PS2_KEY_CAPS_LOCK);
          break;
        case SW_CAPSENSE | KB_KEY_UP:
          // need to send break code for capslock
          PS2_send(PS2_KEY_UP);
          PS2_send(PS2_KEY_CAPS_LOCK);
          break;
        case SW_RESTORE:
          switch(layout) {
            case SCAN_LAYOUT_POSITIONAL:
              PS2_send(PS2_KEY_EXT);
              PS2_send(PS2_KEY_PAGE_UP);
              break;
            default:
              // send Pause/Break...
              PS2_send(PS2_KEY_EXT_2);
              PS2_send(PS2_KEY_PCTRL);
              PS2_send(PS2_KEY_PAUSE);
              PS2_send(PS2_KEY_EXT_2);
              PS2_send(PS2_KEY_UP);
              PS2_send(PS2_KEY_PCTRL);
              PS2_send(PS2_KEY_UP);
              PS2_send(PS2_KEY_PAUSE);
              break;
          }
          break;
        case (SW_RESTORE | KB_KEY_UP):
          switch(layout) {
            case SCAN_LAYOUT_POSITIONAL:
              PS2_send(PS2_KEY_EXT);
              PS2_send(PS2_KEY_UP);
              PS2_send(PS2_KEY_PAGE_UP);
              break;
          }
      }    
    }
  }
}
