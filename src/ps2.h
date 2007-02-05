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
    along with Foobar; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef PS2H
#define PS2H 1

#define PS2_RX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#define PS2_TX_BUFFER_SIZE 128     /* 2,4,8,16,32,64,128 or 256 bytes */
#define PS2_RX_BUFFER_MASK ( PS2_RX_BUFFER_SIZE - 1 )
#define PS2_TX_BUFFER_MASK ( PS2_TX_BUFFER_SIZE - 1 )
#if ( PS2_RX_BUFFER_SIZE & PS2_RX_BUFFER_MASK )
	#error PS2 RX buffer size is not a power of 2
#endif
#if ( PS2_TX_BUFFER_SIZE & PS2_TX_BUFFER_MASK )
	#error PS2 TX buffer size is not a power of 2
#endif

#define PS2_PORT_CLK_OUT	PORTD
#define PS2_PORT_CLK_IN		PIND
#define PS2_PIN_CLK 		3
#define PS2_PORT_DDR_CLK	DDRD
#define PS2_PORT_DATA_OUT	PORTD
#define PS2_PORT_DATA_IN	PIND
#define PS2_PIN_DATA		2
#define PS2_PORT_DDR_DATA	DDRD

#define PS2_ST_DEV_IDLE          0
#define PS2_ST_DEV_PREP_START    1
#define PS2_ST_DEV_SEND_START    2
#define PS2_ST_DEV_PREP_BIT      3
#define PS2_ST_DEV_SEND_BIT      4
#define PS2_ST_DEV_PREP_PARITY   5
#define PS2_ST_DEV_SEND_PARITY   6
#define PS2_ST_DEV_PREP_STOP     7
#define PS2_ST_DEV_SEND_STOP     8

#define PS2_ST_DEV_WAIT_CLK_R       9   //J
#define PS2_ST_DEV_PREP_START_R  10  //K
#define PS2_ST_DEV_GET_START_R   11  //L
#define PS2_ST_DEV_PREP_BIT_R    12  //M
#define PS2_ST_DEV_GET_BIT_R     13  //N
#define PS2_ST_DEV_PREP_PARITY_R 14  //O
#define PS2_ST_DEV_GET_PARITY_R  15  //P
#define PS2_ST_DEV_PREP_TURN_R   16  //Q
#define PS2_ST_DEV_GET_TURN_R    17  //R
#define PS2_ST_DEV_PREP_ACK_R    18  //S
#define PS2_ST_DEV_SEND_ACK_R    19  //T
#define PS2_ST_DEV_FINISH_R      20  //U

#define PS2_KEY_UP            0xf0
#define PS2_KEY_EXT           0xe0
#define PS2_Q                 0x15
#define PS2_ALT               0x11
#define PS2_SPACE             0x29
#define PS2_LCTRL             0x14
#define PS2_ESC               0x76
#define PS2_1                 0x16
#define PS2_4                 0x25
#define PS2_S                 0x1b
#define PS2_Z                 0x1a
#define PS2_LSHIFT            0x12
#define PS2_A                 0x1c
#define PS2_W                 0x1d
#define PS2_3                 0x26
#define PS2_T                 0x2c
#define PS2_F                 0x2b
#define PS2_C                 0x21
#define PS2_X                 0x22
#define PS2_D                 0x23
#define PS2_R                 0x2d
#define PS2_5                 0x2e
#define PS2_U                 0x3c
#define PS2_B                 0x32
#define PS2_V                 0x2a
#define PS2_G                 0x34
#define PS2_Y                 0x35
#define PS2_O                 0x44
#define PS2_K                 0x42
#define PS2_H                 0x33
#define PS2_M                 0x3a
#define PS2_N                 0x31
#define PS2_J                 0x3b
#define PS2_I                 0x43
#define PS2_MINUS             0x4e
#define PS2_PERIOD            0x49
#define PS2_COMMA             0x41
#define PS2_L                 0x4b
#define PS2_P                 0x4d
#define PS2_SLASH             0x4a
#define PS2_ENTER             0x5a
#define PS2_BS                0x66
#define PS2_2                 0x1e
#define PS2_APOSTROPHE        0x52
#define PS2_6                 0x36
#define PS2_7                 0x3d
#define PS2_8                 0x3e
#define PS2_9                 0x46
#define PS2_0                 0x45
#define PS2_EQUALS            0x55
#define PS2_SEMICOLON         0x4c
#define PS2_LBRACKET          0x54
#define PS2_RBRACKET          0x5b
#define PS2_CRSR_UP           0x75
#define PS2_CRSR_DOWN         0x72
#define PS2_CRSR_LEFT         0x6b
#define PS2_CRSR_RIGHT        0x74
#define PS2_F2                0x06
#define PS2_F1                0x05
#define PS2_F4                0x0c
#define PS2_F3                0x04
#define PS2_F6                0x0b
#define PS2_F5                0x03
#define PS2_F8                0x0a
#define PS2_F7                0x83
#define PS2_TAB               0x0d
#define PS2_E                 0x24
#define PS2_BACKSLASH         0x5d
#define PS2_BACKQUOTE         0x0e
#define PS2_DEL               0x71
#define PS2_NUM_8             0x75
#define PS2_NUM_7             0x6c
#define PS2_NUM_4             0x6b
#define PS2_NUM_1             0x69
#define PS2_NUM_2             0x72
#define PS2_NUM_3             0x7a
#define PS2_NUM_6             0x74
#define PS2_NUM_9             0x7d
#define PS2_RCTRL             0x14
#define PS2_HOME              0x6c


#define PS2_CMD_RESET         0xff
#define PS2_CMD_ENABLE        0xf4
#define PS2_CMD_DISABLE       0xf5
#define PS2_CMD_ACK           0xfa
#define PS2_CMD_ECHO          0xee
#define PS2_CMD_SET_CODE_SET  0xf0
#define PS2_CMD_SET_RATE      0xf3
#define PS2_CMD_LEDS          0xed
#define PS2_CMD_RESEND        0xfe
#define PS2_CMD_BAT           0xaa
#define PS2_CMD_ERROR         0x00
#define PS2_CMD_OVERFLOW      0xff
#define PS2_CMD_READ_ID       0xf2


void PS2_init(void);

void PS2_set_CLK(void);
void PS2_clear_CLK(void);
unsigned char PS2_read_CLK(void);

void PS2_set_DATA(void);
void PS2_clear_DATA(void);
unsigned char PS2_read_DATA(void);

void PS2_enable_IRQ_CLK_Rise(void);
void PS2_enable_IRQ_CLK_Fall(void);
void PS2_disable_IRQ_CLK(void);

void PS2_enable_IRQ_DATA_Rise(void);
void PS2_enable_IRQ_DATA_Fall(void);
void PS2_disable_IRQ_DATA(void);

void PS2_enable_IRQ_timer0(int us);
void PS2_disable_IRQ_timer0(void);

unsigned char PS2_recv( void );
void PS2_send( unsigned char data );
unsigned char PS2_peek( void );
void PS2_prep_bit(void);
void PS2_send_bit(void);

void PS2_check_CLK(void);
void PS2_handle_cmds(unsigned char data);
unsigned int PS2_get_typematic_delay(unsigned char rate);
unsigned int PS2_get_typematic_period(unsigned char rate);

#endif