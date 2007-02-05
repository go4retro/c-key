#ifndef _SCANNER64_H_
#define _SCANNER64_H_

#define JOY_NONE  0
#define JOY_UP    1
#define JOY_DOWN  2
#define JOY_RIGHT 4
#define JOY_LEFT  8
#define JOY_FIRE  16

#define KEY_UP    0
#define KEY_DOWN  1

#define SCAN_ESC_NONE   0
#define SCAN_ESC_1      1
#define SCAN_ESC_2      2
#define SCAN_ESC_3      4
#define SCAN_ESC        7

#define MAP_NORMAL      0
#define MAP_VICE        1

#ifdef atmega162
#define SW_4080         PIN0
#define SW_CAPSENSE     PIN1
#define SW_RESTORE      PIN2
#else
#define SW_4080         PIN4
#define SW_CAPSENSE     PIN5
#define SW_RESTORE      PIN6
#endif

#endif //_SCANNER64_H_
