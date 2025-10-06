/*
 * Input_output.cpp
 *
 * Created: 7/22/2021 5:13:50 PM
 * Author : Asus
 */ 

#define F_CPU 1000000L


//	LCD DISPLAY
#define D4 eS_PORTC2
#define D5 eS_PORTC3
#define D6 eS_PORTC4
#define D7 eS_PORTC5
#define RS eS_PORTC0
#define EN eS_PORTC1


//	ACTIVE PIN ON PORT C
//	BUTTON INPUTS ON PORT A
//	BUTTON INDICATORS ON PORT D
//	BUTTON INTERRUPT INT2

#define inpDelay 500
#define activePin 7				//	PORT C

//	BUTTON INTERRUPTS: INT2

#define inpPin 0				//	PORTA
#define captPin 1				//	PORTA
#define retakePin 2				//	PORTA
#define scramblePin 3			//	PORTA
#define solvePin 4				//	PORTA
#define cancelPin 5				//	PORTA

//	BUTTON INDICATORS

#define inpPinActive 2				//	PORTD
#define capPinActive 3				//	PORTD
#define retakePinActive 4			//	PORTD
#define scramblePinActive 5			//	PORTD
#define solvePinActive 6			//	PORTD
#define cancelPinActive 7			//	PORTD

//	SPI
//	LOW MEANS MASTER SENDING
#define miso 6						//	PORTB
#define spiDataTrigger 3			//	PORTB
#define spiDataPassOkay 1			//	PORTB
volatile int spiOnGoing = 0;
volatile int spiDataLen = 0;
volatile int spiDataSender = 0;
volatile char spiData[100];

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
void writeInLcd(char str[], int r, int c, bool linebreak);
void clearLcd();

void spi_init();
void spi_send(int to, char str[]);




int main(void)
{
	atmega_init();
	printf("all okay in i/p\n\r");
	
	writeInLcd("All okay in i/p", 1, 0, true);
	
	while (1)
	{
	}
}







//==============================================
//
//			INTERRUPT
//
//==============================================

ISR(INT2_vect){
	
	int x = PINA;
	
	if(getBits(x, inpPin) == 1){
		printf("INPUT NEWA SHURU\n\r");
		PORTD |= (1 << inpPinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << inpPinActive);
	}
	else if(getBits(x, captPin) == 1){
		printf("CAPTURE KORBE\n\r");
		PORTD |= (1 << capPinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << capPinActive);
	}
	else if(getBits(x, retakePin) == 1){
		printf("RETAKE KORBE\n\r");
		PORTD |= (1 << retakePinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << retakePinActive);
	}
	else if(getBits(x, scramblePin) == 1){
		printf("SCRAMBLE KORBE\n\r");
		PORTD |= (1 << scramblePinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << scramblePinActive);
	}
	else if(getBits(x, solvePin) == 1){
		printf("SOLVE KORBE\n\r");
		PORTD |= (1 << solvePinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << solvePinActive);
	}
	else if(getBits(x, cancelPin) == 1){
		printf("CANCEL KORBE\n\r");
		PORTD |= (1 << cancelPinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << cancelPinActive);
	}
	GIFR |= (1 << INT2);
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
	spi_send_char(IpOp);
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
	DDRA = 0x00;
	DDRB = 0x00;
	DDRB |= (1 << spiDataTrigger);
	DDRC = 0b00111111;
	DDRC |= (1 << activePin);
	DDRD = 0xFC;
	
	//	ENABLING PINS
	PORTC |= (1 << activePin);
	PORTB &= ~(1 << spiDataTrigger);
	
	//	INITIALIZE LCD
	Lcd4_Init();
	
	//	ENABLE UART
	uart_init();
	stdout = fdevopen(uart_send, NULL);
	stdin = fdevopen(NULL, uart_receive);
	
	//	ENABLE SPI
	spi_init();
	
	//ENABLE INTERRUPT 2
	GICR |= (1 << INT2);
	MCUCSR |= (1 << ISC2);
	
	//	ENABLE INTERRUPTS
	sei();
}

//==============================================
//
//==============================================
