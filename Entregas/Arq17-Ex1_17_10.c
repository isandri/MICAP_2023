// MICAP T01 2023/2
// Arq17-Ex1 (17/10/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include "config.c"
#include "def_pinos.h"
#include "fonte.c"

#include <stdio.h>
#include <string.h>


// Assinatura das funções auxiliares
void delay_ms(unsigned int);
void delay_us(unsigned int);
void putchar(char);


// Constantes auxiliares
#define NOP4()  NOP();NOP();NOP();NOP()
#define NOP8()  NOP4();NOP4()
#define NOP12() NOP4();NOP8()


//-----------------------------------------------------------------------------
// Código específico do  Arq17-Ex1

volatile __bit  byte_ack;
volatile unsigned char byte_rec;

void putchar(char c)
{
	SBUF0 = c;
	while(TI0 == 0);
	TI0 = 0;
}

void isr_uart() __interrupt 4
{
	if(RI0 == 1)
	{
		byte_ack = 1;
		byte_rec = SBUF0;
		RI0=0;
	}
}

void le_p1() {
	printf_fast_f("Pressione qualquer tecla para parar...\n");

	do {
		printf_fast_f("P1=%u\n", P1);
		delay_ms(500);
	} while(!byte_ack);
	byte_ack=0;
	printf_fast_f("\n");
}

void main () {
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	while (1)
	{
		while(!byte_ack);
		byte_ack=0;

  		if(byte_rec == 'b' || byte_rec == 'B') {
	        P0_7 = !P0_7;
			printf_fast_f("P0_7 foi invertida.\n");
	    } else if(byte_rec == 'a'|| byte_rec == 'A') {
	        le_p1();
	    } else if(byte_rec == 'm'|| byte_rec == 'M') {
	        printf_fast_f("MENU\n");
	        printf_fast_f("a - ler contagem em P1\n");
	        printf_fast_f("b - inverter P0_7\n");
	    } else {
			printf_fast_f("Comando desconhecido.\n");
		}
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

