// MICAP T01 2023/2
// Arq17-Ex1 (17/10/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"

#include <stdio.h>
#include <string.h>


// Assinatura das funções auxiliares
void delay_ms(unsigned int);
void delay_us(unsigned int);
void putchar(char);



//-----------------------------------------------------------------------------
// Código específico do  Arq17-Ex1



void esc_DAC0(float v) {
	unsigned int dac;
	
	dac = v * 1685.5967; // 4096/Vref = 1685.5967	
	DAC0L = dac;
	DAC0H = dac>>8;
}

void main () {
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	while (1)
	{
		esc_DAC0(1.7);
		delay_ms(1000);
		esc_DAC0(2.2);
		delay_ms(1000);
		esc_DAC0(1.3);
		delay_ms(1000);
	}
}


///////////////////////////////////////////////////////////////////////////////


void delay_ms(unsigned int t) {
	TMOD |=	 0x01;
	TMOD &= ~0x02;

	for(; t>0; t--) {
		TR0 = 0;
		TF0 = 0;
		TL0 = 0x58;
		TH0 = 0x9E;

		TR0 = 1;
		while(TF0 == 0);
	}
}

void delay_us(unsigned int t) {
	TMOD |=	 0x01;
	TMOD &= ~0x02;

	for(; t>0; t--) {
		TR0 = 0;
		TF0 = 0;
		TL0 = 0xE7;
		TH0 = 0xE7;

		TR0 = 1;
		while(TF0 == 0);
	}
}

