
U16			paddle1local,paddle2local,paddle1value,paddle2value;
BOOL		paddle1phase,paddle2phase,paddle1sync,paddle2sync,paddleupdate;
static U16	resync1,resync2;


#PRAGMA use standard_io (C)

void paddle_init(void) {
	paddle1value=3440;		// range is from 2460==>0		4880 ==>246
	paddle2value=3440;
	paddle1local=paddle1value;
	paddle2local=paddle2value;
	paddle1sync=FALSE;
	paddle2sync=FALSE;
	paddleupdate=FALSE;
	enable_interrupts(INT_CCP1);
	enable_interrupts(INT_CCP2);
	enable_interrupts(INT_TIMER1);
}

#PRAGMA int_timer1
void paddle_sync_check(void) {	// runs once every 13 SID cycles and forces a resync if unplugged & replugged
	if (paddle1sync) {
		paddle1sync=FALSE;
	} else {
		output_low(PIN_C2);
		CCP_1=paddle1local;
		setup_ccp1(CCP_COMPARE_SET_ON_MATCH);
		paddle1phase=FALSE;
		resync1++;
	}
	if (paddle2sync) {
		paddle2sync=FALSE;
	} else {
		output_low(PIN_C1);
		CCP_2=paddle2local;
		setup_ccp2(CCP_COMPARE_SET_ON_MATCH);
		paddle2phase=FALSE;
		resync2++;
	}
	if (paddleupdate) {
		paddle1local=paddle1value;
		paddle2local=paddle2value;
	}
}


#PRAGMA int_ccp1 NOCLEAR
void paddle1_done(void) {
	if (paddle1phase) {
	    output_low(PIN_C2);
	    CCP_1+=paddle1local;
	    setup_ccp1(CCP_COMPARE_SET_ON_MATCH);
	    CCP1IF=0;
	    paddle1phase=FALSE;
	    paddle1sync=TRUE;
	} else {
		CCP1IF=0;
		setup_ccp1(CCP_CAPTURE_FE);
		delay_cycles(1);
		output_float(PIN_C2);
		paddle1phase=TRUE;
	}
}

#PRAGMA int_ccp2 NOCLEAR
void paddle2_done(void) {
	if (paddle2phase) {
	    output_low(PIN_C1);
	    CCP_2+=paddle2local;
	    setup_ccp2(CCP_COMPARE_SET_ON_MATCH);
	    CCP2IF=0;
	    paddle2phase=FALSE;
	    paddle2sync=TRUE;
	} else {
		delay_us(5);
		CCP2IF=0;
		setup_ccp2(CCP_CAPTURE_FE);
		delay_cycles(1);
		output_float(PIN_C1);
		paddle2phase=TRUE;
	}
}

#PRAGMA use fast_io (C)

