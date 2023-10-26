// MICAP T01 2023/2
// Arq14-Ex4 (26/10/2023)
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

#define AIN0_0 0
#define AIN0_1 1
#define AIN0_2 2
#define AIN0_3 3
#define HVDA 4
#define AGND 5
#define P3EVEN 6
#define P3ODD 7
#define TEMP_SENSOR 8
#define G05 6
#define G1 0
#define G2 1
#define G4 2
#define G8 3
#define G16 4

unsigned int le_adc0(unsigned char canal, unsigned char ganho) {
	ADC0CF = (ADC0CF & 0xf8) | ganho;
	AMX0SL = canal;

	delay_us(1); // Tempo para estabilizar (Rmux, Csample)

	AD0BUSY = 1; // Inicia convesão AD

	while(AD0BUSY); // Espera conversão terminar

	return(ADC0H<<8 | ADC0L);
}

void main (void)
{
    unsigned int ladc;

	Init_Device();

	SFRPAGE=LEGACY_PAGE;

    glcd_init();
	glcd_clear(ESQ);
	glcd_clear(DIR);
    
	while(1){
		
        read_adc = le_adc0(AIN0_1, G4);

		printf_fast_f("\x01 ADC0=%04u    ", read_adc);

		// 0.01483154 = (Vref/12)* (100/4) = (2.43/4096)* (100/4)
		printf_fast_f("\x02 T=%3.2f C", read_adc*0.01483154); // Exibe variação de temperatura
        
		delay_ms(500);  
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
