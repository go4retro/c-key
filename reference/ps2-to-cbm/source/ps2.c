
static U8 ps2buf0[20],ps2buf1[20];
static U8 *ps2wp0,*ps2rp0,*ps2wp1,*ps2rp1;


#PRAGMA INT_EXT HIGH
void ps2clk0_fallingedge_interrupt(void) {
static U16  prevedge,now;
static U8	bit,rxshift;
static BOOL	indata;

	// sample as close to the falling edge as possible (will be about 40 cycles (4us) for non-fast interrupts)
	indata=input(PS2_DATA0);
	// typical time between falling edges is 83us (12 kHz clock)
	// this will add some degree of noise immunity by clearing out the shift register if
	// it's been a long time (more than 0.1 us X 256 X 20 = 512 us) since the last edge
	// wraparound is not handled so any noise that occured during the 512 us window around X*1677.7216 ms ago is not filtered
	now=get_timer0();
	if ((now-prevedge)>20) bit=0;
	prevedge=now;
	if (!bit) rxshift=0;
	rxshift>>=1;
	if (indata) bit_set(rxshift,7);
	bit++;
	if (bit==9) {
		ps2buf0[ps2wp0]=rxshift;
		ps2wp0++;
		if (ps2wp0==sizeof(ps2buf0)) ps2wp0=0;
	}
	if (bit==11) bit=0;
}

#PRAGMA INT_EXT1 HIGH
void ps2clk1_fallingedge_interrupt(void) {
static U16  prevedge,now;
static U8	bit,rxshift;
static BOOL	indata;

	// sample as close to the falling edge as possible (will be about 40 cycles (4us) for non-fast interrupts)
	indata=input(PS2_DATA1);
	// typical time between falling edges is 83us (12 kHz clock)
	// this will add some degree of noise immunity by clearing out the shift register if
	// it's been a long time (more than 0.1 us X 256 X 20 = 512 us) since the last edge
	// wraparound is not handled so any noise that occured during the 512 us window around X*1677.7216 ms ago is not filtered
	now=get_timer0();
	if ((now-prevedge)>20) bit=0;
	prevedge=now;
	if (!bit) rxshift=0;
	rxshift>>=1;
	if (indata) bit_set(rxshift,7);
	bit++;
	if (bit==9) {
		ps2buf1[ps2wp1]=rxshift;
		ps2wp1++;
		if (ps2wp1==sizeof(ps2buf1)) ps2wp1=0;
	}
	if (bit==11) bit=0;
}


#define	CLKHI	0
#define	CLKLO	1
#define	DATAHI	2
#define	DATALO	3

BOOL waitfor_ps2_0(U8 state) {
U16	start,now;
	start=get_timer0();
	do {
		switch (state) {
			case CLKHI:  if ( input(PS2_CLK0 )) return(TRUE); break;
			case CLKLO:  if (!input(PS2_CLK0 )) return(TRUE); break;
			case DATAHI: if ( input(PS2_DATA0)) return(TRUE); break;
			case DATALO: if (!input(PS2_DATA0)) return(TRUE); break;
		}
		now=get_timer0();
	} while (now-start<196);	// waiting more than 5ms means the keypad is probably unplugged
	return(FALSE);
}

BOOL waitfor_ps2_1(U8 state) {
U16	start,now;
	start=get_timer0();
	do {
		switch (state) {
			case CLKHI:  if ( input(PS2_CLK1 )) return(TRUE); break;
			case CLKLO:  if (!input(PS2_CLK1 )) return(TRUE); break;
			case DATAHI: if ( input(PS2_DATA1)) return(TRUE); break;
			case DATALO: if (!input(PS2_DATA1)) return(TRUE); break;
		}
		now=get_timer0();
	} while (now-start<196);	// waiting more than 5ms means the keypad is probably unplugged
	return(FALSE);
}

#PRAGMA use standard_io (PS2_CLK_PORT)
#PRAGMA use standard_io (PS2_DATA_PORT)

// normally will take about 3ms but can take 120ms if the keypad is unplugged
BOOL ps2_send0(U8 txchar) {
U16		txshift;
BOOL	parity,status;
U8		i;

	txshift=txchar;
	parity=1;
	for (i=0; i<8; i++) {
		if (bit_test(txchar,0)) parity^=1;
		txchar>>=1;
	}
	if (parity) bit_set(txshift,8);	// parity bit
	bit_set(txshift,9);				// stop bit
	status=TRUE;
	disable_interrupts(INT_EXT);
	output_low(PS2_CLK0);
	delay_us(110);
	output_float(PS2_CLK0);
	output_low(PS2_DATA0);
	for (i=0; i<10; i++) {
		status&=waitfor_ps2_0(CLKLO);
		if (bit_test(txshift,0)) output_float(PS2_DATA0);
		else 		             output_low  (PS2_DATA0);
		txshift>>=1;
		status&=waitfor_ps2_0(CLKHI);
	}
	status&=waitfor_ps2_0(DATALO);
	status&=waitfor_ps2_0(CLKLO);
	status&=waitfor_ps2_0(CLKHI);
	status&=waitfor_ps2_0(DATAHI);
	INT0IF=0;
	enable_interrupts(INT_EXT);
	return(status);
}

