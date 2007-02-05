#include <avr/io.h>
#include <inttypes.h>
#include "util.h"
#include "switches.h"

static unsigned char SW_RxBuf[SW_RX_BUFFER_SIZE];
static volatile uint8_t SW_RxHead;
static volatile uint8_t SW_RxTail;

static uint8_t SW_cache;
static uint8_t SW_mask;

void SW_init(uint8_t type, uint8_t mask) {
  SW_mask=mask;
  // bring them to inputs;
  PORT_SW_DDR&=(uint8_t)~mask;
  // bring them HI-Z
  PORT_SW_OUT|=mask;
  SW_cache=(PORT_SW_IN & mask);
}

void SW_store(uint8_t data) {
  uint8_t tmphead;
  
  tmphead = ( SW_RxHead + 1 ) & SW_RX_BUFFER_MASK;
  SW_RxHead = tmphead;      /* Store new index */

  if ( tmphead == SW_RxTail ) {
    /* ERROR! Receive buffer overflow */
  }
  
  SW_RxBuf[tmphead] = data; /* Store received data in buffer */
}

uint8_t SW_data_available( void ) {
  return ( SW_RxHead != SW_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
}

uint8_t SW_send( uint8_t sw) {
  // 0x80 indicates key up.
  uint8_t state=(sw&0x80);
  sw&=0x7f;
  if(state) {
    // bring to input and set HI-Z
    PORT_SW_DDR&=(uint8_t)~(1<<sw);
    PORT_SW_OUT|=(1<<sw);
  } else {
    PORT_SW_DDR|=(1<<sw);
    PORT_SW_OUT&=(uint8_t)~(1<<sw);
  }
}

uint8_t SW_recv( void ) {
  uint8_t tmptail;
  
  while ( SW_RxHead == SW_RxTail ) {
    ;
  }
  tmptail = ( SW_RxTail + 1 ) & SW_RX_BUFFER_MASK;/* Calculate buffer index */
  
  SW_RxTail = tmptail;                /* Store new index */
  
  return SW_RxBuf[tmptail];           /* Return data */
}

void SW_scan(void) {
  uint8_t mask,up,down,in;

  in=(PORT_SW_IN & SW_mask);
  if(in != SW_cache) {
    // a key has changed.
    mask = in ^ SW_cache;
    down=SW_cache & mask;
    up=in&mask;
    if(up) {
      // keys released.
      if(up&1) {SW_store(0x80|0);}
      if(up&2) {SW_store(0x80|1);}
      if(up&4) {SW_store(0x80|2);}
      if(up&8) {SW_store(0x80|3);}
      if(up&16) {SW_store(0x80|4);}
      if(up&32) {SW_store(0x80|5);}
      if(up&64) {SW_store(0x80|6);}
      if(up&128) {SW_store(0x80|7);}
    }
    if(down) {
      // keys pressed.
      if(down&1) {SW_store(0);}
      if(down&2) {SW_store(1);}
      if(down&4) {SW_store(2);}
      if(down&8) {SW_store(3);}
      if(down&16) {SW_store(4);}
      if(down&32) {SW_store(5);}
      if(down&64) {SW_store(6);}
      if(down&128) {SW_store(7);}
    }
    SW_cache=in;
  }
}

