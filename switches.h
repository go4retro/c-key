#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#define SW_RX_BUFFER_SIZE 4     /* 2,4,8,16,32,64,128 or 256 bytes */
#define SW_RX_BUFFER_MASK ( SW_RX_BUFFER_SIZE - 1 )
#if ( SW_RX_BUFFER_SIZE & SW_RX_BUFFER_MASK )
  #error SW RX buffer size is not a power of 2
#endif

#define SW_TYPE_OUTPUT   0
#define SW_TYPE_INPUT    1

#if defined __AVR_ATmega162__
#define PORT_SW_OUT      PORTE
#define PORT_SW_IN       PINE
#define PORT_SW_DDR      DDRE
#elif defined __AVR_ATmega16__
#define PORT_SW_OUT      PORTD
#define PORT_SW_IN       PIND
#define PORT_SW_DDR      DDRD
#else
#  error Unknown CPU!
#endif

#define SW_UP             0x80

void SW_init(uint8_t type,uint8_t mask);
uint8_t SW_data_available( void );
void SW_send( uint8_t sw);
uint8_t SW_recv( void );
void SW_scan(void);

#endif //_SWITCHES_H_
