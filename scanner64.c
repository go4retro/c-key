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

static prog_uint8_t normal[0x58] =  { SCAN_CBM_KEY_SPECIAL+0,PS2_KEY_Q,SCAN_CBM_KEY_SPECIAL+0x0f,PS2_KEY_SPACE,SCAN_CBM_KEY_SPECIAL+0x10,SCAN_CBM_KEY_SPECIAL+0x0d,SCAN_CBM_KEY_SPECIAL+0x0e,PS2_KEY_1
                                      ,PS2_KEY_4,PS2_KEY_E,PS2_KEY_S,PS2_KEY_Z,PS2_KEY_LSHIFT,PS2_KEY_A,PS2_KEY_W,PS2_KEY_3
                                      ,SCAN_CBM_KEY_SPECIAL+1,PS2_KEY_T,PS2_KEY_F,PS2_KEY_C,PS2_KEY_X,PS2_KEY_D,PS2_KEY_R,PS2_KEY_5
                                      ,SCAN_CBM_KEY_SPECIAL+3,PS2_KEY_U,PS2_KEY_H,PS2_KEY_B,PS2_KEY_V,PS2_KEY_G,PS2_KEY_Y,SCAN_CBM_KEY_SPECIAL+2
                                      ,SCAN_CBM_KEY_SPECIAL+5,PS2_KEY_O,PS2_KEY_K,PS2_KEY_M,PS2_KEY_N,PS2_KEY_J,PS2_KEY_I,SCAN_CBM_KEY_SPECIAL+4
                                      ,SCAN_CBM_KEY_SPECIAL+24,SCAN_CBM_KEY_SPECIAL+7,SCAN_CBM_KEY_SPECIAL+9,PS2_KEY_PERIOD,PS2_KEY_COMMA,PS2_KEY_L,PS2_KEY_P,SCAN_CBM_KEY_SPECIAL+6
                                      ,SCAN_CBM_KEY_SPECIAL+0x11,SCAN_CBM_KEY_SPECIAL+0x0c,SCAN_CBM_KEY_SPECIAL+0x0b,PS2_KEY_RSHIFT,PS2_KEY_SLASH,SCAN_CBM_KEY_SPECIAL+0x0a,SCAN_CBM_KEY_SPECIAL+8,PS2_KEY_BACKSLASH
                                      ,SCAN_CBM_KEY_SPECIAL+0x12,SCAN_CBM_KEY_SPECIAL+0x13,SCAN_CBM_KEY_SPECIAL+0x14,SCAN_CBM_KEY_SPECIAL+0x15,SCAN_CBM_KEY_SPECIAL+0x16,SCAN_CBM_KEY_SPECIAL+0x17,PS2_KEY_ENTER,PS2_KEY_BS
                                      ,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE
                                      ,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE
                                      ,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE,SCAN_CBM_KEY_NONE
                                     };
                                        