BOOL ps2_send1(U8 txchar) {
U16		txshift;
BOOL	parity,status;
U8		i;

	txshift=txchar;
	parity=1;
	for (i=0; i<8; i++) {
		if (bit_test(txchar,0)) parity^=1;
		txchar>>=1;
	}
	if (parity) bit_set(txshift,8);	// parity bit
	bit_set(txshift,9);				// stop bit
	status=TRUE;
	disable_interrupts(INT_EXT1);
	output_low(PS2_CLK1);
	delay_us(110);
	output_float(PS2_CLK1);
	output_low(PS2_DATA1);
	for (i=0; i<10; i++) {
		status&=waitfor_ps2_1(CLKLO);
		if (bit_test(txshift,0)) output_float(PS2_DATA1);
		else 		             output_low  (PS2_DATA1);
		txshift>>=1;
		status&=waitfor_ps2_1(CLKHI);
	}
	status&=waitfor_ps2_1(DATALO);
	status&=waitfor_ps2_1(CLKLO);
	status&=waitfor_ps2_1(CLKHI);
	status&=waitfor_ps2_1(DATAHI);
	INT1IF=0;
	enable_interrupts(INT_EXT1);
	return(status);
}

void ps2_suspend0(void) {
	disable_interrupts(INT_EXT);
	output_low(PS2_CLK0);
	INT0IF=0;
}

void ps2_suspend1(void) {
	disable_interrupts(INT_EXT1);
	output_low(PS2_CLK1);
	INT1IF=0;
}

void ps2_resume0(void) {
	output_float(PS2_CLK0);			// data will start flowing in about 800 us
	INT0IF=0;
	enable_interrupts(INT_EXT);
}

void ps2_resume1(void) {
	output_float(PS2_CLK1);			// data will start flowing in about 800 us
	INT1IF=0;
	enable_interrupts(INT_EXT1);
}

#PRAGMA use fast_io (PS2_CLK_PORT)
#PRAGMA use fast_io (PS2_DATA_PORT)


void ps2_init(void) {
	ps2wp0=0;
	ps2rp0=0;
	ps2wp1=0;
	ps2rp1=0;
	ext_int_edge(0,H_TO_L);
	ext_int_edge(1,H_TO_L);
	enable_interrupts(INT_EXT);
	enable_interrupts(INT_EXT1);
}


U8 ps2_rxavailable1(void) {
U8	w,r;
	disable_interrupts(GLOBAL);
	w=ps2wp1;	// create a local copy since the ISR owns the global one
	enable_interrupts(GLOBAL);
	r=ps2rp1;
	if (w>=r) return(w-r);
	else      return(w+sizeof(ps2buf1)-r);
}

U8 ps2_rxavailable0(void) {
U8	w,r;
	disable_interrupts(GLOBAL);
	w=ps2wp0;	// create a local copy since the ISR owns the global one
	enable_interrupts(GLOBAL);
	r=ps2rp0;
	if (w>=r) return(w-r);
	else      return(w+sizeof(ps2buf0)-r);
}

U8	ps2_getrxbyte1(void) {
U8	x;
	x=ps2buf1[ps2rp1];
	ps2rp1++;
	if (ps2rp1==sizeof(ps2buf1)) ps2rp1=0;
	return(x);
}

U8	ps2_getrxbyte0(void) {
U8	x;
	x=ps2buf0[ps2rp0];
	ps2rp0++;
	if (ps2rp0==sizeof(ps2buf0)) ps2rp0=0;
	return(x);
}


BOOL set_keyboard_lights(BOOL numlock, BOOL capslock, BOOL scrollock) {
BOOL status;
U8	cmd;
	cmd=0;
	if (numlock  ) bit_set(cmd,1);
	if (capslock ) bit_set(cmd,2);
	if (scrollock) bit_set(cmd,0);
	status=ps2_send1(0xED);
    delay_ms(2);
    status&=ps2_send1(cmd);
    delay_ms(2);
	return(status);
}
