/*
 * SPI_slave.cpp
 *
 * Created: 7/17/2021 7:39:29 AM
 * Author : Kamal
 */

#define isWorking 7

#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>




//================================================
//
//			BIT OPERATIONS
//
//================================================

int getBit(int reg, int loc){
	int temp = reg & (1 << loc);
	temp = temp >> loc;
	return (1 & temp);
}

//	setBit -> reg |= (1 << loc);
//	resetBit -> reg &= ~(1 << loc);





//================================================
//
//			TERMINAL SETUP
//
//================================================

void usart_init(){
	UBRRH = 0;
	UBRRL = 0x33;
	
	UCSRA = 0b00000000;
	UCSRB = 0b00011000;
	UCSRC = 0b10110110;
}

int usart_send(char c, FILE *unused){
	while(getBit(UCSRA, UDRE) == 0) {}
	UDR = c;
	return 0;
}

int usart_receive(FILE *unused){
	while(getBit(UCSRA, RXC) == 0) {}
	return UDR;
}





//================================================
//
//			SPI COMMUNICATION
//
//================================================

void spi_slave_init(){

	DDRB |= (1 << DDB6);										//	MISO OUTPUT
	//DDRB &= ~(1 << DDB5) & ~(1 << DDB7) & ~(1 << DDB4);		//	MOSI, SCK, SS AS INPUT, AUTO

	SPCR = 0b01000101;
	SPSR = 0b00000000;
}

void spi_slave_communicate(){

	//	SPIF = 0
	SPSR &= ~(1 << SPIF);

	//	WRITE DATA
	unsigned char c = 0x33;
	printf("From slave: %c\n\r", c);

	SPDR = c;

	//	WAIT
	while(getBit(SPSR, SPIF) == 0) {}

	c = SPDR;
	printf("Received in slave: %c\n\r", c);
}






//================================================
//
//			ATMEGA32 INITIALIZATION
//
//================================================

void atmega_init(){
	
	DDRD |= (1 << isWorking);
	PORTD |= (1 << isWorking);

	// JTAG DISABLED
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);
	
	usart_init();
	spi_slave_init();
	
	sei();
	
	stdout = fdevopen(usart_send, NULL);
	stdin = fdevopen(NULL, usart_receive);
	
	printf("Slave terminal:\n\n\r");
	spi_slave_communicate();
}

int main(void)
{
	atmega_init();
	
	/* Replace with your application code */
	while (1)
	{
	}
}

 