static prog_uint8_t layouts[2][25][2][2] = {
                                          // default layouts
                                         {
                                          {{SCAN_MAP_NONE,PS2_KEY_2},{SCAN_MAP_SHIFT,PS2_KEY_APOSTROPHE}},            //0 0 -> 2/Sh+'
                                          {{SCAN_MAP_NONE,PS2_KEY_6},{SCAN_MAP_SHIFT,PS2_KEY_7}},                     //1 6 -> 6/Sh+7
                                          {{SCAN_MAP_NONE,PS2_KEY_7},{SCAN_MAP_NONE,PS2_KEY_APOSTROPHE}},             //2 7 -> 7/'
                                          {{SCAN_MAP_NONE,PS2_KEY_8},{SCAN_MAP_SHIFT,PS2_KEY_9}},                     //3 8 -> 8/Sh+9
                                          {{SCAN_MAP_NONE,PS2_KEY_9},{SCAN_MAP_SHIFT,PS2_KEY_0}},                     //4 9 -> 9/Sh+0
                                          {{SCAN_MAP_NONE,PS2_KEY_0},{SCAN_MAP_EXT,PS2_KEY_DELETE}},                  //5 0 -> 0/
                                          {{SCAN_MAP_SHIFT,PS2_KEY_EQUALS},{SCAN_MAP_EXT,PS2_KEY_INSERT}},            //6 + -> Sh+=/Insert
                                          {{SCAN_MAP_SHIFT,PS2_KEY_2},{SCAN_MAP_SHIFT,PS2_KEY_LBRACKET}},             //7 @ -> Sh+2/Sh+[
                                          {{SCAN_MAP_SHIFT,PS2_KEY_8},{SCAN_MAP_SHIFT,PS2_KEY_RBRACKET}},             //8 * -> Sh+8/Sh+]
                                          {{SCAN_MAP_SHIFT,PS2_KEY_SEMICOLON},{SCAN_MAP_NONE,PS2_KEY_LBRACKET}},      //9 : -> Sh+;/[
                                          {{SCAN_MAP_NONE,PS2_KEY_SEMICOLON},{SCAN_MAP_NONE,PS2_KEY_RBRACKET}},       //10 ; -> ;/]
                                          {{SCAN_MAP_NONE,PS2_KEY_EQUALS},{SCAN_MAP_NONE,PS2_KEY_PAGE_DOWN}},         //11 = -> =/PageDown
                                          {{SCAN_MAP_SHIFT,PS2_KEY_6},{SCAN_MAP_NONE,PS2_KEY_PAGE_UP}},               //12 ^ -> Sh+6/PageUp
                                          {{SCAN_MAP_NONE,PS2_KEY_LCTRL},{SCAN_MAP_SHIFT,PS2_KEY_LCTRL}},             //13 LCTRL -> LCTRL/Sh+LCTRL 
                                          {{SCAN_MAP_NONE,PS2_KEY_BACKQUOTE},{SCAN_MAP_SHIFT,PS2_KEY_BACKQUOTE}},     //14 <- -> `/Sh+`
                                          {{SCAN_MAP_NONE,PS2_KEY_ALT},{SCAN_MAP_SHIFT,PS2_KEY_ALT}},                 //15 C= -> Alt/Sh+Alt
                                          {{SCAN_MAP_NONE,PS2_KEY_ESC},{SCAN_MAP_SHIFT,PS2_KEY_ESC}},                 //16 RUN-STOP -> ESC/Sh+ESC
                                          {{SCAN_MAP_EXT,PS2_KEY_HOME},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_HOME}}, //17 HOME -> HOME/SH+HOME
                                          {{SCAN_MAP_NONE,PS2_KEY_F7},{SCAN_MAP_NONE,PS2_KEY_F8}},                    //18 f7
                                          {{SCAN_MAP_NONE,PS2_KEY_F5},{SCAN_MAP_NONE,PS2_KEY_F6}},                    //19 f5
                                          {{SCAN_MAP_NONE,PS2_KEY_F3},{SCAN_MAP_NONE,PS2_KEY_F4}},                    //20 f3
                                          {{SCAN_MAP_NONE,PS2_KEY_F1},{SCAN_MAP_NONE,PS2_KEY_F2}},                    //21 f1
                                          {{SCAN_MAP_EXT,PS2_KEY_CRSR_DOWN},{SCAN_MAP_EXT,PS2_KEY_CRSR_UP}},          //22 CRSR DOWN
                                          {{SCAN_MAP_EXT,PS2_KEY_CRSR_RIGHT},{SCAN_MAP_EXT,PS2_KEY_CRSR_LEFT}},       //23  CRSR RIGHT
                                          {{SCAN_MAP_NONE,PS2_KEY_MINUS},{SCAN_MAP_SHIFT,PS2_KEY_MINUS}}              // 24 - -> -/Sh+-
                                         },
                                         // VICE layouts
                                         {
                                          {{SCAN_MAP_NONE,PS2_KEY_2},{SCAN_MAP_SHIFT,PS2_KEY_2}},                     //2
                                          {{SCAN_MAP_NONE,PS2_KEY_6},{SCAN_MAP_SHIFT,PS2_KEY_6}},                     //6
                                          {{SCAN_MAP_NONE,PS2_KEY_7},{SCAN_MAP_SHIFT,PS2_KEY_7}},                     //7
                                          {{SCAN_MAP_NONE,PS2_KEY_8},{SCAN_MAP_SHIFT,PS2_KEY_8}},                     //8
                                          {{SCAN_MAP_NONE,PS2_KEY_9},{SCAN_MAP_SHIFT,PS2_KEY_9}},                     //9
                                          {{SCAN_MAP_NONE,PS2_KEY_0},{SCAN_MAP_SHIFT,PS2_KEY_0}},                     //0
                                          {{SCAN_MAP_NONE,PS2_KEY_MINUS},{SCAN_MAP_SHIFT,PS2_KEY_MINUS}},             //+
                                          {{SCAN_MAP_NONE,PS2_KEY_LBRACKET},{SCAN_MAP_SHIFT,PS2_KEY_LBRACKET}},       //@
                                          {{SCAN_MAP_NONE,PS2_KEY_RBRACKET},{SCAN_MAP_SHIFT,PS2_KEY_RBRACKET}},       //*
                                          {{SCAN_MAP_NONE,PS2_KEY_SEMICOLON},{SCAN_MAP_SHIFT,PS2_KEY_SEMICOLON}},     //:
                                          {{SCAN_MAP_NONE,PS2_KEY_APOSTROPHE},{SCAN_MAP_SHIFT,PS2_KEY_APOSTROPHE}},   //;
                                          {{SCAN_MAP_NONE,PS2_KEY_BACKSLASH},{SCAN_MAP_SHIFT,PS2_KEY_BACKSLASH}},     //=
                                          {{SCAN_MAP_EXT,PS2_KEY_DELETE},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_DELETE}},//^ = DEL
                                          {{SCAN_MAP_NONE,PS2_KEY_TAB},{SCAN_MAP_SHIFT,PS2_KEY_TAB}},                 //CTRL
                                          {{SCAN_MAP_NONE,PS2_KEY_BACKQUOTE},{SCAN_MAP_SHIFT,PS2_KEY_BACKQUOTE}},     //<-
                                          {{SCAN_MAP_NONE,PS2_KEY_LCTRL},{SCAN_MAP_SHIFT,PS2_KEY_LCTRL}},             // C=
                                          {{SCAN_MAP_NONE,PS2_KEY_ESC},{SCAN_MAP_SHIFT,PS2_KEY_ESC}},                 // RUN-STOP
                                          {{SCAN_MAP_EXT,PS2_KEY_HOME},{SCAN_MAP_SHIFT | SCAN_MAP_EXT,PS2_KEY_HOME}}, // CLR/HOME
                                          {{SCAN_MAP_NONE,PS2_KEY_F7},{SCAN_MAP_NONE,PS2_KEY_F8}},                    // f7
                                          {{SCAN_MAP_NONE,PS2_KEY_F5},{SCAN_MAP_NONE,PS2_KEY_F6}},                    // f5
                                          {{SCAN_MAP_NONE,PS2_KEY_F3},{SCAN_MAP_NONE,PS2_KEY_F4}},                    // f3
                                          {{SCAN_MAP_NONE,PS2_KEY_F1},{SCAN_MAP_NONE,PS2_KEY_F2}},                    // f1
                                          {{SCAN_MAP_NONE,PS2_KEY_CRSR_DOWN},{SCAN_MAP_NONE,PS2_KEY_CRSR_UP}},        // CRSR DOWN
                                          {{SCAN_MAP_NONE,PS2_KEY_CRSR_RIGHT},{SCAN_MAP_NONE,PS2_KEY_CRSR_LEFT}},     // CRSR RIGHT
                                          {{SCAN_MAP_SHIFT,PS2_KEY_EQUALS},{SCAN_MAP_SHIFT,PS2_KEY_EQUALS}}           // -
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

