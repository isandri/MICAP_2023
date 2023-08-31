#include "config.c"
#include "def_pinos.h"

#define DISP_E P1_0
#define DISP_D P1_1
#define SEGS P0
#define PONTO P0_7
#define LIG 0
#define DES 1

__code unsigned char
disp7[12]={0x7e,0x30,0x6d,0x79,0x33,0x5f,0x70,0x7f,0x7b,0x00,0xff};

void esc_disp7 (unsigned char alg, char dp)
{
	DISP_D=LIG;
	DISP_E=DES;
	if(alg<12) SEGS = disp7[alg];
		else SEGS = 0X4F;
	if(alg==11) 
		dp=1;
	if(dp) 
		PONTO=1;
		else PONTO=0;
}

void delay_ms(unsigned int t){
	TMOD |= 0x01;
	TMOD &= ~0x02;
	for (;t>0;t--){
		TR0 = 0;
		TF0 = 0;
		TL0 = 0x58;
		TH0 = 0x9E;
		TR0 = 1;
		while(TF0==0);
	}
}


void main (void){
	Init_Device();
	SFRPAGE=LEGACY_PAGE;
	esc_disp7(8, 1);
	P0_0=1;
}
