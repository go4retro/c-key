#ifndef _POLL64_H_
#define _POLL64_H_

#define C64_KEY_NONE        0xff
#define C64_KEY_UNMAPPED    0xff
#define C64_KEY_SPECIAL     0xc0

#define POLL_MAP_NONE       0
#define POLL_MAP_SHIFT      1

#define POLL_LAYOUT_POSITIONAL_C64  0
#define POLL_LAYOUT_POSITIONAL_C128 1
#define POLL_LAYOUT_SYMBOLIC_C64    2
#define POLL_LAYOUT_SYMBOLIC_C128   3

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
#define POLL_ST_PRINT_1       11
#define POLL_ST_PRINT_2       12
#define POLL_ST_PRINT_UP_1    13
#define POLL_ST_PRINT_UP_2    14
#define POLL_ST_PRINT_UP_3    15

#ifdef REV3
#define C64_KEY_RUNSTOP     0x7A
#define C64_KEY_LSHIFT      0x72
#define C64_KEY_X           0x7E
#define C64_KEY_V           0x76  
#define C64_KEY_N           0xBE
#define C64_KEY_COMMA       0xB6
#define C64_KEY_SLASH       0x3E
#define C64_KEY_CRSR_DOWN   0x36
#define C64_KEY_CTRL        0x5A
#define C64_KEY_A           0x52
#define C64_KEY_D           0x5E
#define C64_KEY_G           0x56
#define C64_KEY_J           0x9E
#define C64_KEY_L           0x96
#define C64_KEY_SEMICOLON   0x1E
#define C64_KEY_CRSR_RIGHT  0x16
#define C64_KEY_BACKARROW   0x6A
#define C64_KEY_W           0x62
#define C64_KEY_R           0x6E
#define C64_KEY_Y           0x66
#define C64_KEY_I           0xAE
#define C64_KEY_P           0xA6
#define C64_KEY_ASTERIX     0x2E
#define C64_KEY_RETURN      0x26
#define C64_KEY_1           0x4A
#define C64_KEY_3           0x42
#define C64_KEY_5           0x4E
#define C64_KEY_7           0x46
#define C64_KEY_9           0x8E
#define C64_KEY_PLUS        0x86
#define C64_KEY_POUND       0x0E
#define C64_KEY_DELETE      0x06
#define C64_KEY_2           0x78
#define C64_KEY_4           0x70
#define C64_KEY_6           0x7C
#define C64_KEY_8           0x74
#define C64_KEY_0           0xBC
#define C64_KEY_MINUS       0xB4
#define C64_KEY_HOME        0x3C
#define C64_KEY_F7          0x34
#define C64_KEY_Q           0x58
#define C64_KEY_E           0x50
#define C64_KEY_T           0x5C
#define C64_KEY_U           0x54
#define C64_KEY_O           0x9C
#define C64_KEY_AT          0x94
#define C64_KEY_UPARROW     0x1C
#define C64_KEY_F5          0x14
#define C64_KEY_CBM         0x68
#define C64_KEY_S           0x60
#define C64_KEY_F           0x6C
#define C64_KEY_H           0x64
#define C64_KEY_K           0xAC
#define C64_KEY_COLON       0xA4
#define C64_KEY_EQUALS      0x2C
#define C64_KEY_F3          0x24
#define C64_KEY_SPACE       0x48
#define C64_KEY_Z           0x40
#define C64_KEY_C           0x4C
#define C64_KEY_B           0x44
#define C64_KEY_M           0x8C
#define C64_KEY_PERIOD      0x84
#define C64_KEY_RSHIFT      0x0C
#define C64_KEY_F1          0x04
#define C128_KEY_NOSCROLL   0x3A
#define C128_KEY_NUM_3      0xB2
#define C128_KEY_NUM_1      0xBA
#define C128_KEY_NUM_PERIOD 0x1A
#define C128_KEY_NUM_MINUS  0x92
#define C128_KEY_NUM_5      0x9A
#define C128_KEY_NUM_0      0x2A
#define C128_KEY_NUM_PLUS   0xA2
#define C128_KEY_NUM_8      0xAA
#define C128_KEY_CRSR_UP    0x38
#define C128_KEY_LINEFEED   0xB0
#define C128_KEY_TAB        0xB8
#define C128_KEY_CRSR_RIGHT 0x18
#define C128_KEY_NUM_9      0x90
#define C128_KEY_NUM_7      0x98
#define C128_KEY_CRSR_LEFT  0x28
#define C128_KEY_NUM_6      0xA0
#define C128_KEY_NUM_4      0xA8
#define C128_KEY_CRSR_DOWN  0x08
#define C128_KEY_ENTER      0x80
#define C128_KEY_NUM_2      0x88
#define C128_KEY_ALT        0x0A
#define C128_KEY_ESC        0x82
#define C128_KEY_HELP       0x8A

