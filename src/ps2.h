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
#ifndef PS2_H
#define PS2_H 1

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

/*
 * After a device sends a byte to host, it has to holdoff for a while
 * before doing anything else.  One KB I tested this is 2.14mS.
 */
 
#define PS2_HALF_CYCLE 40 // ~42 uS when all is said and done.
#define PS2_SEND_HOLDOFF_COUNT  ((uint8_t)(2140/PS2_HALF_CYCLE)) 

#define PS2_PORT_CLK_OUT	PORTD
#define PS2_PORT_CLK_IN		PIND
#define PS2_PIN_CLK 		  (1<<PIN3)
#define PS2_PORT_DDR_CLK	DDRD
#define PS2_PORT_DATA_OUT	PORTD
#define PS2_PORT_DATA_IN	PIND
#define PS2_PIN_DATA		  (1<<PIN2)
#define PS2_PORT_DDR_DATA	DDRD

#ifdef atmega8
#define SIG_OUTPUT_COMPARE SIG_OUTPUT_COMPARE2
#define OCR OCR2
#define TCNT  TCNT2
#define TCCR  TCCR2
#define TCCR_DATA_DELAY (1<<CS22) | (1<<CS21) | (1<<CS20) | (1<<WGM21)
#define TCCR_DATA (1<<CS21) | (1<<WGM21)
#define TIFR_DATA (1<<OCF2)
#define TIMSK_DATA (1<<OCIE2)
#else
#define SIG_OUTPUT_COMPARE SIG_OUTPUT_COMPARE0
#define OCR OCR0
#define TCNT  TCNT0
#define TCCR  TCCR0
#define TCCR_DATA_DELAY (1<<CS02) | (1<<CS00) | (1<<WGM01)
#define TCCR_DATA (1<<CS01) | (1<<WGM01)
#define TIFR_DATA (1<<OCF0)
#define TIMSK_DATA (1<<OCIE0)
#endif



#define PS2_ST_IDLE           0
#define PS2_ST_PREP_START     1
#define PS2_ST_SEND_START     2
#define PS2_ST_PREP_BIT       3
#define PS2_ST_SEND_BIT       4
#define PS2_ST_PREP_PARITY    5
#define PS2_ST_SEND_PARITY    6
#define PS2_ST_PREP_STOP      7
#define PS2_ST_SEND_STOP      8

#define PS2_ST_HOLDOFF        9
#define PS2_ST_WAIT_START     10
#define PS2_ST_GET_START      11
#define PS2_ST_WAIT_BIT       12
#define PS2_ST_GET_BIT        13
#define PS2_ST_WAIT_PARITY    14
#define PS2_ST_GET_PARITY     15
#define PS2_ST_WAIT_STOP      16
#define PS2_ST_GET_STOP       17
#define PS2_ST_GET_ACK        18
#define PS2_ST_WAIT_ACK       19
#define PS2_ST_WAIT_ACK2      20
#define PS2_ST_HOST_INHIBIT   21
#define PS2_ST_WAIT_RESPONSE  22

#define PS2_MODE_DEVICE       1
#define PS2_MODE_HOST         2

#define PS2_KEY_UP            0xf0
#define PS2_KEY_EXT           0xe0
#define PS2_KEY_EXT_2         0xe1


// normal keys
#define PS2_KEY_F5            0x03
#define PS2_KEY_F3            0x04
#define PS2_KEY_F1            0x05
#define PS2_KEY_F2            0x06
#define PS2_KEY_F8            0x0a
#define PS2_KEY_F6            0x0b
#define PS2_KEY_F4            0x0c
#define PS2_KEY_TAB           0x0d
#define PS2_KEY_BACKQUOTE     0x0e
#define PS2_KEY_ALT           0x11
#define PS2_KEY_LSHIFT        0x12
#define PS2_KEY_LCTRL         0x14
#define PS2_KEY_Q             0x15
#define PS2_KEY_1             0x16
#define PS2_KEY_Z             0x1a
#define PS2_KEY_S             0x1b
#define PS2_KEY_A             0x1c
#define PS2_KEY_W             0x1d
#define PS2_KEY_2             0x1e
#define PS2_KEY_C             0x21
#define PS2_KEY_X             0x22
#define PS2_KEY_D             0x23
#define PS2_KEY_E             0x24
#define PS2_KEY_4             0x25
#define PS2_KEY_3             0x26
#define PS2_KEY_SPACE         0x29
#define PS2_KEY_V             0x2a
#define PS2_KEY_F             0x2b
#define PS2_KEY_T             0x2c
#define PS2_KEY_R             0x2d
#define PS2_KEY_5             0x2e
#define PS2_KEY_N             0x31
#define PS2_KEY_B             0x32
#define PS2_KEY_H             0x33
#define PS2_KEY_G             0x34
#define PS2_KEY_Y             0x35
#define PS2_KEY_6             0x36
#define PS2_KEY_M             0x3a
#define PS2_KEY_J             0x3b
#define PS2_KEY_U             0x3c
#define PS2_KEY_7             0x3d
#define PS2_KEY_8             0x3e
#define PS2_KEY_COMMA         0x41
#define PS2_KEY_K             0x42
#define PS2_KEY_I             0x43
#define PS2_KEY_O             0x44
#define PS2_KEY_0             0x45
#define PS2_KEY_9             0x46
#define PS2_KEY_PERIOD        0x49
#define PS2_KEY_SLASH         0x4a
#define PS2_KEY_L             0x4b
#define PS2_KEY_SEMICOLON     0x4c
#define PS2_KEY_P             0x4d
#define PS2_KEY_MINUS         0x4e
#define PS2_KEY_APOSTROPHE    0x52
#define PS2_KEY_LBRACKET      0x54
#define PS2_KEY_EQUALS        0x55
#define PS2_KEY_CAPS_LOCK     0x58
#define PS2_KEY_RSHIFT        0x59
#define PS2_KEY_ENTER         0x5a
#define PS2_KEY_RBRACKET      0x5b
#define PS2_KEY_BACKSLASH     0x5d
#define PS2_KEY_BS            0x66
#define PS2_KEY_NUM_1         0x69
#define PS2_KEY_NUM_4         0x6b
#define PS2_KEY_NUM_7         0x6c
#define PS2_KEY_NUM_0         0x70
#define PS2_KEY_NUM_PERIOD    0x71
#define PS2_KEY_NUM_2         0x72
#define PS2_KEY_NUM_5         0x73
#define PS2_KEY_NUM_6         0x74
#define PS2_KEY_NUM_8         0x75
#define PS2_KEY_ESC           0x76
#define PS2_KEY_NUM_LOCK      0x77
#define PS2_KEY_NUM_3         0x7a
#define PS2_KEY_NUM_9         0x7d
#define PS2_KEY_NO_SCROLL     0x7e
#define PS2_KEY_F7            0x83

