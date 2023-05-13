#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define ledGreen 10
#define ledRed 11
#define servo 12
#define pir A0
#define speaker A1

//Keypadhoz
char keymap[4][4]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

byte rowPins[4] = {2,3,4,5}; 
byte colPins[4]= {6,7,8,9};

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, 4, 4);

Servo myservo;
LiquidCrystal_I2C lcd(32,16,2);
int pirValue = 0; //Riasztóhoz
int previousMillis = 0;
int interval = 150;
int state = 0;
char key;
String code = "";

void setup()
{
  Serial.begin(9600);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(pir,INPUT);
  pinMode(speaker,OUTPUT);
  myservo.attach(servo);
  lcd.init();
}

void loop()
{
  
  //Kikapcsolt állapot
  if(state == 0)
  {
    digitalWrite(ledGreen, LOW);
    lcd.noBacklight();
    lcd.clear();
    while(true)
    {
      key = myKeypad.getKey();
      if(key == 'D'){state = 1; break;}
    }
  }
  
  //Bekapcsolt, nem készenléti állapot
  if(state == 1)
  {
    digitalWrite(ledGreen, HIGH);
    myservo.write(180);
    lcd.clear();
    code = "";
    lcd.backlight();
    while(true)
    {
      key = myKeypad.getKey();
      if(key == 'D')
      {state = 0;
       break;}
      else if(key == 'C')
      {
        code = "";
        lcd.clear();
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
          lcd.clear();
          lcd.print("Invalid code!");
          delay(2000);
          lcd.clear();
          lcd.print("Try again!");
          delay(2000);
          lcd.clear();
        }    
      }
      else if(code.length() < 3 && key != NO_KEY)
      {
        code = code + key;
        lcd.print(key);
      }
    }
    
  }
  
  //Bekapcsolt, készenléti állapot
  if(state == 2)
  {
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
    lcd.clear();
    code = "";
    myservo.write(0);
    while(true)
    {
      key = myKeypad.getKey();
      if(key == 'C')
      {
        code = "";
        lcd.clear();
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
          lcd.clear();
          lcd.print("Invalid code!");
          delay(2000);
          lcd.clear();
          lcd.print("Try again!");
          delay(2000);
          lcd.clear();
        }    
      }
      else if(code.length() < 3 && key != NO_KEY && key != 'D')
      {
        code = code + key;
        lcd.print(key);
      }
      pirValue = analogRead(pir);
      if(pirValue > 100)
      {
        state = 3;
        break;
      }
    }
  }
  
  
  //Riasztási állapot
  if(state == 3)
  {
    lcd.clear();
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
      
      key = myKeypad.getKey();
      if(key == 'C')
      {
        code = "";
        lcd.clear();
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
          lcd.clear();
          lcd.print("Invalid code!");
          invalid = true;
        }
        else
        {
          state = 4;
          break;
        }
      }
      else if(code.length() < 3 && key != NO_KEY && key != 'D')
      {
        if(invalid == true){lcd.clear(); invalid = false;}
        code = code + key;
        lcd.print(key);
      }
    }
  }
  
  
  //Lezárt riasztási állapot
  if(state == 4)
  {
    lcd.clear();
    lcd.print("Closed!");
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