
void translate_keyboard(U8 scancode) {
static BOOL leftshift,rightshift,leftcontrol,rightcontrol,leftalt,rightalt,capslock,numlock,scrolllock;
static BOOL	release,extcode,pausekey;
U8 tableindex,action,category,argument;

	action=0;
	switch(scancode) {
		case 0xE0:
			extcode=TRUE;
			break;
		case 0xE1:
			pausekey=TRUE;
			break;
		case 0xF0:
			release=TRUE;
			return;
		case 0xAB:		//keyboard ID is AB 83
		case 0x00:		//overflow
		case 0xAA:		//self-test passed
		case 0xEE:		//echo
		case 0xFA:		//ack
		case 0xFC:		//self-test failed
		case 0xFE:		//resend
		case 0xFF:		//key detect error or overflow
			break;
		default:
			if  (scancode==0x12)              leftshift   =!release;
			if  (scancode==0x59)              rightshift  =!release;
			if ((scancode==0x14) && !extcode) leftcontrol =!release;
			if ((scancode==0x14) &&  extcode) rightcontrol=!release;
			if ((scancode==0x77) && pausekey) {pausekey   =!release; break;}
			if ((scancode==0x11) &&  extcode) rightalt    =!release;
			if ((scancode==0x11) && !extcode) leftalt     =!release;
			tableindex=scancode;
			if (extcode ) tableindex+=0x80;
			if (pausekey) tableindex+=0xC0;
			action=translation[tableindex];
			printf("[%02X %02X] ",tableindex,action);
			extcode=FALSE;
			break;
	}
	category=action&0xC0;
	argument=action&0x3F;
	if (!release) blink_keyboard_led();
	switch (category) {
	    case REMAP:	// Standard key - connect or disconnect one crosspoint
			connect(argument,!release);
            break;
		case SPECIAL:// Extended categories
            category=(action&0x38);
            argument=(action&0x07);
            switch(category) {
                case RESTORE_KEY:     // Restore button
                    if (release) break;
                    printf(" RESTORE ");
                    press_restore_key();
                    break;
                case PUNCTUATION1:     // Translate diffrently if shifted
                case PUNCTUATION2:     // Translate diffrently if shifted
                    if (category==PUNCTUATION2) argument+=8;
                    if (!leftshift && !rightshift) {
                        argument=punctuation_norm[argument];
                        if (bit_test(argument,6)) connect(0x34,!release);
                    } else {
                        argument=punctuation_swap[argument];
                        if (!bit_test(argument,6)) {
                            if (!release) {
                                if (leftshift ) connect(0x0F,FALSE);    // release left shift
                                if (rightshift) connect(0x34,FALSE);    // release right shift
                            } else {
                                if (leftshift ) connect(0x0F,TRUE);     // press left shift
                                if (rightshift) connect(0x34,TRUE);     // press right shift
                            }
                        }
                    }
                    bit_clear(argument,6);
                    connect(argument,!release);
                    break;
                case LOCK_KEY:     // Caps lock, Num Lock, Scroll Lock
                    if (release) break;
                    if (argument==1) capslock^=1;
                    if (argument==2) {numlock^=1; output_bit(JOYSWITCH_SELECT,~input(JOYSWITCH_SELECT));}
                    if (argument==3) scrolllock^=1;
                    set_keyboard_lights(numlock,capslock,scrolllock);
                    break;
                case SNAPSHOT:
				    if (release) break;
                    press_snapshot_button();
                    break;
                case CBM_RESET:
				    if (release) break;
	                if ((leftcontrol || rightcontrol) && (leftalt || rightalt)) press_reset_button();
	                break;
				case RELEASE_ALL:
					if (release) break;
					crosspoint_init();
					break;
                default:
					printf("Nothing defined for subcategory %d ",category);
					break;
            }
            break;
	    case SHIFT_REMAP:		// Shifted key - press SHIFT then press the key or release the key and then release SHIFT
            if (!release) connect(0x34,TRUE);       // press left shift
            connect(argument,!release);
            if (release) connect(0x34,FALSE);       // release left shift
            break;
	    case IGNORE:// Ignored key - do nothing
	    default:
		    break;
	}
	release=FALSE;
}

#define PADDLEMIN	3250
#define PADDLEMAX	4500

void translate_mouse(U8 a, U8 b, U8 c, U8 d) {
BOOL	left,middle,right;
S16		dx,dy;

	left  =bit_test(a,0);
	right =bit_test(a,1);
	middle=bit_test(a,2);
	if (bit_test(a,4)) dx=make16(-1,b); else dx=make16(0,b);
	if (bit_test(a,5)) dy=make16(-1,c); else dy=make16(0,c);

	paddle1value+=dx*9;
	paddle2value+=dy*9;
	if (paddle1value>PADDLEMAX) paddle1value=PADDLEMIN;
	if (paddle2value>PADDLEMAX) paddle2value=PADDLEMIN;
	if (paddle1value<PADDLEMIN) paddle1value=PADDLEMAX;
	if (paddle2value<PADDLEMIN) paddle2value=PADDLEMAX;
	paddleupdate=TRUE;

	// JOYSTICK0=1 (UP)	JOYSTICK1=2	(DOWN) JOYSTICK2=4 (LEFT) JOYSTICK3=8 (RIGHT) JOYSTICK4=16 (FIRE) (commodore 1351 mouse uses LEFT=16 RIGHT=1)

	#use standard_io (JOYSTICK_IO_PORT)
	if (left)   output_low(JOYSTICK4); else output_float(JOYSTICK4);
	if (middle) output_low(JOYSTICK2); else output_float(JOYSTICK2);
	if (right)  output_low(JOYSTICK0); else output_float(JOYSTICK0);
	#use fast_io (JOYSTICK_IO_PORT)

	blink_mouse_led();
	printf("[%3ld %3ld] %6ld %6ld  %d %d %d  ",dx,dy,paddle1value,paddle2value,left,middle,right);
	if (bit_test(a,7)) printf("Y Overflow ");
	if (bit_test(a,6)) printf("X Overflow ");
	printf("\n\r");
}
