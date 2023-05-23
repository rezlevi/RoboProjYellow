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
char keymap[4][4] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[4] = {2, 3, 4, 5};
byte colPins[4] = {6, 7, 8, 9};

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, 4, 4);

Servo myservo;
LiquidCrystal_I2C lcd(32, 16, 2);
int pirValue = 0; // Riasztóhoz
int previousMillis = 0;
int interval = 150;
int state = 0;
char key;
String code = "";
int ledState = LOW;

bool offState() //Kikapcsolt állapot
{
  digitalWrite(ledGreen, LOW);
  lcd.noBacklight();
  lcd.clear();

  while (true)
  {
    key = myKeypad.getKey();
    if (key == 'D')
    {
      state = 1;
      return true;
    }
  }
  return false;
}

bool onState() //Bekapcsolt, nem készenléti állapot
{
  initialiseLCD();
  myservo.write(180);
  lcd.backlight();
  lcd.print("Security code:");
  lcd.setCursor(0, 1);
  int wrongCodeCounter = 0;
  
  while (true)
  {
    key = myKeypad.getKey();
    switch (key)
    {
      case 'D':
      	state = 0;
      	return false;
      	break;
      case 'C':
      	initialiseLCD();
      	break;
      case 'A':
      	if (code == "911")
        {
          state = 2;
          break;
        }
        else
        {
          handleInvalidCode();
          wrongCodeCounter++;
          break;
        }
    }
    if (code.length() < 3 && key != NO_KEY)
    {
      code += key;
      lcd.print(key);
    }
    if (wrongCodeCounter >= 3)
    {
      state = 4;
    }
  }
  return false;
}

bool standbyState() //Bekapcsolt, készenléti állapot
{
  initialiseLCD();
  lcd.print("Ready to serve");
  myservo.write(0);

  while (true)
  {
    key = myKeypad.getKey();
    checkKey(key);
    pirValue = analogRead(pir);
    if (pirValue > 100)
    {
      state = 3;
      return true;
    }
  }
  return false;
}

bool alarmState() //Riasztási állapot
{
  initialiseLCD();
  lcd.print("INTRUDER HERE");

  while (true)
  {
    //LED + Hangszóró
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      tone(speaker, 500, 100);
      delay(200);
      tone(speaker, 440, 150);
      delay(200);
      alarmLights();
      digitalWrite(ledRed, ledState);
    }

    key = myKeypad.getKey();
    checkKey(key);
    if (state == 4)
    {
      return true;
    }
  }
}

void lockDownState() // Lezárt riasztási állapot
{
  initialiseLCD();
  lcd.print("CLOSED");

  while (true)
  {
    //LED + Hangszóró
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      tone(speaker, 440, 150);
      alarmLights();
      digitalWrite(ledRed, ledState);
    }
  }
}

void alarmLights()
{
  if (ledState == LOW)
  {
    ledState = HIGH;
  }
  else
  {
    ledState = LOW;
  }
}

void initialiseLCD()
{
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledRed, HIGH);
  lcd.clear();
  code = "";
}

void checkKey(char key)
{
  int wrongCodeCounter = 0;
  switch (key)
  {
    case 'D':
      state = 1;
      break;
    case 'C':
      initialiseLCD();
      break;
    case 'A':
      if (code == "911")
      {
        state = 1;
      }
      else
      {
        handleInvalidCode();
        wrongCodeCounter++;
      }
      break;
    default:
      if (code.length() < 3 && key != NO_KEY && key != 'D')
      {
        code += key;
        lcd.print(key);
      }
      break;
  }
  if (wrongCodeCounter >= 3)
  {
    state = 4;
  }
}

void handleInvalidCode()
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

void setup()
{
  Serial.begin(9600);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(speaker, OUTPUT);
  myservo.attach(servo);
  lcd.init();
}

void loop()
{
  switch (state)
  {
    case 0:
		offState();
        break;
    case 1:
      if (offState())
      {
        onState();
        break;
      }
      break;
    case 2:
      if (onState())
      {
        standbyState();
        break;
      }
      break;
    case 3:
      if (standbyState())
      {
        alarmState();
        break;
      }
      break;
    case 4:
      lockDownState();
      break;
  }
}
