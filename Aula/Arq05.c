// MICAP T01 2023/2
// Arq5-Ex(5-7) (08/08/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"

// Ex 5
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

// Ex 7
void delay_us(unsigned int t){
	TR0 = 0;
	TF0 = 0;
	TMOD |= 0x02;
	TMOD &= ~0x01;
	TL0 = 231; //e7h
	TH0 = 231;
	TR0 = 1;

	for (;t>0;t--){
		while(TF0==0);
		TF0=0;
	}
}

void main (void){
	Init_Device();
	SFRPAGE=LEGACY_PAGE;
	P0=0;
	while (1)
	{
		P0_0=1;
		delay_us(65000);
		P0_0=0;
		delay_ms(1000);
	}
}