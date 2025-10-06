#include <LiquidCrystal_I2C.h>

//#include <jm_LiquidCrystal_I2C.h>
//#include <jm_Scheduler.h>
//#include <jm_Wire.h>

#include <LiquidCrystal.h>


//#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display



//
//#define S0 4
//#define S1 5
//#define S2 6
//#define S3 7
//#define sensorOut 8


const int takeInput = 2;     // the number of the pushbutton pin
const int confirmInput = 3;
const int retakeInput = 4;
const int ledPin =  5;      // the number of the LED pin

// variables will change:
int takeInputState = 0;         // variable for reading the pushbutton status
int confirmInputState = 0;
int retakeInputState = 0;

#define S0 13
#define S1 12
#define S2 11
#define S3 10
#define sensorOut 9


// Calibration Values
// *Get these from Calibration Sketch
int redMin = 0; // Red minimum value
int redMax = 0; // Red maximum value
int greenMin = 0; // Green minimum value
int greenMax = 0; // Green maximum value
int blueMin = 0; // Blue minimum value
int blueMax = 0; // Blue maximum value


int frequency = 0;
int red_value = 0;
int green_value = 0;
int blue_value = 0;

char currentColor;
int confirmed = 1;

int faceValue = 0;   // FRONT = 0, RIGHT = 1, BACK = 2, LEFT = 3, UP = 4, DOWN = 5
int posValue = 0;    // 0 to 8
int inputCompleted = 0;
int validInput = 0;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
    
  // LCD input pins  
  //pinMode(ledPin, OUTPUT);
  //pinMode(buttonPin, INPUT);

  

  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  // Print a message on both lines of the LCD.
  //lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
  //lcd.print("Hello world!");
  
  //lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
  //lcd.print("LCD Tutorial");
  
  Serial.begin(9600);
}

void makeSpace(int row, int pos){
  for(int i = pos; i < 16; i++){
    lcd.setCursor(i, row);
    lcd.print(" ");
  }
}

void loop() {


//
//  if(inputCompleted == 1){
//    break;
//  }

  takeInputState = digitalRead(takeInput);
  confirmInputState = digitalRead(confirmInput);
  retakeInputState = digitalRead(retakeInput);




  if(faceValue == 0){
    lcd.setCursor(2, 0);
    lcd.print("FRONT");

    lcd.setCursor(0, 1);
    lcd.print("INPUT: ");
    lcd.print(posValue);

  }




  if(posValue == 9){
    posValue %= 9;
    faceValue += 1;

    lcd.clear();
    lcd.setCursor(2, 0);
    if(faceValue == 1)
    {
      lcd.print("RIGHT");
    }
    else if(faceValue == 2)
    {
      lcd.print("BACK");
    }
    else if(faceValue == 3)
    {
      lcd.print("LEFT");
    }
    else if(faceValue == 4)
    {
      lcd.print("UP");
    }
    else if(faceValue == 5)
    {
      lcd.print("DOWN");
    }

    else if(faceValue == 6)
    {
      inputCompleted = 1;
      lcd.clear();
      lcd.print("INPUT COMPLETED");
      //break;
    }
    

    lcd.setCursor(0, 1);
    lcd.print("INPUT: 0");

    //delay(1000);
    
  }

  
  //lcd.print(posValue);

  //delay(1000);


 if (confirmInputState == HIGH && validInput == 1) {
    confirmed = 1;
    validInput = 0;
    posValue += 1;

    lcd.setCursor(7, 1);
    lcd.print(posValue);
 }

 if(retakeInputState == HIGH){
  confirmed = 1;
 }

  

//   check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (takeInputState == HIGH && confirmed == 1) {

    Serial.print("takeinput er vitore");
    confirmed = 0;
    validInput = 1;
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    

      // Setting red filtered photodiodes to be read
        digitalWrite(S2,LOW);
        digitalWrite(S3,LOW);
        // Reading the output frequency
        frequency = pulseIn(sensorOut, LOW);
      
        red_value = frequency;

        // Printing the value on the serial monitor
        Serial.print("R= ");//printing name
        Serial.print(frequency);//printing RED color frequency
        Serial.print("  ");
        delay(100);
        
        // Setting Green filtered photodiodes to be read
        digitalWrite(S2,HIGH);
        digitalWrite(S3,HIGH);
      // Reading the output frequency
        frequency = pulseIn(sensorOut, LOW);

        green_value = frequency;
  
      //  green_value = map(frequency, greenMin,greenMax,255,0);

      // Printing the value on the serial monitor
        Serial.print("G= ");//printing name
        Serial.print(frequency);//printing RED color frequency
        Serial.print("  ");
        delay(100);
  
        // Setting Blue filtered photodiodes to be read
        digitalWrite(S2,LOW);
        digitalWrite(S3,HIGH);
        // Reading the output frequency
        frequency = pulseIn(sensorOut, LOW);

        blue_value = frequency;
  
        //  blue_value = map(frequency, blueMin,blueMax,255,0);

        // Printing the value on the serial monitor
        Serial.print("B= ");//printing name
        Serial.print(frequency);//printing RED color frequency


        if((red_value > 40 && red_value < 80) && (green_value > 45 && green_value < 70 ) && (blue_value > 30 && blue_value < 60))
        {
          Serial.print(" WHITE ");
          currentColor = 'W';
        }
        else  if((red_value > 15 && red_value < 35) && (green_value > 90 && green_value < 140 ) && (blue_value >70 && blue_value < 110))
        {
          Serial.print(" ORANGE ");
          currentColor = 'O';

        }
        else if((red_value > 35 && red_value < 90) && (green_value > 30 && green_value < 100 ) && (blue_value >55 && blue_value < 130))
        {
          Serial.print(" YELLOW ");
          currentColor = 'Y';

        }

        else{

        if(red_value < green_value)
        {
          if(red_value < blue_value){
            Serial.print(" RED ");
            currentColor = 'R';

          }
          else{
            Serial.print(" BLUE ");
            currentColor = 'B';

          }
        }
        else{
          if(green_value < blue_value){
            Serial.print(" GREEN ");
            currentColor = 'G';

          }
          else{
            Serial.print(" BLUE ");
            currentColor = 'B';

          }
        }

        
      }

        makeSpace(1, 10);
        lcd.setCursor(10, 1);
        if(currentColor == 'R'){
          lcd.print("RED");
        }
        else if(currentColor == 'G'){
          lcd.print("GREEN");

        } 
        else if(currentColor == 'B'){
          lcd.print("BLUE");
        } 

        else if(currentColor == 'O'){
          lcd.print("ORANGE");
        } 

        else if(currentColor == 'Y'){
          lcd.print("YELLOW");
        } 

        else if(currentColor == 'W'){
          lcd.print("WHITE");
        } 
        

      Serial.println("  ");
      delay(100);
    }
     
    else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }


  
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);

  red_value = frequency;

  //Serial.println("map er age");
