#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <IRremote.h>

#define ledGreen 10
#define ledRed 11
#define servo 12
#define pir A0
#define speaker 3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);

Servo myservo;
int pirValue = 0; //Riasztóhoz
int previousMillis = 0;
int interval = 150;
int state = 0;
char key;
String code = "";

#define IR_RECEIVE_PIN 2
decode_results results;

void setup()
{
  Serial.begin(9600);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(pir,INPUT);
  pinMode(speaker,OUTPUT);
  myservo.attach(servo);
  myservo.write(180);
  IrReceiver.begin(IR_RECEIVE_PIN);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(100);
  display.clearDisplay();
  display.display();
  delay(100);

}

char getKey()
{
  char key;
  if (IrReceiver.decode())
      {
      switch(IrReceiver.decodedIRData.command)
        {
          case 69: key = '1';
           break;
          case 70: key =  '2';
           break;
          case 71: key =  '3';
           break;
          case 68: key =  '4';
           break;
          case 64: key =  '5';
           break;
          case 67: key =  '6';
           break;
          case 7: key =  '7';
           break;
          case 21: key =  '8';
           break;
          case 9: key =  '9';
           break;
          case 25: key =  '0';
           break;
          case 22: key =  'D';
           break;
          case 13: key =  'C';
           break;
          case 28: key =  'A';
           break;
          default:
            break;
        }
        IrReceiver.resume();
        return key;
      }
  key = 'F';
  return key;
}


void loop()
{
  
  //Kikapcsolt állapot
  if(state == 0)
  {
    digitalWrite(ledGreen, LOW);
    display.clearDisplay();
    display.print("State 0");
    while(true)
    {
      key = getKey();
      if(key == 'D'){state = 1; break;}
    }
  }
  
  //Bekapcsolt, nem készenléti állapot
  if(state == 1)
  {
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);

    myservo.write(180);
    display.clearDisplay();
    code = "";
    while(true)
    {
      key = getKey();
      
      if(key == 'D')
      {state = 0;
       break;}
      else if(key == 'C')
      {
        code = "";
        display.clearDisplay();
      }
      else if(key == 'A')
      {
        if(code == "911")
        {
          state = 2;
          break;
        }
        else
        {
          code = "";
          display.clearDisplay();
          display.print("Invalid code!");
          delay(2000);
          display.clearDisplay();
          display.print("Try again!");
          delay(2000);
          display.clearDisplay();
        }    
      }
      else if(code.length() < 3 && key != 'F')
      {
        code = code + key;
        display.print(key);
      }
    }
    
  }
  
  //Bekapcsolt, készenléti állapot
  if(state == 2)
  {
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
    display.clearDisplay();
    code = "";
    myservo.write(0);
    while(true)
    {
      key = getKey();
      if(key == 'C')
      {
        code = "";
        display.clearDisplay();
      }
      else if(key == 'A')
      {
        if(code == "911")
        {
          state = 1;
          break;
        }
        else
        {
          code="";
          display.clearDisplay();
          display.print("Invalid code!");
          delay(2000);
          display.clearDisplay();
          display.print("Try again!");
          delay(2000);
          display.clearDisplay();
        }    
      }
      else if(code.length() < 3 && key != 'F' && key != 'D')
      {
        code = code + key;
        display.print(key);
      }
      pirValue = analogRead(pir);
      if(pirValue < 750)
      {
        state = 3;
        break;
      }
    }
  }
  
  
  //Riasztási állapot
  if(state == 3)
  {
    display.clearDisplay();
    code = "";
    int wrongCodeCounter = 0;
    int ledState = LOW;
    bool invalid = false;
    
    while(true)
    {   
      //LED + Hangszóró 
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval)
      {
          previousMillis = currentMillis;
          tone(speaker,440, 150);
          if (ledState == LOW)
            {
             ledState = HIGH;
            } 
          else 
            {
              ledState = LOW;
            }
        digitalWrite(ledRed, ledState);
      }
      
      key = getKey();
      
      if(key == 'C')
      {
        code = "";
        display.clearDisplay();
      }
      else if(key == 'A')
      {
        if(code == "911")
        {
          state = 1;
          break;
        }
        else if(wrongCodeCounter < 2)
        {
          code="";
          wrongCodeCounter++;
          display.clearDisplay();
          display.print("Invalid code!");
          invalid = true;
        }
        else
        {
          state = 4;
          break;
        }
      }
      else if(code.length() < 3 && key != 'F' && key != 'D')
      {
        if(invalid == true){display.clearDisplay(); invalid = false;}
        code = code + key;
        display.print(key);
      }
    }
  }
  
  
  //Lezárt riasztási állapot
  if(state == 4)
  {
    display.clearDisplay();
    display.print("Closed!");
    int ledState = LOW;
    
    while(true)
    {   
      //LED + Hangszóró 
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval)
      {
          previousMillis = currentMillis;
          tone(speaker,440, 150);
          if (ledState == LOW)
            {
             ledState = HIGH;
            } 
          else 
            {
              ledState = LOW;
            }
        digitalWrite(ledRed, ledState);
      }
    }
  }
}
