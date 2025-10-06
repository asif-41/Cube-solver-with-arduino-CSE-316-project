#include <LiquidCrystal_I2C.h>
#include <string.h>

#ifdef F_CPU
#undef F_CPU
#define F_CPU 1000000L
#endif

//  TERMINAL
const int terminal_baud = 9600;     //  TERMINAL BAUD RATE(FIXED)

//  LCD DISPLAY
const int sda = A4;
const int scl = A5;
LiquidCrystal_I2C lcd(0x20,16,2);     //  0x27 FOR TEXAS, 0x20 FOR PROTEUS


//  UART
const int rx = 0;
const int tx = 1;

volatile int isIdle = LOW;
const int master_idle = 13;         //  OUTPUT
const int ip_op_idle = 12;          //  INPUT
const int motor_idle = 11;          //  INPUT
const int receiveInterrupt = 3;     //  INTERRUPT FOR RECEIVE
const int sendTrigger = 4;          //  TRIGGERS INTERRUPT IN RECEIVER

volatile int dataCount = 0;
volatile char sender = -1;
volatile char receiver = -1;
volatile int dataLen = 0;
volatile char receivedData[250];


//  COMPONENT ADDRESS
#define Master 'M'
#define IpOp 'I'
#define Motors 'R'
#define Terminal 'T'


//  FUNCTIONS
int getBit(int reg, int loc);
void arduino_init();
void writeInLcd(char str[], int r, int c, bool linebreak);
void clearLcd();


volatile char str[100];
volatile int len = 0;

void setup() {

  arduino_init();

  writeInLcd("All okay in master", 0, 0, true);
  
  isIdle = HIGH;
  digitalWrite(master_idle, HIGH);
}

void loop() {

  if(Serial.available()){
    if(dataCount == 0){
      dataCount++;
      dataLen = Serial.read();
      dataLen++;

      //Serial.println(dataLen);
    }
    else if(dataCount == 1){
      dataCount++;
      sender = Serial.read();

//      Serial.println(sender);
    }
    else if(dataCount == 2){
      dataCount++;
      receiver = Serial.read();

//      Serial.println(receiver);
    }
    else{
      receivedData[dataCount-3] = Serial.read();
      dataCount++;

      if(dataCount == dataLen+2){
        receivedData[dataLen] = '\0';
        //for(int i=0; i<dataLen; i++) Serial.print(receivedData[i]);
        //Serial.println();
        
        dataCount = 0;
        isIdle = HIGH;
        digitalWrite(master_idle, HIGH);
        
        //Serial.println(dataLen);
        uart_send(receivedData);
      }
    }
  }

}


//=======================================================
//
//      UART COMMUNICATION
//
//=======================================================

void uart_send_char(int c){
  Serial.write(c);
  delay(100);
}

void uart_send(char str[]){

  while(isIdle == LOW || digitalRead(motor_idle) == LOW) {}

  isIdle = LOW;
  digitalWrite(master_idle, LOW);
  digitalWrite(sendTrigger, HIGH);

  int len = strlen(str);
  int from = Master;
  int to = Motors;

  uart_send_char(len);
  uart_send_char(from);
  uart_send_char(to);
  for(int i=0; i<len; i++) uart_send_char(str[i]);

  isIdle = HIGH;
  digitalWrite(master_idle, HIGH);
  digitalWrite(sendTrigger, LOW);
}

void uart_receive(){
  dataCount = 0;
  isIdle = LOW;
  digitalWrite(master_idle, LOW);
}

void uart_flush(){
  while(Serial.available() > 0){
    char c = Serial.read();
  }
}

//=======================================================
//
//=======================================================



//=======================================================
//
//    LCD DISPLAY
//
//=======================================================

//  ROW STARTS FROM 0
//  COL STARTS FROM 0

void clearLcd(){
  lcd.clear();
}

void writeInLcd(char str[], int r, int c, bool linebreak){
  lcd.setCursor(c,r);
  if(linebreak == false) lcd.print(str);
  else {
    int rem = 16 - c;
    int len = strlen(str);

    if(len <= rem || r > 0) lcd.print(str);
    else{
      char line1[rem+1];
      char line2[len-rem+1];
      
      for(int i=0; i<rem; i++) line1[i] = str[i];
      for(int i=rem, j=0; i<len; i++, j++) line2[j] = str[i];
      
      line1[rem] = '\0';
      line2[len-rem] = '\0';

      lcd.print(line1);
      lcd.setCursor(c, r+1);
      lcd.print(line2);      
    }
  }
}

//=======================================================
//
//=======================================================







//======================================================
//
//      ARDUINO INIT
//
//======================================================

int getBit(int reg, int loc){
  int temp = reg >> loc;
  return (temp & 1);
}

void arduino_init(){
  pinMode(master_idle, OUTPUT);
  pinMode(ip_op_idle, INPUT);
  pinMode(motor_idle, INPUT);
  pinMode(sendTrigger, OUTPUT);
  pinMode(receiveInterrupt, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(receiveInterrupt), uart_receive, RISING);

  digitalWrite(master_idle, LOW);
  digitalWrite(sendTrigger, LOW); 
  
  Serial.begin(terminal_baud);

  lcd.init();
  lcd.clear();         
  lcd.backlight();  
}

//======================================================
//
//======================================================
