#ifndef _SCANNER64_H_
#define _SCANNER64_H_

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

#define SCAN_FLAG_CONFIG (SCAN_FLAG_SHIFT |SCAN_FLAG_CMDR)

#define SCAN_LAYOUT_SYMBOLIC      0
#define SCAN_LAYOUT_POSITIONAL    1
#define SCAN_LAYOUT_NUM           2

#define SCAN_C64_KEY_RSHIFT     0x33
#define SCAN_C64_KEY_LSHIFT     0x0c
#define SCAN_C64_KEY_CMDR       0x02
#define SCAN_C64_KEY_1          0x07
#define SCAN_C64_KEY_2          0x00
#define SCAN_C64_KEY_RETURN     0x3e
#define SCAN_C64_KEY_CTRL       0x05

#ifdef atmega162
#define SW_4080         PIN1
#define SW_CAPSENSE     PIN0
#define SW_RESTORE      PIN2
#else
#define SW_4080         PIN4
#define SW_CAPSENSE     PIN5
#define SW_RESTORE      PIN6
#endif


#endif //_SCANNER64_H_
