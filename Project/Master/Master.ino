#include <LiquidCrystal.h>
#include <string.h>
#include <SPI.h>

#ifdef F_CPU
#undef F_CPU
#define F_CPU 1000000L
#endif

//  TERMINAL
const int terminal_baud = 9600;     //  TERMINAL BAUD RATE(FIXED)

//  LCD DISPLAY
const int rs = A0;                   //  PORT A
const int en = A1;                   //  PORT A
const int d4 = A2;                   //  PORT A
const int d5 = A3;                   //  PORT A
const int d6 = A4;                   //  PORT A
const int d7 = A5;                   //  PORT A
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//  SPI COMMUNICATION
const int ip_op_ss = 7;               //  PORT D
const int ip_op_trigger = 3;          //  PORT D, LOW MEANS MASTER SENDING
const int ip_op_idle = 6;             //  PORT D, DATA PASS OKAY WHEN HIGH

const int motor_ss = 5;               //  PORT D
const int motor_trigger = 2;          //  PORT D, LOW MEANS MASTER SENDING
const int motor_idle = 4;             //  PORT D, DATA PASS OKAY WHEN HIGH

const int miso = 12;
const int mosi = 11;
const int sck = 13;

//  SPI RECEIVE DATA
volatile int spiOnGoing = 0;          //  0 MEANS IDLE
volatile int spiDataLen = 0;
volatile int spiDataSender = 0;
volatile char spiData[100];


//  COMPONENT ADDRESS
#define Master 0
#define IpOp 1
#define Motors 2


//  FUNCTIONS
int getBit(int reg, int loc);
void arduino_init();
void writeInLcd(char str[], int r, int c, bool linebreak);
void clearLcd();
void spi_init();
void spi_send(int to, char str[]);
void spi_receive();





void setup() {

  arduino_init();

  writeInLcd("All okay in master", 0, 0, true);
  Serial.println("all okay in master");
}

void loop() {


}






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
//      SPI
//
//======================================================

//  DORD 0
//  CPOL 0
//  CPHA 1
//  frequency/16

void spi_init(){
  
  //  MISO INPUT, MOSI SCK OUTPUT
  digitalWrite(miso, INPUT);
  digitalWrite(mosi, OUTPUT);
  digitalWrite(sck, OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE1);
  SPI.setBitOrder(MSBFIRST);
}

void spi_send_char(char c){
  SPI.transfer(c);
  delay(250);
}

void spi_send(int to, char str[]){

  if(spiOnGoing != 0){
    while(true){
      while(spiOnGoing != 0) {}
      delay(250);
      if(spiOnGoing == 0) break;
    }
  }

  int ss;
  int len = strlen(str);

  if(to == IpOp) ss = ip_op_ss;
  else if(to == Motors) ss = motor_ss;

  spiOnGoing = 1;
  digitalWrite(ss, LOW);
  digitalWrite(ip_op_idle, LOW);
  digitalWrite(motor_idle, LOW);
  
  spi_send_char(len);
  spi_send_char(Master);
  for(int i=0; i<len; i++) spi_send_char(str[i]);

  digitalWrite(ss, HIGH);
  digitalWrite(ip_op_idle, HIGH);
  digitalWrite(motor_idle, HIGH);
  spiOnGoing = 0;
}

void spi_receive_trigger1(){
  spi_receive(IpOp);
}

void spi_receive_trigger2(){
  spi_receive(Motors);
}

void spi_receive(int component){

  int ss, idle;
  if(component == 1) ss = ip_op_ss, idle = motor_idle;
  else if(component == 2) ss = motor_ss, idle = ip_op_idle;

  if(digitalRead(ss) == HIGH) digitalWrite(ss, LOW);

  if(spiOnGoing == 0){
    spiOnGoing++;
    digitalWrite(idle, LOW);
    
    int d = SPI.transfer(' ');
    delay(150);
    spiDataLen = d;

    Serial.print("Length: ");
    Serial.println(spiDataLen);
  }
  else if(spiOnGoing == 1){
    spiOnGoing++;
    int d = SPI.transfer(' ');
    delay(150);
    spiDataSender = d;

    if(spiDataSender == Master) Serial.println("Sender: Master");
    else if(spiDataSender == IpOp) Serial.println("Sender: IpOp");
    else if(spiDataSender == Motors) Serial.println("Sender: Motors");
  }
  else if(spiOnGoing < spiDataLen+2){
    spiData[spiOnGoing-2] = SPI.transfer(' ');
    delay(150);
    spiOnGoing++;
  
    if(spiOnGoing == spiDataLen+2){
      spiData[spiOnGoing] = '\0';
      
      Serial.print("Received: ");
      for(int i=0; i<spiDataLen; i++) Serial.print(spiData[i]);
      Serial.println();
    }
  }
  else{
    digitalWrite(ss, HIGH);
    spiOnGoing = 0;
    digitalWrite(idle, HIGH);
  }
}

//======================================================
//
//======================================================





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
  Serial.begin(terminal_baud);
  lcd.begin(16,2);

  pinMode(ip_op_ss, OUTPUT);
  pinMode(ip_op_idle, OUTPUT);
  pinMode(ip_op_trigger, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ip_op_trigger), spi_receive_trigger1, RISING);
  digitalWrite(ip_op_trigger, LOW);
  
  pinMode(motor_ss, OUTPUT);
  pinMode(motor_idle, OUTPUT);
  pinMode(motor_trigger, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motor_trigger), spi_receive_trigger2, RISING);
  digitalWrite(motor_trigger, LOW);

  digitalWrite(ip_op_ss, HIGH);
  digitalWrite(motor_ss, HIGH);

  digitalWrite(ip_op_idle, HIGH);
  digitalWrite(motor_idle, HIGH);

  spi_init();
}

//======================================================
//
//======================================================
