
// 12 rows (AX) and 8 columns (AY)
// three LSb are column (0-7) and four MSb are row (0-11)

void connect(U8 value, BOOL connect) {
static const U8 rowval[12]={0,1,2,3,4,5,8,9,10,11,12,13};
U8	row,col,ax,ay;

	if (value>=0x5F) return;

	col=value& 7;	// 0 -  7
	row=value>>3;	// 0 - 11

	ax=rowval[row];	// 0 - 15
	ay=col;

	value=ax<<3;
	value|=ay;


	printf("[%d %d %c] ",ay,ax,connect?'+':'-');
	if (bit_test(LATD,7)) bit_set(value,7); else bit_clear(value,7);
	output_d(value);
	if (connect) output_high(CROSSPOINT_DATA);
	else         output_low (CROSSPOINT_DATA);

	delay_cycles(1);
	output_high(CROSSPOINT_STROBE);
	delay_cycles(1);
	output_low(CROSSPOINT_STROBE);
}


void crosspoint_init(void) {
	output_low(CROSSPOINT_DATA);
	output_low(CROSSPOINT_STROBE);
	output_low(CROSSPOINT_RESET);
	delay_cycles(5);
	output_high(CROSSPOINT_RESET);
	delay_cycles(2);
	output_low(CROSSPOINT_RESET);
}
