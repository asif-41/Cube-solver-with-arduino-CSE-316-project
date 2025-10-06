/*
 * Sender.cpp
 *
 * Created: 7/16/2021 12:15:08 PM
 * Author : Kamal
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


int regBit(int reg, int loc){
	int temp = reg & (1 << loc);
	temp = temp >> loc;
	return (temp & 1);	
}


void usart_init(){
	
	UCSRA = 0b00000010;
	UCSRB = 0b00011000;
	UCSRC = 0b10000110;
	
	UBRRH = 0;
	UBRRL = 12;
}

int usart_send(char data, FILE *unused){
	while(regBit(UCSRA, UDRE) == 0) {}
	UDR = data;	
	return 0;
}

int usart_receive(FILE *unused1){
	while(regBit(UCSRA, RXC) == 0) {}
	return UDR;
}


int main()
{
	
	usart_init();
	stdout = fdevopen(usart_send, NULL);
	stdin = fdevopen(NULL, usart_receive);
	
	printf("Enter password: \n\r");
	
	int a;
	scanf("%d", &a);
	printf("Countdown:\n\r");
	
	int count = 10;
	
    /* Replace with your application code */
    while (1) 
    {
		printf("%d\n\r", count--);
		_delay_ms(1000);
    }
}

