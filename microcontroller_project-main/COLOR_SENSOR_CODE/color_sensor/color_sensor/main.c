/*
 * color_sensor.c
 *
 * Created: 7/18/2021 9:41:43 AM
 * Author : Shakil
 */ 


#ifndef F_CPU
#define F_CPU 1000000UL
#endif
#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7


#include <avr/io.h>
#include <util/delay.h>
#include<stdio.h>
#include<string.h>
#include "lcd.h"



volatile unsigned char result;

#define TCS_OUT_POS 4
#define S0 0
#define S1 1
#define S2 2
#define S3 3


uint32_t readInput()
{
	//If the function is entered when the level on OUT line was low
	//Then wait for it to become high.
	
	//ADCSRA |= (1<<ADSC);
	
	if(!(PORTC & (1<<TCS_OUT_POS)))
	{
		while(!(PORTC & (1<<TCS_OUT_POS)));   //Wait for rising edge
	}


	while(PORTC & (1<<TCS_OUT_POS));   //Wait for falling edge

	TCNT1=0x0000;//Reset Counter

	TCCR1B=(1<<CS10); //Prescaller = F_CPU/1 (Start Counting)

	while(!(PORTC & (1<<TCS_OUT_POS)));   //Wait for rising edge

	//Stop Timer
	TCCR1B=0x00;

	return ((float)8000000UL/TCNT1);

}


uint32_t MeasureR()
{
	uint8_t selector = 0x00;
	selector |= (0<<S2);
	selector |= (0<<S3);
	
	PORTC |= selector;
	
	//TCSSelectRed();
	
	uint32_t r;

	_delay_ms(10);
	r = readInput();

	_delay_ms(10);
	r += readInput();

	_delay_ms(10);
	r += readInput();

	return r/3.3;

}

uint32_t MeasureG()
{
	
	uint8_t selector = 0x00;
	selector |= (1<<S2);
	selector |= (1<<S3);
	
	PORTC |= selector;
	
	
	//TCSSelectGreen();
	uint32_t r;

	_delay_ms(10);
	r = readInput();

	_delay_ms(10);
	r += readInput();

	_delay_ms(10);
	r += readInput();

	return r/3;

}

uint32_t MeasureB()
{
	uint8_t selector = 0x00;
	selector |= (0<<S2);
	selector |= (1<<S3);
	
	PORTC |= selector;
	
	
	//TCSSelectBlue();
	uint32_t r;

	_delay_ms(10);
	r = readInput();

	_delay_ms(10);
	r += readInput();

	_delay_ms(10);
	r += readInput();

	return r/4.2;

}

uint32_t MeasureC()
{
	
	uint8_t selector = 0x00;
	selector |= (1<<S2);
	selector |= (0<<S3);
	
	PORTC |= selector;
	
	
	//TCSSelectClear();
	
	
	uint32_t r;

	_delay_ms(10);
	r = readInput();

	_delay_ms(10);
	r += readInput();

	_delay_ms(10);
	r += readInput();

	return r/3;
}


int main(void)
{
	MCUCSR |= 1<<JTD;
	MCUCSR |= 1<<JTD;
	
	DDRC = 0b00001111;
	
	TCNT0 = 0x00;
	TCCR0 = 0x07;
	
	//ADMUX = 0b01000000;                                    
	
	//ADCSRA |= 0b10000000;
	
	//ADCSRA |= (1<<ADSC);
	
	Lcd4_Init();

	
    while (1) 
    {
		
		Lcd4_Set_Cursor(1,1);
		Lcd4_Write_String("R: ");
		Lcd4_Set_Cursor(1,4);
		
		
		uint32_t r,g,b;
		
		r=MeasureR();
		g=MeasureG();
		b=MeasureB();
				
		
		/*
		while(ADCSRA & (1 << ADSC)){};
		
		unsigned int lower_half = ADCL;
		unsigned int upper_half = ADCH;
		
		
		float result = ((upper_half << 8) | lower_half)*4.5/1024;
		*/
		
		char result_char[50];
		
		
		sprintf (result_char, "%.2f", r);
		
		Lcd4_Write_String(result_char);
		
		Lcd4_Set_Cursor(1,7);
		
		sprintf (result_char, "%.2f", g);

		Lcd4_Write_String(result_char);
		
		Lcd4_Set_Cursor(1,10);
		
		sprintf (result_char, "%.2f", b);

		Lcd4_Write_String(result_char);		
		
		
    }
}

