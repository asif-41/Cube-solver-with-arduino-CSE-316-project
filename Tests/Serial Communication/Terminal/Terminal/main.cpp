/*
 * Terminal.cpp
 *
 * Created: 7/16/2021 3:05:45 PM
 * Author : Kamal
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

volatile int cnt;

int regBit(int reg, int loc){
	int temp = reg & (1 << loc);
	temp = temp >> loc;
	return (temp & 1);	
}

void usart_init(){
	UBRRH = 0;
	UBRRL = 0x33;
	
	UCSRA = 0b00000000;
	UCSRB = 0b00011000;
	UCSRC = 0b10110110;	
}

int usart_send(char c, FILE *unused){
	while(regBit(UCSRA, UDRE) == 0) {}
	UDR = c;
	return 0;
}

int usart_receive(FILE *unused){
	while(regBit(UCSRA, RXC) == 0) {}
	return UDR;
}



ISR(INT2_vect){
	cnt++;	
}

int main(void)
{
	DDRC = 0xFF;
	PORTC = 0xFF;
	DDRD = 0b11000000;
	PORTD |= (1 << 7);
	
	GICR |= (1 << INT2);
	MCUCSR |= (1 << ISC2);
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);
	
	sei();
	
	usart_init();
	stdout = fdevopen(usart_send, NULL);
	stdin = fdevopen(NULL, usart_receive);
	
	cnt = 100;
	printf("Counting: \n\r");
	
    /* Replace with your application code */
    while (1) 
    {
		printf("%d\n\r", cnt--);
		if(cnt < 0) cnt = 100;
		_delay_ms(100);		
    }
}

