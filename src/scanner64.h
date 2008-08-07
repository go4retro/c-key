#ifndef _SCANNER64_H_
#define _SCANNER64_H_

#define SCAN_IRQ_DIVIDER (28)
#define SCAN_LED_IRQ_DIVIDER (14745600/256/2/SCAN_IRQ_DIVIDER/3)   // 3/s

#define SCAN_ADDR_LAYOUT         (void*)8

#define SCAN_CBM_KEY_NONE        0xff
#define SCAN_CBM_KEY_UNMAPPED    0xfe
#define SCAN_CBM_KEY_SPECIAL     0xc0

#define POLL_JOY_NONE  0
#define POLL_JOY_UP    1
#define POLL_JOY_DOWN  2
#define POLL_JOY_RIGHT 4
#define POLL_JOY_LEFT  8
#define POLL_JOY_FIRE  16

#define SCAN_FLAG_NONE   0
#define SCAN_FLAG_LSHIFT 1
#define SCAN_FLAG_RSHIFT 2
#define SCAN_FLAG_SHIFT  3
#define SCAN_FLAG_CTRL   4
#define SCAN_FLAG_CMDR   8

#define SCAN_LAYOUT_SYMBOLIC_C64      0
#define SCAN_LAYOUT_POSITIONAL_C64    1
#define SCAN_LAYOUT_NUM           2

#define SCAN_C64_KEY_RSHIFT     0x33
#define SCAN_C64_KEY_LSHIFT     0x0c
#define SCAN_C64_KEY_CMDR       0x02
#define SCAN_C64_KEY_1          0x07
#define SCAN_C64_KEY_2          0x00
#define SCAN_C64_KEY_3          0x0f
#define SCAN_C64_KEY_EQUALS     0x32
#define SCAN_C64_KEY_RETURN     0x3e
#define SCAN_C64_KEY_CTRL       0x05
#define SCAN_C64_KEY_DEL        0x3f

#if defined __AVR_ATmega162__
#define SW_4080         PIN1
#define SW_CAPSENSE     PIN0
#define SW_RESTORE      PIN2
#elif defined __AVR_ATmega16__
#define SW_4080         PIN4
#define SW_CAPSENSE     PIN5
#define SW_RESTORE      PIN6
#else
#  error Unknown CPU!
#endif


#endif //_SCANNER64_H_
