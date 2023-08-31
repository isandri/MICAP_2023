// MICAP T01 2023/2
// Arq7-Ex2 (17/08/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"

#define DISP_E P1_0
#define DISP_D P1_1
#define SEGS P0
#define PONTO P0_7
#define LIG 0
#define DES 1

__code unsigned char caract[13] = {0x7E, 0x30, 0x6D, 0x79,
                                   0x33, 0x5B, 0x5F, 0x70,
                                   0x7F, 0x7B, 0x00, 0xFF,
                                   0x4F};

unsigned char disp1;
unsigned char disp2;
unsigned char select=1;


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

void funcao_isr (void) __interrupt 5 {
	if(select == 1){
		P1_1=1;
		P1_0=0;
		P0=caract[disp1];
		select=2;
	}
	else if(select == 2){
		P1_0=1;
		P1_1=0;
		P0=caract[disp2];
		select=1;
	}
}


void esc_disp7s(unsigned int num_vis){
	disp1 = num_vis/10;
	disp2 = num_vis%10;
	
}

void main (void){
	int i;
	Init_Device();
	SFRPAGE=LEGACY_PAGE;
	

	for(i=0; i<100;i++){
		esc_disp7s(i);
		delay_ms(500);
	}
}