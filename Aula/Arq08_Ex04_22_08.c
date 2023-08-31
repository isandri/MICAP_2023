// MICAP T01 2023/2
// Arq8-Ex(3-4) (22/08/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"

#define RS P3_0
#define E  P3_1
#define DB P3   // LCD Alfanumérico
#define NI 0    // nibble
#define BY 1    // byte
#define CO 0    // comando
#define DT 1    // comando

void delay_ms(unsigned int t) {
	TMOD |=  0x01;
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
	TMOD |=  0x01;
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

// Ex2
void esc_LCDan(unsigned char dado, __bit nb, __bit cd) {
	unsigned char aux;

	RS = cd;
	NOP();
	E = 1;
 
	aux = (dado >> 2) & 0xfc;
	aux = aux | 0x02 | cd;
	DB  = aux;
	NOP();
	E = 0;
	
	if(nb) {
		delay_us(1);
		aux = (dado << 2) & 0xfc;
		aux = aux | 0x02 | cd;
		DB  = aux;
		NOP();
		E = 0;
	} 
	
	if(dado < 4 && cd == CO) {
	 	delay_us(1520);
	} else {
		delay_us(43);
	}

}

// Ex3
void ini_LCDan(void) {
	E=0;
	delay_ms(16);

	esc_LCDan(0x30, NI, CO);
	delay_ms(5);

	esc_LCDan(0x30, NI, CO);
	delay_us(100);

	esc_LCDan(0x30, NI, CO);
	esc_LCDan(0x20, NI, CO);

	esc_LCDan(0x28, BY, CO); // Function Set
	esc_LCDan(0x08, BY, CO); // Display Off
	esc_LCDan(0x01, BY, CO); // Display Clear
	esc_LCDan(0x06, BY, CO); // Entry Mode Set
	esc_LCDan(0x0c, BY, CO); // Liga LCD

}

//  Ex4
void main(void) {
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	ini_LCDan();
	esc_LCDan(0x80, BY, CO); // Set DDRAM = 0x00

	esc_LCDan(24, BY, DT);
	esc_LCDan('I', BY, DT);
	esc_LCDan('s', BY, DT);
	esc_LCDan('a', BY, DT);
	esc_LCDan('d', BY, DT);
	esc_LCDan('o', BY, DT);
	esc_LCDan('r', BY, DT);
	esc_LCDan('a', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan('S', BY, DT);
	esc_LCDan('.', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan('S', BY, DT);
	esc_LCDan('.', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan(0xCE, BY, DT);

	esc_LCDan(0xc0, BY, CO); // Set DDRAM = 0x40

	esc_LCDan(21, BY, DT);
	esc_LCDan('M', BY, DT);
	esc_LCDan('a', BY, DT);
	esc_LCDan('t', BY, DT);
	esc_LCDan('e', BY, DT);
	esc_LCDan('u', BY, DT);
	esc_LCDan('s', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan('B', BY, DT);
	esc_LCDan('.', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan('C', BY, DT);
	esc_LCDan('.', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan(' ', BY, DT);
	esc_LCDan(0xCF, BY, DT);

	while(1) delay_ms(100);

}
