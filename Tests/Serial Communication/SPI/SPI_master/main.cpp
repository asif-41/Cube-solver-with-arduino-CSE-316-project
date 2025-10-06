/*
 * SPI_master.cpp
 *
 * Created: 7/17/2021 7:39:07 AM
 * Author : Kamal
 */ 

#define isWorking 7										// PORT D
#define slave1 0										// PORT A
#define slave1Address 'A'								// SLAVE ADDRESS
#define sendIndicator 6									// PORT D

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

void spi_master_init(){

	DDRB |= (1 << DDB5) | (1 << DDB7);		//	MOSI, SCK OUTPUT
	//DDRB &= ~(1 << DDB6);					//	MISO INPUT, AUTO 	
	DDRA |= (1 << slave1);					//	SELECT PIN FOR SLAVE

	SPCR = 0b01010101;
	SPSR = 0b00000000;
}

void spi_master_communicate(){

	//	HIGH ALL SS PINS
	PORTA |= (1 << slave1);

	//	LOW TARGET SS PIN
	PORTA &= ~(1 << slave1);

	//	SET SPIF LOW
	SPSR &= ~(1 << SPIF);

	//	WRITE DATA
	unsigned char c = 0xF2;
	printf("From master: %c\n\r", c);

	//	SEND AND WAIT
	SPDR = c;
	while(getBit(SPSR, SPIF) == 0) {}

	//	READ DATA
	c = SPDR;
	printf("Received in master: %c\n\r", c);

	//	INDICATE SENDIND COMPLETION
	PORTD &= ~(1 << sendIndicator);
}




//================================================
//
//			ATMEGA32 INTERRUPTS
//
//================================================

ISR(INT2_vect){
	PORTD |= (1 << sendIndicator);
	spi_master_communicate();
}




//================================================
//
//			ATMEGA32 INITIALIZATION
//
//================================================

void atmega_init(){
	
	DDRD |= (1 << isWorking) | (1 << sendIndicator);

	// JTAG DISABLED
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);

	PORTD |= (1 << isWorking);

	GICR |= (1 << INT2);
	MCUCSR |= (1 << ISC2);	

	usart_init();
	spi_master_init();
	sei();
	
	stdout = fdevopen(usart_send, NULL);
	stdin = fdevopen(NULL, usart_receive);
	
	printf("Master terminal:\n\n\r");
	printf("Thikase ki? %s\n\r", hehe());
}





int main(void)
{
	atmega_init();		
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

