// MICAP T01 2023/2
// Arq6-Ex6 (15/08/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"

__code unsigned char caract[44][5] = {{0xff, 0x81, 0x81, 0xff, 0x00},  // 0
									  {0x84, 0x82, 0xff, 0x80, 0x00},  // 1
									  {0xc6, 0xa1, 0x91, 0x8e, 0x00},  // 2
									  {0x42, 0x99, 0x99, 0x66, 0x00},  // 3
									  {0x1f, 0x10, 0x10, 0xff, 0x00},  // 4
									  {0x8e, 0x89, 0x89, 0x79, 0x00},  // 5
									  {0x7e, 0x89, 0x89, 0x79, 0x00},  // 6
									  {0xc1, 0x31, 0x19, 0x0f, 0x00},  // 7
									  {0x66, 0x99, 0x99, 0x66, 0x00},  // 8
									  {0x4e, 0x91, 0x91, 0x7e, 0x00},  // 9

									  {0x00, 0x66, 0x66, 0x00, 0x00},  // :
									  {0x00, 0x46, 0x26, 0x00, 0x00},  // ;
									  {0x18, 0x24, 0x42, 0x81, 0x00},  // <
									  {0x66, 0x66, 0x66, 0x66, 0x00},  // =
									  {0x81, 0x42, 0x24, 0x18, 0x00},  // >
									  {0x06, 0xa1, 0x11, 0x0e, 0x00},  // ?
									  {0x7e, 0x9d, 0xa3, 0x3e, 0x00},  // @

									  {0xfe, 0x11, 0x11, 0xfe, 0x00},  // A
									  {0xff, 0x99, 0x99, 0x66, 0x00},  // B
									  {0x7e, 0x81, 0x81, 0x81, 0x00},  // C
									  {0xff, 0x81, 0x81, 0x7e, 0x00},  // D
									  {0xff, 0x99, 0x99, 0x99, 0x00},  // E
									  {0xff, 0x19, 0x19, 0x19, 0x00},  // F
									  {0x7e, 0x81, 0x91, 0x72, 0x00},  // G
									  {0xff, 0x18, 0x18, 0xff, 0x00},  // H
									  {0x81, 0xff, 0xff, 0x81, 0x00},  // I
									  {0xe1, 0x81, 0xff, 0x01, 0x00},  // J
									  {0xff, 0x18, 0x36, 0xc1, 0x00},  // K
									  {0xff, 0xc0, 0xc0, 0xc0, 0x00},  // L
									  {0xff, 0x0e, 0x0e, 0xff, 0x00},  // M
									  {0xff, 0x0c, 0x30, 0xff, 0x00},  // N
									  {0x7e, 0x81, 0x81, 0x7e, 0x00},  // O
									  {0xfe, 0x11, 0x11, 0x0e, 0x00},  // P
									  {0x3e, 0x41, 0x41, 0xbe, 0x00},  // Q
									  {0xff, 0x11, 0x31, 0xce, 0x00},  // R
									  {0x8e, 0x99, 0x99, 0x71, 0x00},  // S
									  {0x03, 0xff, 0xff, 0x03, 0x00},  // T
									  {0x7f, 0xc0, 0xc0, 0x7f, 0x00},  // U
									  {0x3f, 0xc0, 0xc0, 0x3f, 0x00},  // V
									  {0xff, 0x70, 0x70, 0xff, 0x00},  // W
									  {0xe7, 0x18, 0x18, 0xe7, 0x00},  // X
									  {0x0f, 0xf0, 0xf0, 0x0f, 0x00},  // Y
									  {0xc1, 0xb1, 0x99, 0x87, 0x00},  // Z

									  {0x00, 0x00, 0x00, 0x00, 0x00}}; // <espa�o>


void mostra_car(unsigned char lin) {
	SFRPAGE = LEGACY_PAGE;

	P0 = caract[lin][0];
	P1 = caract[lin][1];
	P2 = caract[lin][2];
	P3 = caract[lin][3];
}

void mostra_car2(unsigned char c) {
	if (c >= 48 && c <= 90)
		mostra_car(c - 48);
	else
		mostra_car(43);
}

void delay_ms(unsigned int t) {
	SFRPAGE = LEGACY_PAGE;

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

void print_LEDs(unsigned char* pont) {
	unsigned char i=0;

	while(pont[i] != 0) {
		mostra_car2(pont[i]);
		i++;
		delay_ms(100);
	}
}

void desloca_texto(unsigned char* pont, unsigned int t) {
	unsigned char i=0;   // Caractere atual
	unsigned char j=0;   // Coluna do caractere atual
	unsigned int  aux;

	SFRPAGE = LEGACY_PAGE;
	P0 = 0;
	P1 = 0;
	P2 = 0;
	P3 = 0;
	SFRPAGE = CONFIG_PAGE;
	P4 = 0;
	P5 = 0;
	P6 = 0;
	P7 = 0;
	SFRPAGE = LEGACY_PAGE;

	while(pont[i] != 0) {
		P0 = P1;
		P1 = P2;
		P2 = P3;
		SFRPAGE = CONFIG_PAGE;
		aux = P4;
		SFRPAGE = LEGACY_PAGE;
		P3 = aux;
		SFRPAGE = CONFIG_PAGE;
		P4 = P5;
		P5 = P6;
		P6 = P7;
		P7 = caract[pont[i]-48][j];

		j++;
		delay_ms(t);

		if(j > 4) {
			// Avan�a p/ pr�ximo caractere
			j = 0;
			i++;
		}
	}

	// Fim da string, esvazia visor
	for(i=0; i<8; i++) {
		P0 = P1;
		P1 = P2;
		P2 = P3;
		SFRPAGE = CONFIG_PAGE;
		aux = P4;
		SFRPAGE = LEGACY_PAGE;
		P3 = aux;
		SFRPAGE = CONFIG_PAGE;
		P4 = P5;
		P5 = P6;
		P6 = P7;
		P7 = 0;
		delay_ms(t);
	}
}

void main(void) {
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	desloca_texto("ISA:><:NERD?", 150);
}