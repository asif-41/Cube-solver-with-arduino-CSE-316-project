/*
 * USART_Receiver.cpp
 *
 * Created: 7/16/2021 7:03:06 AM
 * Author : Kamal
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void uart_receive(){
	//RECEIVE DATA
	
	char d;
	d = UDR;
	
	PORTC |= (1 << 1);
	_delay_ms(1000);
	PORTC = 0;
}

void uart_send(){
	
}

void uart_init(){
	
	//SET BAUD RATE
	//SET CONTROL REGISTERS
	
	UBRRL = 0x33;
	UBRRH = 0x00;
	
	UCSRA = 0b00000000;
	UCSRB = 0b10010000;
	UCSRC = 0b10110110;	
}

ISR(USART_RXC_vect){
	PORTC |= (1 << 0);
	uart_receive();
}



int main(void)
{
	DDRC = 0x03;
	DDRD = 0b10000000;
	PORTC = 0;
	PORTD |= 1 << 7;
	
	uart_init();
	
	sei();
	
	/* Replace with your application code */
	while (1)
	{
	}
}

