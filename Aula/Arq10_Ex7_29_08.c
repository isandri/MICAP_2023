// MICAP T01 2023/2
// Arq10-Ex(4-5-6-7) (29/08/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"
#include <stdio.h>
#include <string.h>

#define NOP4() NOP();NOP();NOP();NOP() 
#define NOP8() NOP4();NOP4() 
#define NOP12() NOP4();NOP8() 

#define CS1 P2_0
#define CS2 P2_1
#define RS P2_2
#define RW P2_3
#define E P2_4
#define RST P2_5
#define DB P4

#define CO 0    // comando
#define DA 1    // comando

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
unsigned char l_gLCD(__bit cd, __bit cs){
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

// Ex3
void e_gLCD(unsigned char byte, __bit cd, __bit cs){
	
	while(l_gLCD(CO, cs) & 0x80); // Espera se BUSY é 1
	
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

// Ex4
void ini_LCDg(){
	// definindo condições iniciais
	E = 0;
	RST = 1;
	CS1 = 1;
	CS2 = 1;
	
	SFRPAGE = CONFIG_PAGE;
	DB = 0xff;
	SFRPAGE = LEGACY_PAGE;

	while(l_gLCD(0, 0) & 0x10); // verfica se o reset terminou
	while(l_gLCD(0, 1) & 0x10);


	e_gLCD(0x3f, 0, 0); // liga o display
	e_gLCD(0x3f, 0, 1);

	e_gLCD(0xb8, 0, 0); // zera x
	e_gLCD(0x40 , 0, 0); // zera y
	e_gLCD(0xc0, 0, 0); //zera z
	
	e_gLCD(0xb8, 0, 1); // zera x
	e_gLCD(0x40 , 0, 1); // zera y
	e_gLCD(0xc0, 0, 1); //zera z
}

// Ex5
void conf_Y(unsigned char y, __bit cs){
	y &= 0x7f;
	y |= 0x40;
	
	e_gLCD(y, CO, cs);
}

// Ex5
void conf_page(unsigned char x, __bit cs){
	
	x &= 0x07;
	x |= 0xb8;
	
	e_gLCD(x, CO, cs);
}

// Ex6
void limpa_gLCD(__bit cs){
	unsigned char i, j;

	for(i = 0; i<8; i++){
		conf_page(i, cs);
		conf_Y(0, cs);
		for(j = 0; j<64; j++)
			e_gLCD(0x00, DA, cs);
	}
}

// Ex7
void main(void) {

	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	ini_LCDg();

	limpa_gLCD(0);
	limpa_gLCD(1);

	conf_page(0, 0);
	conf_Y(0, 0);

	e_gLCD(0x7e, DA, 0);
	e_gLCD(0x11, DA, 0);
	e_gLCD(0x11, DA, 0);
	e_gLCD(0x11, DA, 0);
	e_gLCD(0x7e, DA, 0);

	e_gLCD(0x00, DA, 0);
	e_gLCD(0x00, DA, 0);
	e_gLCD(0x00, DA, 0);

	e_gLCD(0x7f, DA, 0);
	e_gLCD(0x49, DA, 0);
	e_gLCD(0x49, DA, 0);
	e_gLCD(0x49, DA, 0);
	e_gLCD(0x36, DA, 0);
	while(1);

}
