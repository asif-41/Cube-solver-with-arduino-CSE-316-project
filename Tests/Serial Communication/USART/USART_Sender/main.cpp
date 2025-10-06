/*
 * USART_Sender.cpp
 *
 * Created: 7/16/2021 7:02:28 AM
 * Author : Kamal
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void uart_receive(){
	
}

void uart_send(){
	
	while( ! (((UCSRA & (1<< UDRE)) >> UDRE) & 1) ) {}
	UDR = 'a';
}

void uart_init(){
	
	//SET BAUD RATE
	//SET CONTROL REGISTERS
	
	UBRRL = 0x33;
	UBRRH = 0x00;
	
	UCSRA = 0b00000000;
	UCSRB = 0b01001000;
	UCSRC = 0b10110110;	
}

ISR(USART_TXC_vect){
	
	PORTC |= (1 << 0);
	_delay_ms(1000);
		
	PORTC = 0;
}




ISR(INT2_vect){
	PORTC |= (1 << 1);
	uart_send();
}



int main(void)
{
	DDRC = 0x03;
	DDRD = 0b10000000;
	PORTC = 0;
	PORTD |= 1 << 7;
	
	uart_init();
	
	GICR |= (1 << INT2);
	MCUCSR |= (1 << ISC2);
	
	sei();
	
	/* Replace with your application code */
	while (1)
	{
	}
}
