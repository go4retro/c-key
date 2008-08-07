#include <18F4520.h>
#PRAGMA device ICD=TRUE
#PRAGMA device adc=8
#PRAGMA device HIGH_INTS=TRUE


#PRAGMA FUSES NOWDT                 	//No Watch Dog Timer
#PRAGMA FUSES WDT128                	//Watch Dog Timer uses 1:128 Postscale
#PRAGMA FUSES H4
#PRAGMA FUSES NOPROTECT             	//Code not protected from reading
#PRAGMA FUSES NOBROWNOUT            	//No brownout reset
#PRAGMA FUSES BORV42                	//Brownout reset at 4.2V
#PRAGMA FUSES NOPUT                 	//No Power Up Timer
#PRAGMA FUSES NOCPD                 	//No EE protection
#PRAGMA FUSES NOSTVREN              	//Stack full/underflow will not cause reset
#PRAGMA FUSES DEBUG                 	//Debug mode for use with ICD
#PRAGMA FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#PRAGMA FUSES NOWRT                 	//Program memory not write protected
#PRAGMA FUSES NOWRTD                	//Data EEPROM not write protected
#PRAGMA FUSES NOIESO                	//Internal External Switch Over mode disabled
#PRAGMA FUSES NOFCMEN               	//Fail-safe clock monitor disabled
#PRAGMA FUSES PBADEN                	//PORTB pins are configured as analog input channels on RESET
#PRAGMA FUSES NOWRTC                	//configuration not registers write protected
#PRAGMA FUSES NOWRTB                	//Boot block not write protected
#PRAGMA FUSES NOEBTR                	//Memory not protected from table reads
#PRAGMA FUSES NOEBTRB               	//Boot block not protected from table reads
#PRAGMA FUSES NOCPB                 	//No Boot Block code protection
#PRAGMA FUSES LPT1OSC               	//Timer1 configured for low-power operation
#PRAGMA FUSES MCLR                  	//Master Clear pin enabled
#PRAGMA FUSES NOXINST               	//Extended set extension and Indexed Addressing mode disabled (Legacy mode)

#PRAGMA use delay(clock=40000000)
#PRAGMA use rs232(baud=115200,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)

