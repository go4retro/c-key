S8		mousestate;


void mouse_init(void) {
	mousestate=0;
}


void mouse_handler(void) {
static U16 reinit;
static U8  report[4];
U16	now;
U8 n,c;

	n=ps2_rxavailable0();
	if (!n) {
		if (mousestate) return;
		now=get_timer0();
		if ((now-reinit)<32000) return;		// attempt a reinit once every ~800 ms
		reinit=now;
		printf("Attempt mouse reinit (sending FF)\n\r");
		ps2_send0(0xFF);	// reset command; reply should be FA (acknowledge) followed by self-test result (AA or FC)
		return;
	}
	c=ps2_getrxbyte0();
	if (mousestate<8) printf("{%d %02X} ",mousestate,c);
	switch (mousestate) {
	case 0:	if (c!=0xAA) {mousestate=-1; break;}
			printf("Mouse self-test passed (AA) sending read ID command (F2)");
			ps2_send0(0xF2);	// read ID command; reply should be 00
			break;
	case 1:	if (c!=0x00) {
				printf("Device is not a mouse. Initialization aborted. ");
				mousestate=-1;
				break;
			}
			printf("Device ID is 00 (mouse). Starting setup sequence. ");
			blink_mouse_led();
			ps2_send0(0xE8);			// Set resolution
			break;
	case 2:	ps2_send0(0x03);	break;	// eight counts per mm (200 dpi)
	case 3: ps2_send0(0xE6);	break;	// set scaling 1:1
	case 4:	ps2_send0(0xF3);	break;	// set sample rate
	case 5:	ps2_send0(0x28);	break;	// forty samples per second
	case 6: ps2_send0(0xF4);	break;	// enable
	case 7:						break;
	case 8:	report[0]=c;		break;
	case 9:	report[1]=c;		break;
	case 10:report[2]=c;
			translate_mouse(report[0],report[1],report[2],report[3]);
			mousestate=8-1;
			break;
	default:mousestate=-1;
			break;
	}
	mousestate++;
	if (mousestate<8) printf("\n\r");
}

