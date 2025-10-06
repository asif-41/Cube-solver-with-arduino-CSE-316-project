/*
 * Motors.cpp
 *
 * Created: 7/25/2021 11:33:10 AM
 * Author : Kamal
 */

 #define F_CPU 1000000L


 //	LCD DISPLAY
 #define D4 eS_PORTC2
 #define D5 eS_PORTC3
 #define D6 eS_PORTC4
 #define D7 eS_PORTC5
 #define RS eS_PORTC0
 #define EN eS_PORTC1

 #define activePin 7				//	PORT D

 #define motorDelay 1
 #define stepCount 54
 #define stepCountF 53
 #define stepCountR 53
 #define stepCountB 53
 #define stepCountL 53
 #define stepCountD 53
 #define stepCountPrime 56

 
 
 #define moveDelay 2000

 #define stepPinF 0				//	PORT A
 #define dirPinF 1				//	PORT A
 #define stepPinR 2				//	PORT A
 #define dirPinR 3				//	PORT A
 #define stepPinB 4				//	PORT A
 #define dirPinB 5				//	PORT A
 #define stepPinL 6				//	PORT A
 #define dirPinL 7				//	PORT A
 #define stepPinD 7				//	PORT C
 #define dirPinD 6				//	PORT C

 //	SPI
 //	LOW MEANS MASTER SENDING
 #define miso 6						//	PORTB
 #define spiDataTrigger 3			//	PORTB
 #define spiDataPassOkay 1			//	PORTB
 volatile int spiOnGoing = 0;
 volatile int spiDataLen = 0;
 volatile int spiDataSender = 0;
 volatile char spiData[250];

 //  COMPONENT ADDRESS
 #define Master 0
 #define IpOp 1
 #define Motors 2
 
 
 

 #include <avr/io.h>
 #include <stdio.h>
 #include <string.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "lcd.h"


 void atmega_init();
 int getBits(int reg, int loc);
 void uart_init();
 int uart_send(char c, FILE *unused);
 int uart_receive(FILE *unused);
 void rotate(char c, bool prime);
 void followMoves(char str[]);
 void writeInLcd(char str[], int r, int c, bool linebreak);
 void clearLcd();
 
 void spi_init();
 void spi_send(int to, char str[]);




int main(void)
{
	atmega_init();
	
	//char moves[10] = {'F', '\'', 'R', '\'', 'B', '\'',  'L', '\'', 'D', '\''};
	
	_delay_ms(2000);
	followMoves("FFFFF'F'F'F'RRRRR'R'R'R'BBBBB'B'B'B'LLLLL'L'L'L'DDDDD'D'D'D'");

	
	/*
	while (1)
	{
			//_delay_ms(moveDelay);
	}
	*/
}


//==============================================
//
//			MOTOR MOVEMENTS
//
//==============================================

void followMoves(char str[]){
	
	int len = strlen(str);
	for(int i=0; i<len; i++){
		
		char move = str[i];
		bool prime = false;
		
		if(i != len-1 && str[i+1] == '\''){
			prime = true;
			i++;
		}
		
		rotate(move, prime);
		_delay_ms(moveDelay);
	}
}

void rotate(char move, bool prime){
	
	int incStep = 0;
	
	if(prime) incStep = 3;
	
	if(move == 'F'){		
		
		
		if(prime == false) PORTA |= (1 << dirPinF);
		else
		{
			PORTA &= ~(1 << dirPinF);
		}
		
		for(int i=0; i<stepCountF + incStep; i++){
			PORTA |= (1 << stepPinF);
			_delay_ms(motorDelay);
			PORTA &= ~(1 << stepPinF);
			_delay_ms(motorDelay);
		}
		PORTA &= ~(1 << dirPinF);
	}
	else if(move == 'R'){
		if(prime == false) PORTA |= (1 << dirPinR);
		else PORTA &= ~(1 << dirPinR);
		
		for(int i=0; i<stepCountR + incStep; i++){
			PORTA |= (1 << stepPinR);
			_delay_ms(motorDelay);
			PORTA &= ~(1 << stepPinR);
			_delay_ms(motorDelay);
		}
		PORTA &= ~(1 << dirPinR);
	}
	else if(move == 'B'){
		if(prime == false) PORTA |= (1 << dirPinB);
		else PORTA &= ~(1 << dirPinB);
		
		for(int i=0; i<stepCountB + incStep; i++){
			PORTA |= (1 << stepPinB);
			_delay_ms(motorDelay);
			PORTA &= ~(1 << stepPinB);
			_delay_ms(motorDelay);
		}
		PORTA &= ~(1 << dirPinB);
	}
	else if(move == 'L'){
		if(prime == false) PORTA |= (1 << dirPinL);
		else PORTA &= ~(1 << dirPinL);
		
		for(int i=0; i<stepCountL + incStep; i++){
			PORTA |= (1 << stepPinL);
			_delay_ms(motorDelay);
			PORTA &= ~(1 << stepPinL);
			_delay_ms(motorDelay);
		}
		PORTA &= ~(1 << dirPinL);
	}
	else if(move == 'D'){
		if(prime == false) PORTC |= (1 << dirPinD);
		else PORTC &= ~(1 << dirPinD);
		
		for(int i=0; i<stepCountD + incStep; i++){
			PORTC |= (1 << stepPinD);
			_delay_ms(motorDelay);
			PORTC &= ~(1 << stepPinD);
			_delay_ms(motorDelay);
		}
		PORTC &= ~(1 << dirPinD);
	}
}

