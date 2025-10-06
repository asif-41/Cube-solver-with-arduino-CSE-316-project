/*
 * GccApplication1.cpp
 *
 * Created: 7/9/2021 9:29:11 AM
 * Author : Kamal
 */ 

#define F_CPU 1000000

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <avr/interrupt.h>
#include <string.h>

int min(int a, int b){
	if(a < b) return a;
	else return b;
}

void printNumber(int v, int row, int col){

	char line[20];

	if(v == 0) line[0] = '0';
	else{
		
		int loc = 0;
		int isNeg = 0;
		
		if(v < 0){
			isNeg = 1;
			v = 0 - v;
		}
		while(v != 0){
			int k = v % 10;
			line[loc++] = k + 48;
			v /= 10;
		}
		if(isNeg == 1) line[loc++] = '-';
		
		strrev(line);
		
	}
	
	Lcd4_Set_Cursor(row, col);
	Lcd4_Write_String(line);
}

void printDouble(double v, int fracLimit, int row, int col){

	char line[20];
	int isNeg = 0;
	int loc = 0;
	
	int mul = 1;
	int k = min(fracLimit, 4);
	for(int i=0; i<k; i++) mul *= 10;
	
	int dec = (int) v;
	int frac = (v - dec) * mul;
	
	if(dec < 0 || frac < 0){
		isNeg = 1;
		dec = 0 - dec;
		frac = 0 - frac;
	}
	
	int d;
	int fracAse = (frac != 0);
	int decAse = (dec != 0);
	
	while(frac != 0){
		d = frac % 10;
		line[loc++] = d + 48;
		frac /= 10;
	}
	if(fracAse != 1) line[loc++] = '0';
	line[loc++] = '.';
	
	while(dec != 0){
		d = dec % 10;
		line[loc++] = d + 48;
		dec /= 10;
	}
	if(decAse != 1) line[loc++] = '0';
	
	if(isNeg == 1) line[loc++] = '-';
	strrev(line);
	
	Lcd4_Set_Cursor(row, col);
	Lcd4_Write_String(line);
}

void writeInLcd(char *str, int row, int col){
	
	Lcd4_Set_Cursor(row, col);
	Lcd4_Write_String(str);
}



ISR(ADC_vect){
	
	int x = ADCL;
	int y = ADCH;
	
	int v = ((y & 0x03) << 8) | x;
	double value = ((double) v * 210) / 1024 - 55;
	
	Lcd4_Clear();	
	writeInLcd("Value:", 1, 0);
	printDouble(value, 3, 2, 0);
	_delay_ms(1000);
}


int main(void)
{

	DDRC = 0b11000000;
	DDRD = 0b11110000;
	
	Lcd4_Init();
	Lcd4_Clear();	
	
	ADMUX = 0b00000000;
	ADCSRA = 0b10001000;
	
	sei();
	
	
	
	while(1)
	{
		ADCSRA |= (1 << ADSC);
	}
}

