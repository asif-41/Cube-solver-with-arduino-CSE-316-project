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
#define initialDelay 1000			//	INITIAL DELAY

//	BUTTON INTERRUPTS: INT2

#define inpPin 0				//	PORTA
#define captPin 1				//	PORTA
#define retakePin 2				//	PORTA
#define scramblePin 3			//	PORTA
#define solvePin 4				//	PORTA
#define cancelPin 5				//	PORTA

//	BUTTON INDICATORS

#define inpPinActive 6				//	PORTC
#define capPinActive 3				//	PORTD
#define retakePinActive 4			//	PORTD
#define scramblePinActive 5			//	PORTD
#define solvePinActive 6			//	PORTD
#define cancelPinActive 7			//	PORTD



//	UART
#define rx 0						//	PORT D
#define tx 1						//	PORT D
#define master_idle 0				//  INPUT, PORT B
#define ipOp_idle 1					//  OUTPUT, PORT B
#define sendTrigger 3				//	PORT B, OUTPUT
									//	RECEIVE INTERRUPT INT0

volatile int isIdle = 0;
volatile char sender = -1;
volatile char receiver = -1;
volatile int dataLen = 0;
volatile char receivedData[250];


//  COMPONENT ADDRESS
#define Master 'M'
#define IpOp 'I'
#define Motors 'R'
#define Terminal 'T'



#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"



void atmega_init();
int getBits(int reg, int loc);
void writeInLcd(char str[], int r, int c, bool linebreak);
void clearLcd();

//	ONLY TRANSMIT
void uart_init();
void uart_send_char(char c);
void uart_send(char str[]);




int main(void)
{
	atmega_init();
	writeInLcd("All okay in i/p", 1, 0, true);
	
	isIdle = 1;
	PORTB |= (1 << ipOp_idle);
	PORTC |= (1 << activePin);
	
	while (1)
	{
	}
}







//==============================================
//
//			INTERRUPT
//
//==============================================

ISR(INT0_vect){
	
}

ISR(INT2_vect){
	
	if(getBits(PORTC, activePin) == 0){
		GIFR |= (1 << INT2);
		return ;
	}
	
	int x = PINA;
	
	if(getBits(x, inpPin) == 1){
		PORTC |= (1 << inpPinActive);
		uart_send("INPUT NEWA SHURU");
		_delay_ms(inpDelay);
		PORTC &= ~(1 << inpPinActive);
	}
	else if(getBits(x, captPin) == 1){
		uart_send("CAPTURE KORBE");
		PORTD |= (1 << capPinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << capPinActive);
	}
	else if(getBits(x, retakePin) == 1){
		uart_send("RETAKE KORBE");
		PORTD |= (1 << retakePinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << retakePinActive);
	}
	else if(getBits(x, scramblePin) == 1){
		uart_send("SCRAMBLE KORBE");
		PORTD |= (1 << scramblePinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << scramblePinActive);
	}
	else if(getBits(x, solvePin) == 1){
		uart_send("SOLVE KORBE");
		PORTD |= (1 << solvePinActive);
		_delay_ms(inpDelay);
		PORTD &= ~(1 << solvePinActive);
	}
	else if(getBits(x, cancelPin) == 1){
		uart_send("CANCEL KORBE");
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
//			USART COMMUNICATION
//
//==============================================

//	BAUD RATE 9600
//	STOP BIT 1
//	DATA BIT 8
//	PARITY NONE

void uart_init(){
	UBRRH = 0;
	UBRRL = 12;
	
	UCSRA = 0b00000010;
	UCSRB = 0b00011000;
	UCSRC = 0b10000110;
}

void uart_send_char(char c){
	while(getBits(UCSRA, UDRE) == 0) {}
	UDR = c;
}

void uart_send(char str[]){
	
	while( ! (isIdle == 1 && getBits(PINB, master_idle) == 1)) {}
	
	isIdle = 0;
	PORTB &= ~(1 << ipOp_idle);
	PORTB |= (1 << sendTrigger);
	
	int len = strlen(str);
	int from = IpOp;
	int to = Master;
	
	uart_send_char(len);
	uart_send_char(from);
	uart_send_char(to);
	for(int i=0; i<len; i++) uart_send_char(str[i]);
	
	isIdle = 1;
	PORTB |= (1 << ipOp_idle);
	PORTB &= ~(1 << sendTrigger);
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
	DDRC = 0b00111111;
	DDRC |= (1 << activePin) | (1 << inpPinActive);
	DDRD = 0b11111000;
	DDRB = 0;
	DDRB |= (1 << ipOp_idle) | (1 << sendTrigger);
	
	//	ENABLING PINS
	PORTC &= ~(1 << activePin);
	isIdle = 0;
	PORTB &= ~(1 << ipOp_idle);
	PORTB &= ~(1 << sendTrigger);
	
	//	INITIALIZE LCD
	Lcd4_Init();
	
	//	ENABLE UART
	uart_init();
	
	
	//ENABLE INTERRUPT 2
	GICR |= (1 << INT2) | (1 << INT0);
	MCUCSR |= (1 << ISC2);
	MCUCR |= (1 << ISC01) | (1 << ISC00);
	
	//	ENABLE INTERRUPTS
	sei();
	
	//	INITIAL DELAY
	_delay_ms(initialDelay);
}

//==============================================
//
//==============================================
