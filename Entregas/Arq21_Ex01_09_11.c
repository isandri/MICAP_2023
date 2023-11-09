// MICAP T01 2023/2
// Arq21-Ex1 (09/11/2023)
// Escrito por Isadora S. Silva e Mateus B. Cassiano

#include <stdio.h>
#include "config.c"
#include "def_pinos.h"

// Variaveis globais
volatile __bit  byte_recebido;
volatile unsigned char rec_byte;

void putchar(char c)
{
	SBUF0 = c;
	while(TI0 == 0);
	TI0 = 0;
	
}

void ISR_UART0() __interrupt 4
{
	if(RI0 == 1)
	{
		byte_recebido = 1;
		rec_byte = SBUF0;
		RI0=0;
	}
	
}

void main (void){
	Init_Device();
	SFRPAGE=LEGACY_PAGE;

	while (1)
	{
		while(byte_recebido == 0);
		byte_recebido=0;
 			if(rec_byte == 'i'){
				P0_7 = !P0_7;
	        }else if(rec_byte == 'd'){
				if (PCA0CPH0<255){
						PCA0CPH0++;}
	        }else if(rec_byte == 'a'){
				if (PCA0CPH0>0){
						PCA0CPH0--;}
			}else if(rec_byte == 'm'){
				printf_fast_f("\n\n\n");
				printf_fast_f("MENU: \n");
				printf_fast_f("i-Inverte P0_7\n");
				printf_fast_f("a-Aumenta RPM\n");
				printf_fast_f("d-Diminui RPM\n");
	        }
	
	}

}
