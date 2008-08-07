S8 kbstate;


void keyboard_init() {
	kbstate=0;
}


void keyboard_handler(void) {
static U16 reinit;
U16	now;
U8 n,c;

	n=ps2_rxavailable1();
	if (!n) {
		if (kbstate) return;
		now=get_timer0();
		if ((now-reinit)<32000) return;		// attempt a reinit once every ~800 ms
		reinit=now;
		printf("Attempt keyboard reinit (sending FF)\n\r");
		ps2_send1(0xFF);	// reset command; reply should be FA (acknowledge) followed by self-test result (AA or FC)
		return;
	}
	c=ps2_getrxbyte1();
	printf("(%d %02X) ",kbstate,c);
	switch (kbstate) {
	case 0:	if (c!=0xAA) {kbstate=-1; break;}
			printf("Keyboard self-test passed (AA) sending read ID command (F2)");
			ps2_send1(0xF2);	//  read ID command; reply should be FA (acknowledge) AB 83 (keyboard ID)
			break;
	case 1:	if (c==0xFA) break;
			kbstate=-1;
			break;
	case 2:	if (c==0xAB) break;
			printf("Device is not a keyboard. Initialization aborted. ");
			kbstate=-1;
			break;
	case 3:	if (c==0x83) {blink_keyboard_led(); break;}
			kbstate=-1;
			break;
	case 4:	translate_keyboard(c);
			kbstate--;
			break;
	default:kbstate=-1;
			break;
	}
	printf("\n\r");
	kbstate++;
}
