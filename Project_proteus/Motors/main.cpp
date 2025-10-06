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
 #define initialDelay 1000			//	INITIAL DELAY

 #define motorDelay 1
 #define stepCount 56
 #define moveDelay 3000

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


//	UART 
#define rx 0					//	PORT D
#define tx 1					//	PORT D
#define master_idle 0			//  INPUT, PORT B
#define motor_idle 1			//  OUTPUT, PORT B
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
 void rotate(char c, bool prime);
 void followMoves(char str[]);
 void writeInLcd(char str[], int r, int c, bool linebreak);
 void clearLcd();
 
 //	ONLY RECEIVE
 void uart_init();
 int uart_send(char c, FILE *unused);
 int uart_receive();




int main(void)
{
	atmega_init();
	writeInLcd("All okay in motors", 1, 0, true);
	
	isIdle = 1;
	PORTB |= (1 << motor_idle);
	PORTD |= (1 << activePin);
	
	//followMoves("FRBLDF'R'B'L'D'");
	
	printf("Debugger\n\r");
	
	while (1)
	{
	}
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
	if(move == 'F'){
		if(prime == false) PORTA |= (1 << dirPinF);
		else PORTA &= ~(1 << dirPinF);
		
		for(int i=0; i<stepCount; i++){
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
		
		for(int i=0; i<stepCount; i++){
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
		
		for(int i=0; i<stepCount; i++){
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
		
		for(int i=0; i<stepCount; i++){
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
		
		for(int i=0; i<stepCount; i++){
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
//			INTERRUPT
//
//==============================================

ISR(INT0_vect){
	
	if(getBits(PORTD, activePin) == 0){
		GIFR |= (1 << INT2);
		return ;
	}
		
	isIdle = 0;
	PORTB &= ~(1 << motor_idle);
	
	dataLen = uart_receive();
	printf("Length: %d\n\r", dataLen);
	
	sender = uart_receive();
	printf("Sender: %c\n\r", sender);
	
	receiver = uart_receive();
	printf("Receiver: %c\n\r", receiver);
	
	for(int i=0; i<dataLen; i++) {
		receivedData[i] = uart_receive();
		printf("%c\n", receivedData[i]);
	}
	printf("%s\n\r", receivedData);
	
	isIdle = 1;
	PORTB |= (1 << motor_idle);
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

int uart_send(char c, FILE *unused){
	while(getBits(UCSRA, UDRE) == 0) {}
	UDR = c;
	return 0;
}

int uart_receive(){
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
	DDRA = 0xFF;
	DDRC = 0b00111111;
	DDRC |= (1 << stepPinD) | (1 << dirPinD);
	DDRB = 0;
	DDRB |= (1 << motor_idle);
	
	//	ENABLING PINS
	PORTD &= ~(1 << activePin);
	PORTA = 0;
	PORTC &= ~(1 << stepPinD);
	PORTC &= ~(1 << dirPinD); 
	isIdle = 0;
	PORTB &= ~(1 << motor_idle);
	
	//	INITIALIZE LCD
	Lcd4_Init();
	
	//	ENABLE UART
	uart_init();
	stdout = fdevopen(uart_send, NULL);
		
	//	ENABLE INTERRUPTS
	GICR |= (1 << INT0);
	MCUCR |= (1 << ISC01) | (1 << ISC00);
	
	sei();
	
	//	INITIAL DELAY
	_delay_ms(initialDelay);
}

//==============================================
//
//==============================================


