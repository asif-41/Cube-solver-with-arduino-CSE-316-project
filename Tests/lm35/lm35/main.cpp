/*
 * lm35.cpp
 *
 * Created: 6/24/2021 7:49:20 AM
 * Author : Kamal
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>

volatile int temp = 20;


ISR(ADC_vect){
	
	int result = ADC;
	float d = (1.5*1000*result) / ((1<<10) * 10);
	temp = (int) d;
	
	PORTC = result;
}

ISR(INT0_vect){
	ADCSRA |= (1 << ADSC);
}

int main(void)
{
	DDRB = 0b00000001;
	DDRC = 0xFF;
	
	ADMUX =  0b00000000;
	ADCSRA = 0b10001000;
	
	GICR |= (1 << INT0);
	MCUCR |= (1 << ISC00) | (1 << ISC01);
	
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
	    //ADCSRA |= (1 << ADSC);
		if(temp > 20) PORTB = 0x01;
		else PORTB = 0x00;
    }
}

