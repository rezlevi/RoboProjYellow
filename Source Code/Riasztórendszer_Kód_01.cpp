#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <IRremote.h>
#include <MFRC522.h>

#define ledGreen 5
#define ledRed 6
#define servo 8
#define pir A0
#define speaker 3

#define SS_PIN 10
#define RST_PIN 9

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);
MFRC522 rfid(SS_PIN, RST_PIN);

Servo myservo;
int pirValue = 0; //Riasztóhoz
int previousMillis = 0;
int interval = 500;
int state = 0;
char key;
String code = "";

#define IR_RECEIVE_PIN 2
decode_results results;

void setup()
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522    
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(pir,INPUT);
  pinMode(speaker,OUTPUT);
  myservo.attach(servo);
  myservo.write(180);
  IrReceiver.begin(IR_RECEIVE_PIN);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();

}
void(* resetFunc)(void)=0;

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
    digitalWrite(ledRed, LOW);

    display.clearDisplay();
    display.display();

    while(true)
    {
      key = getKey();
      if(key == 'D'){state = 1; break;}
    }
  }
  
  //Bekapcsolt, nem készenléti állapot
  if(state == 1)
  {
   code = "";
   digitalWrite(ledGreen, HIGH);
   digitalWrite(ledRed, LOW);

   display.clearDisplay();
   display.display();
 
    
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
        display.display();
    
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
          display.display();
      
          display.setCursor(0,0); 
          display.println("Invalid code");
          display.display();
          delay(2000);
      
          display.clearDisplay();
          display.display();
      
          display.setCursor(0,0); 
          display.println("Try again!");
          display.display();
          delay(2000);
      
          display.clearDisplay();
          display.display();
      
        }    
      }
      else if(code.length() < 3 && key != 'F')
      {
        code = code + key;
    
        display.clearDisplay();
        display.display();
    
        display.setCursor(0,0); 
        display.println(code);
        display.display();
      }
      
    }
    
  }
  
  //Bekapcsolt, készenléti állapot
  if(state == 2)
  {
    code = "";
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);

    display.clearDisplay();
    display.display();
 
    int value = analogRead(pir);
    
    while(true)
    {
      key = getKey();
      if(key == 'C')
      {
        code = "";
    
        display.clearDisplay();
        display.display();
    
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
          display.display();
      
          display.setCursor(0,0); 
          display.println("Invalid code");
          display.display();
          delay(2000);
      
          display.clearDisplay();
          display.display();
      
          display.setCursor(0,0); 
          display.println("Try again!");
          display.display();
          delay(2000);
      
          display.clearDisplay();
          display.display();
      
        }    
      }
      else if(code.length() < 3 && key != 'F' && key != 'D')
      {
        code = code + key;
    
        display.clearDisplay();
        display.display();
    
        display.setCursor(0,0); 
        display.println(code);
        display.display();
      }
      
      pirValue = analogRead(pir);
      if(abs(value - pirValue) > 200)
      {
        state = 3;
        break;
      }
    }
  }
  
  
  //Riasztási állapot
  if(state == 3)
  {
    code = "";

    display.clearDisplay();
    display.display();

    display.setCursor(0,0); 
    display.print("Alarm!");
    display.display();
    int wrongCodeCounter = 0;
    bool invalid = false;
    digitalWrite(ledRed, HIGH);
    delay(200);
    digitalWrite(ledRed, LOW);
    delay(200);
    digitalWrite(ledRed, HIGH);
    delay(200);
    digitalWrite(ledRed, LOW);
    delay(200);
    digitalWrite(ledRed, HIGH);
    delay(200);
    digitalWrite(ledRed, LOW);
    delay(200);
    digitalWrite(ledRed, HIGH);
  unsigned long currentMillis = millis();
    while(true)
    {  
      key = getKey();
      if (millis() - currentMillis >= 30000)
      {
          state = 4;
          break;
      }
      if(key == 'C')
      {
        code = "";
    
        display.clearDisplay();
        display.display();
    
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
          display.display();
      
          display.setCursor(0,0); 
          display.println("Invalid code");
          display.display();
          delay(2000);
      
          display.clearDisplay();
          display.display();
      
          display.setCursor(0,0); 
          display.println("Try again!");
          display.display();
          delay(2000);
          display.clearDisplay();
          display.display();
          invalid = true;
        }
        else
        {
          state = 4;
          break;
        }
      }
      else if(code.length() < 3 && key != 'F'  && key != 'D')
      {
        if(invalid == true){Serial.println(" "); invalid = false;}
        code = code + key;
    
        display.clearDisplay();
        display.display();
    
        display.setCursor(0,0); 
        display.println(code);
        display.display();
      }
    }
  }
  
  
  //Lezárt riasztási állapot
  if(state == 4)
{
  myservo.write(0);
  digitalWrite(ledRed, HIGH);
  display.clearDisplay();
  display.display(); 
    display.setCursor(0,0);
    display.print("Closed!");
    display.display();
    
    while(state == 4)
    {   
      tone(speaker,440,150);
      delay(500);
      if (rfid.PICC_IsNewCardPresent())
      {
          rfid.PICC_ReadCardSerial(); //Megfelelő érték beállításához
          if(rfid.uid.uidByte[0] == 74)
          {
            resetFunc();
          }
      }
    }
  }
}