//==============================================
//
//==============================================




//==============================================
//
//		SPI COMMUNICATION (SLAVE)
//
//==============================================

//	DORD 0
//	CPOL 0
//	CPHA 1
//	frequency/16

void spi_init(){
	
	//	MISO OUTPUT, MOSI SCK INPUT(AUTO)
	DDRB |= (1 << miso);
	
	SPCR = 0b11000101;
	SPSR = 0b00000000;
}

ISR(SPI_STC_vect){
	if(spiOnGoing == 0){
		PORTB &= ~(1 << spiDataTrigger);
		spiDataLen = SPDR;
		
		printf("Length: %d\n\r", spiDataLen);
		spiOnGoing++;
	}
	else if(spiOnGoing == 1){
		PORTB &= ~(1 << spiDataTrigger);
		spiDataSender = SPDR;
		
		if(spiDataSender == Master) printf("Sender: Master\n\r");
		else if(spiDataSender == IpOp) printf("Sender: IpOp\n\r");
		else if(spiDataSender == Motors) printf("Sender: Motors\n\r");
		
		spiOnGoing++;
	}
	else{
		PORTB &= ~(1 << spiDataTrigger);
		spiData[spiOnGoing-2] = SPDR;
		spiOnGoing++;
		
		if(spiOnGoing == spiDataLen+2){
			spiData[spiOnGoing] = '\0';
			
			printf("Received: %s\n\r", spiData);
			spiOnGoing = 0;
		}
	}
	SPDR = SPSR;
}

void spi_send_char(char c){
	
	while(getBits(PINB, spiDataPassOkay) == 0) {}
	
	if(getBits(SPCR, SPIE) == 1){
		SPCR &= ~(1 << SPIE);
		cli();
	}
	
	SPDR = SPSR;				//	CLEARING SPIF
	SPDR = c;
	
	PORTB |= (1 << spiDataTrigger);
	_delay_ms(100);
	
	while(getBits(SPSR, SPIF) == 0) {}
	
	PORTB &= ~(1 << spiDataTrigger);
}

void spi_send(int to, char str[]){
	
	while(spiOnGoing != 0) {}
	
	int len = strlen(str);
	
	spi_send_char(len);
	spi_send_char(Motors);
	for(int i=0; i<len; i++) spi_send_char(str[i]);
	
	SPDR = SPSR;				//	CLEARING SPIF
	SPCR |= (1 << SPIE);
	sei();
	
	//	SAY THAT SEI() IS DONE
	PORTB |= (1 << spiDataTrigger);
	_delay_ms(100);
	PORTB &= ~(1 << spiDataTrigger);
}

//==============================================
//
//==============================================









//==============================================
//
//			USART COMMUNICATION
//
//==============================================

//	BAUD RATE 1200
//	STOP BIT 1
//	DATA BIT 8
//	ODD PARITY

void uart_init(){
	UBRRH = 0;
	UBRRL = 0x33;
	
	UCSRA = 0b00000000;
	UCSRB = 0b00011000;
	UCSRC = 0b10110110;
}

int uart_send(char c, FILE *unused){
	while(getBits(UCSRA, UDRE) == 0) {}
	UDR = c;
	return 0;
}

int uart_receive(FILE *unused){
	while(getBits(UCSRA, RXC) == 0) {}
	return UDR;
}

//==============================================
//
//==============================================



//==============================================
//
//			LCD PRINT
//
//==============================================

//	STARTS FROM ROW: 1, COL: 0

void clearLcd(){
	Lcd4_Clear();
}

void writeInLcd(char str[], int r, int c, bool linebreak){
	
	Lcd4_Set_Cursor(r, c);
	if(linebreak == false) Lcd4_Write_String(str);
	else{
		int rem = 16 - c;
		int len = strlen(str);
		
		if(len <= rem || r > 1) Lcd4_Write_String(str);
		else {
			char line1[rem+1];
			char line2[len-rem+1];
			
			for(int i=0; i<rem; i++) line1[i] = str[i];
			for(int i=rem, j=0; i<len; i++, j++) line2[j] = str[i];
			
			line1[rem] = '\0';
			line2[len-rem] = '\0';
			
			Lcd4_Write_String(line1);
			Lcd4_Set_Cursor(r+1, c);
			Lcd4_Write_String(line2);
		}
	}
}

//==============================================
//
//==============================================



//==============================================
//
//			ATMEGA INIT
//
//==============================================

int getBits(int reg, int loc){
	int temp = reg >> loc;
	return (temp & 1);
}

void atmega_init(){
	
	//	ENABLE JTAGE
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);
	
	//	DDR OF PINS
	DDRD |= (1 << activePin);
	DDRB = 0x00;
	DDRB |= (1 << spiDataTrigger);
	DDRA = 0xFF;
	DDRC = 0b00111111;
	DDRC |= (1 << stepPinD) | (1 << dirPinD);
	
	PORTA = 0;
	PORTC &= ~(1 << stepPinD);
	
	//	INITIALIZE LCD
	Lcd4_Init();
	
	//	ENABLING PINS
	PORTD |= (1 << activePin);
	PORTB &= ~(1 << spiDataTrigger);
	
	//	ENABLE UART
	uart_init();
	stdout = fdevopen(uart_send, NULL);
	stdin = fdevopen(NULL, uart_receive);
	
	//	ENABLE SPI
	spi_init();
	
	//	ENABLE INTERRUPTS
	sei();
}

//==============================================
//
//==============================================


