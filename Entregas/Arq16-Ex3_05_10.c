// MICAP T01 2023/2
// Arq16-Ex3 (05/10/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"
#include "fonte.c"

#include <stdio.h>
#include <string.h>


// Assinatura das funções auxiliares
unsigned char glcd_read(__bit, __bit);
void conf_pag(unsigned char, __bit);
void conf_Y(unsigned char, __bit);
void delay_ms(unsigned int);
void delay_us(unsigned int);
void glcd_clear(__bit);
void glcd_init();
void glcd_write(unsigned char, __bit, __bit);
void putchar(char);


// Constantes auxiliares
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


//-----------------------------------------------------------------------------
// Código específico do  Arq16-Ex3

#define R 1
#define W 0

void Timer4_ISR (void) interrupt 16
{
	SMB0CN &= ~0x40;   // Desabilita SMBus
	SMB0CN |=  0x40;   // Habilita SMBus
	TF4 = 0;           // Zera flag de interrupção do TC4
}

unsigned char esc_byte_cntr(unsigned char slave_addr, __bit RW) {
	STA = 1;
	SI  = 0;
	while(SI == 0);

	if(SMB0STA != 0x08 && SMB0STA != 0x10)
		return SMB0STA;

	SMB0DAT = (slave_addr & 0xfe) | RW;
	STA = 0;
	SI  = 0;
	while(SI == 0);

	if(RW == W) {
		if(SMB0STA != 0x18)
			return SMB0STA;
	} else {
		if(SMB0STA != 0x40)
			return SMB0STA;
	}

	return 0;
}

unsigned char esc_byte_dado(unsigned char dado) {
	SMB0DAT = dado;
	SI = 0;
	while(SI == 0);

	if(SMB0STA != 0x28)
		return SMB0STA;

	return 0;
}

int esc_eeprom(unsigned char slave_addr, unsigned char end, unsigned char dado) {
	unsigned char ret;

	ret = esc_byte_cntr(slave_addr, W);
	if(ret != 0)
		return (-(int)ret);

	ret = esc_byte_dado(end);
	if(ret != 0)
		return (-(int)ret);

	ret = esc_byte_dado(dado);
	if(ret != 0)
		return (-(int)ret);

	STO = 1;   // Gera stop
	SI  = 0;
	while (STO == 1);

	while(1) {
		ret = esc_byte_cntr(slave_addr, W);  // ACK Pol.
		if(ret == 0)
			break;
		if(ret != 0x20)
			return (-(int)ret);
	}

	return 0;
}

int le_eeprom(unsigned char slave_addr, unsigned char end) {
	int dado;
	unsigned char ret;

	ret = esc_byte_cntr(slave_addr, W);
	if(ret != 0)
		return (-(int)ret);

	ret = esc_byte_dado(end);
	if(ret != 0)
		return (-(int)ret);

	ret = esc_byte_cntr(slave_addr, R);
	if(ret != 0)
		return (-(int)ret);

	AA = 0;
	SI = 0;
	while (SI==0);   //Aguarda receber byte da EEPROM

	if(SMB0STA != 0x58)
		return (-(int)SMB0STA);

	dado = (int)SMB0DAT;

	STO = 1;   //Gera stop
	SI  = 0;
	while(STO == 1);

	return dado;
}

void main(void) {
	unsigned char cont = 0;

	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	SMB0CN &= ~0x40;   // Desabilita SMBus
	SMB0CN |=  0x40;   // Habilita SMBus

	glcd_init();
	glcd_clear(ESQ);
	glcd_clear(DIR);

	cont = le_eeprom(0xA0, 1);
	while(1) {
		printf_fast_f("\x01 %d    ", (int)cont);
		esc_eeprom(0xA0, 1, cont);
		delay_ms(500);

		if(cont==255)
			cont = 0;
		else
			cont++;
	}
}


///////////////////////////////////////////////////////////////////////////////

unsigned char glcd_read(__bit cd, __bit cs) {
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

void conf_pag(unsigned char x, __bit cs) {

	x &= 0x07;
	x |= 0xb8;

	glcd_write(x, CO, cs);
}

void conf_Y(unsigned char y, __bit cs) {
	y &= 0x7f;
	y |= 0x40;

	glcd_write(y, CO, cs);
}

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

void glcd_clear(__bit cs) {
	unsigned char i, j;

	for(i = 0; i<8; i++){
		conf_pag(i, cs);
		conf_Y(0, cs);
		for(j = 0; j<64; j++)
			glcd_write(0x00, DA, cs);
	}
}

void glcd_init() {
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

void glcd_write(unsigned char byte, __bit cd, __bit cs) {

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
	NOP12();


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
