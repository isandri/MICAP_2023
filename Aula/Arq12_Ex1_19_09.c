#include "config.c"
#include "def_pinos.h"
#include "fonte.c"
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
#define ESQ 0
#define DIR 1


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


void e_gLCD(unsigned char byte, __bit cd, __bit cs){
	
	while(l_gLCD(CO, cs) & 0x80); // Espera se BUSY ? 1
	
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

void ini_LCDg(){
	// definindo condi??es iniciais
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

void conf_Y(unsigned char y, __bit cs){
	y &= 0x7f;
	y |= 0x40;
	
	e_gLCD(y, CO, cs);
}

void conf_pag(unsigned char x, __bit cs){
	
	x &= 0x07;
	x |= 0xb8;
	
	e_gLCD(x, CO, cs);
}

void limpa_gLCD(__bit cs){
	unsigned char i, j;

	for(i = 0; i<8; i++){
		conf_pag(i, cs);
		conf_Y(0, cs);
		for(j = 0; j<64; j++)
			e_gLCD(0x00, DA, cs);
	}
}

void putchar (char c)
{
    static unsigned char caracter_count=0;
    __bit lado;


    if(c == 1){
		caracter_count=0;
		conf_pag(0, ESQ);
        conf_pag(0, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 2){
		caracter_count=0;
		conf_pag(1, ESQ);
        conf_pag(1, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 3){
		caracter_count=0;
		conf_pag(2, ESQ);
        conf_pag(2, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 4){
		caracter_count=0;
		conf_pag(3, ESQ);
        conf_pag(3, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 5){
		caracter_count=0;
		conf_pag(4, ESQ);
        conf_pag(4, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 6){
		caracter_count=0;
		conf_pag(5, ESQ);
        conf_pag(5, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 7){
		caracter_count=0;
		conf_pag(6, ESQ);
        conf_pag(6, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	}else if(c == 8){
		caracter_count=0;
		conf_pag(7, ESQ);
        conf_pag(7, DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
	} else{

	    if (caracter_count<8) {
			lado = ESQ;
	    }else {
	        lado = DIR;
	    }

	    c = c-32;
	    e_gLCD(fonte[c][0], DA, lado);
	    e_gLCD(fonte[c][1], DA, lado);
	    e_gLCD(fonte[c][2], DA, lado);
	    e_gLCD(fonte[c][3], DA, lado);
	    e_gLCD(fonte[c][4], DA, lado);
	    e_gLCD(0x00, DA, lado);   
	    e_gLCD(0x00, DA, lado);
	    e_gLCD(0x00, DA, lado);
	    caracter_count = caracter_count + 1;
	}
}


void main (void) {
	unsigned int cont;
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	ini_LCDg();
	limpa_gLCD(ESQ);
	limpa_gLCD(DIR);
	
	while(1) {
		cont = (TH1 << 8) | TL1;
		printf_fast_f("\x01 cont=%02d", cont);
	}
}