//  red_value = map(frequency, redMin,redMax,255,0);

  //Serial.println("map er pore");

  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);

  green_value = frequency;
  
//  green_value = map(frequency, greenMin,greenMax,255,0);

  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);

  blue_value = frequency;
  
//  blue_value = map(frequency, blueMin,blueMax,255,0);

  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(frequency);//printing RED color frequency



//  if((red_value > 235 && red_value < 280) && (green_value > 170 && green_value < 210 ) && (blue_value > 75 && blue_value < 110))
//  {
//    Serial.print(" BLUE ");
//  } 
//  if((red_value > 140 && red_value < 180) && (green_value > 65 && green_value < 95 ) && (blue_value > 95 && blue_value < 125))
//  {
//    Serial.print(" GREEN ");
//  }  
//  if((red_value > 90 && red_value < 130) && (green_value > 215 && green_value < 250 ) && (blue_value > 145 && blue_value < 170))
//  {
//    Serial.print(" RED ");
//  }



   makeSpace(0, 10);
   lcd.setCursor(10, 0);


  if((red_value > 40 && red_value < 80) && (green_value > 45 && green_value < 70 ) && (blue_value > 30 && blue_value < 60))
  {
    Serial.print(" WHITE ");
    lcd.print("WHITE");
  }
  else  if((red_value > 15 && red_value < 35) && (green_value > 90 && green_value < 140 ) && (blue_value >70 && blue_value < 110))
  {
    Serial.print(" ORANGE ");
    lcd.print("ORANGE");
  }
  else if((red_value > 35 && red_value < 90) && (green_value > 30 && green_value < 100 ) && (blue_value >55 && blue_value < 130))
  {
    Serial.print(" YELLOW ");
    lcd.print("YELLOW");
  }

  else{

    if(red_value < green_value)
    {
      if(red_value < blue_value){
        Serial.print(" RED ");
        lcd.print("RED");
      }
      else{
        Serial.print(" BLUE ");
        lcd.print("BLUE");
      }
    }
    else{
      if(green_value < blue_value){
        Serial.print(" GREEN ");
        lcd.print("GREEN");
      }
      else{
        Serial.print(" BLUE ");
        lcd.print("BLUE");
      }
    }
  }


  Serial.println("  ");


  
  delay(100);

}