#else 
#define C64_KEY_RUNSTOP   0x40
#define C64_KEY_LSHIFT    0x48
#define C64_KEY_X         0x04
#define C64_KEY_V         0x0C
#define C64_KEY_N         0x84
#define C64_KEY_COMMA     0x8C
#define C64_KEY_SLASH     0x44
#define C64_KEY_CRSR_DOWN 0x4c
#define C64_KEY_CTRL      0x60
#define C64_KEY_A         0x68
#define C64_KEY_D         0x24
#define C64_KEY_G         0x2C
#define C64_KEY_J         0xA4
#define C64_KEY_L         0xAC
#define C64_KEY_SEMICOLON 0x64
#define C64_KEY_CRSR_RIGHT  0x6C
#define C64_KEY_BACKARROW 0x50
#define C64_KEY_W         0x58
#define C64_KEY_R         0x14
#define C64_KEY_Y         0x1C
#define C64_KEY_I         0x94
#define C64_KEY_P         0x9C
#define C64_KEY_ASTERIX   0x54
#define C64_KEY_RETURN    0x5C
#define C64_KEY_1         0x70
#define C64_KEY_3         0x78
#define C64_KEY_5         0x34
#define C64_KEY_7         0x3C
#define C64_KEY_9         0xB4
#define C64_KEY_PLUS      0xBC
#define C64_KEY_POUND     0x74
#define C64_KEY_DELETE    0x7C
#define C64_KEY_2         0x42
#define C64_KEY_4         0x4A
#define C64_KEY_6         0x06
#define C64_KEY_8         0x0E
#define C64_KEY_0         0x86
#define C64_KEY_MINUS     0x8E
#define C64_KEY_HOME      0x46
#define C64_KEY_F7        0x4E
#define C64_KEY_Q         0x62
#define C64_KEY_E         0x6A
#define C64_KEY_T         0x26
#define C64_KEY_U         0x2E
#define C64_KEY_O         0xA6
#define C64_KEY_AT        0xAE
#define C64_KEY_UPARROW   0x66
#define C64_KEY_F5        0x6E
#define C64_KEY_CBM       0x52
#define C64_KEY_S         0x5A
#define C64_KEY_F         0x16
#define C64_KEY_H         0x1E
#define C64_KEY_K         0x96
#define C64_KEY_COLON     0x9E
#define C64_KEY_EQUALS    0x56
#define C64_KEY_F3        0x5E
#define C64_KEY_SPACE     0x72
#define C64_KEY_Z         0x7A
#define C64_KEY_C         0x36
#define C64_KEY_B         0x3E
#define C64_KEY_M         0xB6
#define C64_KEY_PERIOD    0xBE
#define C64_KEY_RSHIFT    0x76
#define C64_KEY_F1        0x7E

#define C128_KEY_NOSCROLL   0x08
#define C128_KEY_NUM_3      0x80
#define C128_KEY_NUM_1      0x88
#define C128_KEY_NUM_PERIOD 0x28
#define C128_KEY_NUM_MINUS  0xA0
#define C128_KEY_NUM_5      0xA8
#define C128_KEY_NUM_0      0x18
#define C128_KEY_NUM_PLUS   0x90
#define C128_KEY_NUM_8      0x98
#define C128_KEY_CRSR_UP    0x0A
#define C128_KEY_LINEFEED   0x82
#define C128_KEY_TAB        0x8A
#define C128_KEY_CRSR_RIGHT 0x2A
#define C128_KEY_NUM_9      0xA2
#define C128_KEY_NUM_7      0xAA
#define C128_KEY_CRSR_LEFT  0x1A
#define C128_KEY_NUM_6      0x92
#define C128_KEY_NUM_4      0x9A
#define C128_KEY_CRSR_DOWN  0x3A
#define C128_KEY_ENTER      0xB2
#define C128_KEY_NUM_2      0xBA
#define C128_KEY_ALT        0x38
#define C128_KEY_ESC        0xB0
#define C128_KEY_HELP       0xB8
#endif
// pseudo keys:

#define C128_PKEY_4080      0x00
#define C128_PKEY_CAPSENSE  0x10
#define C64_PKEY_RESTORE    0x20

#define XPT_PORT_DATA_OUT PORTB
#define XPT_DDR_DATA DDRB
#define XPT_PORT_STROBE_OUT PORTD
#define XPT_DDR_STROBE DDRD

#ifdef atmega162
#define XPT_PIN_STROBE (1<<PIN6)
#else
#define XPT_PIN_STROBE (1<<PIN7)
#endif

#ifdef atmega162
#define SW_4080         PIN0
#define SW_CAPSENSE     PIN1
#define SW_RESTORE      PIN2
#else
#define SW_4080         PIN4
#define SW_CAPSENSE     PIN5
#define SW_RESTORE      PIN6
#endif



#define POLL_FLAG_LSHIFT 1
#define POLL_FLAG_RSHIFT 2
#define POLL_FLAG_SHIFT (POLL_FLAG_LSHIFT | POLL_FLAG_RSHIFT)
#define POLL_FLAG_ALT    4
#define POLL_FLAG_CONTROL 8
#define POLL_FLAG_CTRL_ALT     (POLL_FLAG_CONTROL | POLL_FLAG_ALT)


#endif //_POLL64_H_
