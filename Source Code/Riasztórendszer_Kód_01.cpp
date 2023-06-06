#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <IRremote.h>
#include <MFRC522.h>

#define ledGreen 5
#define ledRed 6
#define servo 7
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
int state = 4;
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
  delay(100);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
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
    digitalWrite(ledRed, LOW);
    delay(100);
    display.clearDisplay();
    display.print("Hell");
    display.display();
    delay(100);
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
   delay(100);
   display.clearDisplay();
   display.print("Hello");
   display.display();
   delay(100); 
    

    myservo.write(180);
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
        display.setCursor(0,0);
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
          display.setCursor(0,0); 
          display.println("Invalid code");
          display.display();
          delay(2000);
          display.clearDisplay();
          display.setCursor(0,0); 
          display.println("Try again!");
          display.display();
          delay(2000);
          display.clearDisplay();
          display.setCursor(0,0);
          display.display();
        }    
      }
      else if(code.length() < 3 && key != 'F')
      {
        code = code + key;
        display.clearDisplay();
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
    delay(100);
    display.clearDisplay();
    display.display();
    delay(100); 
    int value = analogRead(pir);
    myservo.write(0);
    while(true)
    {
     
      if(key == 'C')
      {
        code = "";
        display.clearDisplay();
        display.setCursor(0,0);
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
          display.setCursor(0,0); 
          display.println("Invalid code");
          display.display();
          delay(2000);
          display.clearDisplay();
          display.setCursor(0,0); 
          display.println("Try again!");
          display.display();
          delay(2000);
          display.clearDisplay();
          display.setCursor(0,0);
          display.display();
        }    
      }
      else if(code.length() < 3 && key != 'F' && key != 'D')
      {
        code = code + key;
        display.clearDisplay();
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
      key = getKey();
    }
  }
  
  
  //Riasztási állapot
  if(state == 3)
  {
    code = "";
    digitalWrite(ledRed, HIGH);
    delay(100);
    display.clearDisplay();
    display.display();
    int wrongCodeCounter = 0;
    bool invalid = false;
    tone(speaker,440, 150);
    delay(500);
    tone(speaker,440, 150);
    delay(500);
    tone(speaker,440, 150);
    delay(500);
  
    while(true)
    {   
      key = getKey();
      if(key == 'C')
      {
        code = "";
        display.clearDisplay();
        display.setCursor(0,0);
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
          display.setCursor(0,0); 
          display.println("Invalid code");
          display.display();
          delay(2000);
          display.clearDisplay();
          display.setCursor(0,0);
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
        display.setCursor(0,0); 
        display.println(code);
        display.display();
      }
    }
  }
  
  
  //Lezárt riasztási állapot
  if(state == 4)
{
  digitalWrite(ledRed, HIGH);
  delay(100);
  display.clearDisplay();
  display.display();
  delay(100); 
    display.setCursor(0,0);
    display.print("Closed!");
    display.display();
    
    while(state == 4)
    {   
      //LED + Hangszóró 
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval)
      {
          previousMillis = currentMillis;
          tone(speaker,440, 150);
          
          
      }
      if (rfid.PICC_IsNewCardPresent())
      {
          rfid.PICC_ReadCardSerial(); //Megfelelő érték beállításához
          if(rfid.uid.uidByte[0] == 74)
          {
            //setup();
            state = 0;
          }
      }
    }
  }
}
