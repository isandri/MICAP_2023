// MICAP T01 2023/2
// Arq15-Ex2 (17/10/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"
#include "fonte.c"

#include <stdio.h>
#include <string.h>

#define NOP4()  NOP();NOP();NOP();NOP()
#define NOP8()  NOP4();NOP4()
#define NOP12() NOP4();NOP8()

#define CS1 P2_0
#define CS2 P2_1
#define RS  P2_2
#define RW  P2_3
#define E   P2_4
#define RST P2_5
#define DB  P4

#define CO  0   // comando
#define DA  1   // dado
#define ESQ 0
#define DIR 1

#define CS P2_3

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

unsigned char glcd_read(__bit cd, __bit cs){
	unsigned char byte;

	RW = 1;
	CS1 = cs;
	CS2 = !cs;
	RS = cd;
	NOP4();

	E = 1;
	NOP8();

	SFRPAGE = CONFIG_PAGE;
	byte = DB;
	SFRPAGE = LEGACY_PAGE;
	NOP4();
	E=0;
	NOP12();

	return (byte);
}


void glcd_write(unsigned char byte, __bit cd, __bit cs){

	while(glcd_read(CO, cs) & 0x80);   // Espera se BUSY é 1

	RW = 0;
	CS1 = cs;
	CS2 = !cs;
	RS = cd;

	SFRPAGE = CONFIG_PAGE;
	DB = byte;
	SFRPAGE = LEGACY_PAGE;
	NOP4();

	E = 1;
	NOP12();
	E = 0;

	SFRPAGE = CONFIG_PAGE;
	DB = 0xff;
	SFRPAGE = LEGACY_PAGE;
	RW = 1;
	NOP12();


}

void glcd_init(){
	// Definindo condições iniciais
	E = 0;
	RST = 1;
	CS1 = 1;
	CS2 = 1;

	SFRPAGE = CONFIG_PAGE;
	DB = 0xff;
	SFRPAGE = LEGACY_PAGE;

	// Verfica se o reset terminou
	while(glcd_read(0, ESQ) & 0x10);
	while(glcd_read(0, DIR) & 0x10);

	glcd_write(0x3f, 0, ESQ);	// Liga display esquerdo
	glcd_write(0xb8, 0, ESQ);	// Zera X
	glcd_write(0x40, 0, ESQ);	// Zera Y
	glcd_write(0xc0, 0, ESQ);	// Zera Z

	glcd_write(0x3f, 0, DIR);	// Liga display direito
	glcd_write(0xb8, 0, DIR);	// Zera X
	glcd_write(0x40, 0, DIR);	// Zera Y
	glcd_write(0xc0, 0, DIR);	// Zera Z
}

void conf_Y(unsigned char y, __bit cs){
	y &= 0x7f;
	y |= 0x40;

	glcd_write(y, CO, cs);
}

void conf_pag(unsigned char x, __bit cs){

	x &= 0x07;
	x |= 0xb8;

	glcd_write(x, CO, cs);
}

void glcd_clear(__bit cs){
	unsigned char i, j;

	for(i = 0; i<8; i++){
		conf_pag(i, cs);
		conf_Y(0, cs);
		for(j = 0; j<64; j++)
			glcd_write(0x00, DA, cs);
	}
}

void putchar (char c) {
	static unsigned char char_cnt;
	__bit lado;

	if(c > 0 && c < 9) {
		char_cnt=0;
		conf_pag(c-1, ESQ);
		conf_pag(c-1, DIR);
		conf_Y(0, ESQ);
		conf_Y(0, DIR);
	} else {

		if (char_cnt < 8) {
			lado = ESQ;
		} else {
			lado = DIR;
		}

		glcd_write(fonte[c-32][0], DA, lado);
		glcd_write(fonte[c-32][1], DA, lado);
		glcd_write(fonte[c-32][2], DA, lado);
		glcd_write(fonte[c-32][3], DA, lado);
		glcd_write(fonte[c-32][4], DA, lado);

		glcd_write(0x00, DA, lado);
		glcd_write(0x00, DA, lado);
		glcd_write(0x00, DA, lado);

		char_cnt++;
	}
}

unsigned char le_RAM_SPI(unsigned int end) {
	unsigned char end_L, end_H;

	end_L = end;
	end_H = end>>8;

	CS = 0;

	SPI0DAT = 0X03;
	while(!TXBMT);
	SPI0DAT = end_H;
	while(!TXBMT);
	SPI0DAT = end_L;
	while(!TXBMT);
	SPI0DAT = 0X00;
	while(!TXBMT);
	SPIF = 0;
	while(!SPIF);
	SPIF = 0;
	CS = 1;

	return (SPI0DAT);
}


void esc_RAM_SPI (unsigned int end, unsigned char dado) {
	unsigned char end_L, end_H;

	end_L=end;
	end_H=end>>8;

	CS = 0;

	SPI0DAT = 0X02;
	while(!TXBMT);
	SPI0DAT = end_H;
	while(!TXBMT);
	SPI0DAT = end_L;
	while(!TXBMT);
	SPI0DAT = dado;
	while(!TXBMT);
	SPIF = 0;
	while(!SPIF);
	SPIF = 0;
	CS = 1;
}

void main (void) {
	unsigned int end, er=0;
	
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	glcd_init();
	glcd_clear(ESQ);
	glcd_clear(DIR);

	printf_fast_f("\x01 FACOM-EC-MICAP ");

	for(end=0; end<8192; end++){
		if(end%100==0)
			printf_fast_f("\x02 %05u", end);

		esc_RAM_SPI(end, 170);

		if(le_RAM_SPI(end) != 170){
			er=1;
			break;
		}

		esc_RAM_SPI(end, 85);

		if(le_RAM_SPI(end) != 85){
			er=1;
			break;
		}
	}

	if(er){
		printf_fast_f("\x03 %05u ", end);
		printf_fast_f("Erro");
	}
	else {
		printf_fast_f("\x03 fim");
	}
	while(1);
}
