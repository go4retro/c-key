
// This project is compiled with the PCWH compiler v3.249 from CCS. 


#include "config.h"
#include "defs.h"
#include "pinouts.h"
#include "reg-18f4520.h"

// Since CCS 3.249 doesn't have a linker, all source files have to be #INCLUDED in the main source file
#include "ps2.c"
#include "paddles.c"
#include "pulse-io.c"
#include "crosspoint.c"
#include "tables.c"
#include "translation.c"
#include "keyboard.c"
#include "mouse.c"


void main() {
	setup_adc_ports(NO_ANALOGS|VSS_VDD);
	setup_adc(ADC_OFF|ADC_TAD_MUL_0);
	setup_psp(PSP_DISABLED);
	setup_spi(FALSE);
	setup_wdt(WDT_OFF);
	setup_timer_0(RTCC_INTERNAL|RTCC_DIV_256);	// 0.1 us X 256 = 25.6 us per tick X 65536 = 1677.721 ms per period
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
	setup_timer_2(T2_DISABLED,0,1);
	setup_timer_3(T3_DISABLED|T3_DIV_BY_1);
	setup_comparator(NC_NC_NC_NC);
	setup_vref(FALSE);
	setup_low_volt_detect(FALSE);
	setup_oscillator(FALSE);

	output_a  (INITIAL_PORTA_VALUE);
	output_b  (INITIAL_PORTB_VALUE);
	output_c  (INITIAL_PORTC_VALUE);
	output_d  (INITIAL_PORTD_VALUE);
	output_e  (INITIAL_PORTE_VALUE);
	set_tris_a(INITIAL_TRISA_VALUE);
	set_tris_b(INITIAL_TRISB_VALUE);
	set_tris_c(INITIAL_TRISC_VALUE);
	set_tris_d(INITIAL_TRISD_VALUE);
	set_tris_e(INITIAL_TRISE_VALUE);

	printf("Startup\n\r");
	crosspoint_init();
	paddle_init();
	ps2_init();
	enable_interrupts(GLOBAL);
	keyboard_init();
	mouse_init();

	while(1) {
		keyboard_handler();
		mouse_handler();
		pulse_handler();
	}
	sleep();
}