// extended keys
#define PS2_KEY_RALT          0x11
#define PS2_KEY_ECTRL         0x12
#define PS2_KEY_RCTRL         0x14
#define PS2_KEY_NUM_SLASH     0x4a
#define PS2_KEY_NUM_ENTER     0x5a
#define PS2_KEY_END           0x69
#define PS2_KEY_CRSR_LEFT     0x6b
#define PS2_KEY_HOME          0x6c
#define PS2_KEY_INSERT        0x70
#define PS2_KEY_DELETE        0x71
#define PS2_KEY_CRSR_DOWN     0x72
#define PS2_KEY_CRSR_RIGHT    0x74
#define PS2_KEY_CRSR_UP       0x75
#define PS2_KEY_PAGE_DOWN     0x7a
#define PS2_KEY_PRINT_SCREEN  0x7c
#define PS2_KEY_PAGE_UP       0x7d

// new ones
#define PS2_KEY_PCTRL         0x14
#define PS2_KEY_PAUSE         0x77


#define PS2_CMD_LEDS          0xed
#define PS2_CMD_ECHO          0xee
#define PS2_CMD_SET_CODE_SET  0xf0
#define PS2_CMD_READ_ID       0xf2
#define PS2_CMD_SET_RATE      0xf3
#define PS2_CMD_ENABLE        0xf4
#define PS2_CMD_DISABLE       0xf5
#define PS2_CMD_DEFAULT       0xf6
#define PS2_CMD_RESEND        0xfe
#define PS2_CMD_RESET         0xff

#define PS2_CMD_ERROR         0x00
#define PS2_CMD_BAT           0xaa
#define PS2_CMD_ECHO_RESP     0xee
#define PS2_CMD_ACK           0xfa
#define PS2_CMD_BAT_FAILURE   0xfc
#define PS2_CMD_OVERFLOW      0xff

#define PS2_LED_SCROLL_LOCK   1
#define PS2_LED_NUM_LOCK      2
#define PS2_LED_CAPS_LOCK     4



void PS2_init(uint8_t mode);

void PS2_delay(uint16_t ms);
inline uint8_t PS2_set_CLK(void);
inline void PS2_clear_CLK(void);
inline uint8_t PS2_read_CLK(void);

inline void PS2_set_DATA(void);
inline void PS2_clear_DATA(void);
inline uint8_t PS2_read_DATA(void);

inline void PS2_enable_IRQ_CLK_Rise(void);
inline void PS2_enable_IRQ_CLK_Fall(void);
inline void PS2_disable_IRQ_CLK(void);

inline void PS2_enable_IRQ_timer0(uint8_t us);
inline void PS2_disable_IRQ_timer0(void);

uint8_t PS2_get_state(void);
void PS2_set_state(uint8_t state);
inline uint8_t PS2_get_count(void);

uint8_t PS2_recv( void );
void PS2_send( uint8_t data );
uint8_t PS2_data_available( void );
void PS2_write_byte(void);
void PS2_read_byte(void);
void PS2_commit_read_byte(void);
uint8_t PS2_data_to_send(void);

void PS2_write_bit(void);
void PS2_write_parity(void);
void PS2_read_bit(void);
void PS2_clear_counters(void);

void PS2_check_CLK(void);
void PS2_handle_cmds(uint8_t data);
unsigned int PS2_get_typematic_delay(uint8_t rate);
unsigned int PS2_get_typematic_period(uint8_t rate);
void PS2_set_debug(uint8_t b);

#endif