static uint16_t led_divider;
static uint8_t led_state;

void scan_init(void) {
  // init keyboard matrix scanning engine
  KB_init();
  SW_init(SW_TYPE_INPUT,(1<<SW_RESTORE) | (1<<SW_CAPSENSE) | (1<<SW_4080));
  LED_init(LED_PIN_7);
  //meta=SCAN_FLAG_NONE;
  //joy0=POLL_JOY_NONE;
  //joy1=POLL_JOY_NONE;

  
  // initially, load defaults from EEPROM
  while(!eeprom_is_ready());
  layout=eeprom_read_byte(SCAN_ADDR_LAYOUT)%SCAN_LAYOUT_NUM;
  
  OCR2=SCAN_IRQ_DIVIDER; //28 counts * 256 cycles/count * 33 times per run * 60 runs/sec
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
  if(led_divider==SCAN_LED_IRQ_DIVIDER) {
    led_divider=0;
    LED_irq();
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
  uint8_t sh=(meta&SCAN_FLAG_SHIFT?1:0);
  uint8_t code=data & (uint8_t)~(KB_KEY_UP);
  
  if(code < 0x58) {
    key=pgm_read_byte(&normal[code]);
    if(key < SCAN_CBM_KEY_SPECIAL) {
      // cancel override, if in effect.
      check_override();
      if(!state) {
        PS2_send(PS2_KEY_UP);
      }
      PS2_send(key);
    } else if(key != SCAN_CBM_KEY_NONE) {
      key&=0x3f;
      send_key_code(pgm_read_byte(&layouts[layout][key][sh][0]),pgm_read_byte(&layouts[layout][key][sh][1]),state);
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
  uint8_t config=FALSE;
  uint8_t debug=FALSE;
  LED_blink(LED_PIN_7,layout+1,LED_FLAG_END_ON);
  
  for(;;) {
    while(PS2_data_available()) {
      data=PS2_recv();
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
          led_state=data;
          PS2_send(PS2_CMD_ACK);
          if(!config) {
            if(data&PS2_LED_CAPS_LOCK) {
              // caps lock LED is on.
              LED_off(LED_PIN_7);
            } else {
              LED_on(LED_PIN_7);
            }
            //if(data&PS2_LED_NUM_LOCK) {
            //  // num lock is on.
            //}
          }
          break;
        default:
          PS2_handle_cmds(data);
          break;
      }
    }
    if(KB_data_available()) {
      data = KB_recv();
      if(debug) {
        debug('k');
        printHex(data);
      }
      state=(data&KB_KEY_UP?FALSE:TRUE);
      if((data&0x7f) ==SCAN_C64_KEY_LSHIFT)
        meta=(meta&(uint8_t)~SCAN_FLAG_LSHIFT) | (state?SCAN_FLAG_LSHIFT:0);
      else if((data&0x7f) ==SCAN_C64_KEY_RSHIFT)
        meta=(meta&(uint8_t)~SCAN_FLAG_RSHIFT) | (state?SCAN_FLAG_RSHIFT:0);
      else if((data&0x7f) ==SCAN_C64_KEY_CTRL)
        meta=(meta&(uint8_t)~SCAN_FLAG_CTRL) | (state?SCAN_FLAG_CTRL:0);
      else if((data&0x7f) ==SCAN_C64_KEY_CMDR)
        meta=(meta&(uint8_t)~SCAN_FLAG_CMDR) | (state?SCAN_FLAG_CMDR:0);
      else if((meta&(SCAN_FLAG_CMDR|SCAN_FLAG_CTRL))==(SCAN_FLAG_CMDR|SCAN_FLAG_CTRL) && data==(KB_KEY_UP|SCAN_C64_KEY_DEL)) {
        // Ctrl/CMDR/DEL
        if(!config) {
          // go into config mode
          parse_key(KB_KEY_UP | SCAN_C64_KEY_CMDR);
          parse_key(KB_KEY_UP | SCAN_C64_KEY_CTRL);
          parse_key(KB_KEY_UP | SCAN_C64_KEY_DEL);
          KB_set_repeat_code(KB_NO_REPEAT);
          LED_blink(LED_PIN_7,10,LED_FLAG_NONE);
        } else {
          LED_blink(LED_PIN_7,10,(led_state&PS2_LED_CAPS_LOCK?LED_FLAG_NONE:LED_FLAG_END_ON));
          // write layout to EEPROM
          update_eeprom(SCAN_ADDR_LAYOUT,layout);
        }
        config=!config;
      }
      if(config) {
        switch(data) {
          case SCAN_C64_KEY_1:
            // symbolic
            layout=SCAN_LAYOUT_SYMBOLIC_C64;
            LED_blink(LED_PIN_7,1,LED_FLAG_NONE);
            break;
          case SCAN_C64_KEY_3: 
            // positional
            layout=SCAN_LAYOUT_POSITIONAL_C64;
            LED_blink(LED_PIN_7,3,LED_FLAG_NONE);
            break;
          case SCAN_C64_KEY_EQUALS: 
            // debug
            debug=!debug;
            PS2_set_debug(debug);
            LED_blink(LED_PIN_7,1,LED_FLAG_NONE);
            break;
        }
      } else {
        parse_key(data);
        // if KEY_UP AND data is the key repeating, stop.
        if((data & KB_KEY_UP) && (data & 0x7f) == KB_get_repeat_code()) {
          KB_set_repeat_code(KB_NO_REPEAT);
        }
      }
    }
    if(SW_data_available()) {
      
      // handle special switches.
      data=SW_recv();
      if(debug) {
        debug('s');
        printHex(data);
      }
      if(!config) {
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
              case SCAN_LAYOUT_POSITIONAL_C64:
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
              case SCAN_LAYOUT_POSITIONAL_C64:
                PS2_send(PS2_KEY_EXT);
                PS2_send(PS2_KEY_UP);
                PS2_send(PS2_KEY_PAGE_UP);
                break;
            }
            break;
        }
      }    
    }
  }
}
