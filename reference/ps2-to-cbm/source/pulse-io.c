#define	RESTORE_PUSH		0
#define	MOUSE_ACTIVITY		1
#define	RESET_LINE			2
#define	SNAPSHOT_PUSH		3
#define KEYBOARD_ACTIVITY	4

static U8	pulsetime[5];


#PRAGMA use standard_io (C)
#PRAGMA use standard_io (A)

void pulse_handler(void) {
U8		now,diff,i,n;
BOOL	asserted;

	n=sizeof(pulsetime)/sizeof(pulsetime[0]);
	now=make8(get_timer0(),1);
	for (i=0; i<n; i++) {
		if (!pulsetime[i]) continue;
		diff=pulsetime[i]-now;
		asserted=diff<0x80;
		switch (i) {
			case RESTORE_PUSH:		if (asserted) output_low (RESTORE       ); else output_float(RESTORE       ); break;
			case MOUSE_ACTIVITY:	if (asserted) output_low (MOUSE_LED     ); else output_high (MOUSE_LED     ); break;
			case RESET_LINE:		if (asserted) output_high(CBM_RESET_LINE); else output_float(CBM_RESET_LINE); break;
			case SNAPSHOT_PUSH:		if (asserted) output_low (SNAPSHOT_BTN  ); else output_float(SNAPSHOT_BTN  ); break;
			case KEYBOARD_ACTIVITY:	if (asserted) output_low (KEYBOARD_LED  ); else output_high (KEYBOARD_LED  ); break;
			default:                                                                                              break;
		}
		if (!asserted) pulsetime[i]=0;
	}
}

#PRAGMA use fast_io (C)
#PRAGMA use fast_io (A)


void press_restore_key    (void) {pulsetime[RESTORE_PUSH     ]=(make8(get_timer0(),1)+ 8)|1;}
void press_snapshot_button(void) {pulsetime[SNAPSHOT_PUSH    ]=(make8(get_timer0(),1)+ 8)|1;}
void press_reset_button   (void) {pulsetime[RESET_LINE       ]=(make8(get_timer0(),1)+50)|1;}
void blink_keyboard_led   (void) {pulsetime[KEYBOARD_ACTIVITY]=(make8(get_timer0(),1)+ 7)|1;}
void blink_mouse_led      (void) {pulsetime[MOUSE_ACTIVITY   ]=(make8(get_timer0(),1)+ 7)|1;}
