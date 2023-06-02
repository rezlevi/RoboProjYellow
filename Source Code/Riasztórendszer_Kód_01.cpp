#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <IRremote.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Mezők

#define ledGreen 10
#define ledRed 11
#define servo 12
#define pir A0
#define speaker 3
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define recvPin 2
Adafruit_SSD1306 display(OLED_RESET);
Servo myServo;
int pirValue = 0; // Riasztóhoz
unsigned long previousMillis = 0;
int interval = 150;
int state = 0;
String key;
String code = "";
IRrecv reciever(recvPin);
int ledState = LOW;
decode_results results;
unsigned long keyCodes = reciever.decodedIRData.decodedRawData;

// Metódusok

// Opcionális, még lehet, hogy jól jön
/*void decode()
{
    if (IrReceiver.decode())
    {
        reciever.resume();
    }
    delay(100);
}*/

String initialiseRemote()
{
    IrReceiver.decode();

    switch (keyCodes)
    {
        case 0xFFA25D:
            key = "1";
            break;
        case 0xFF629D:
            key = "2";
            break;
        case 0xFFE21D:
            key = "3";
            break;
        case 0xFF22DD:
            key = "4";
            break;
        case 0xFF02FD:
            key = "5";
            break;
        case 0xFFC23D:
            key = "6";
            break;
        case 0xFFE01F:
            key = "7";
            break;
        case 0xFFA857:
            key = "8";
            break;
        case 0xFF906F:
            key = "9";
            break;
        case 0xFF6897:
            key = "*";
            break;
        case 0xFF9867:
            key = "0";
            break;
        case 0xFFB04F:
            key = "#";
            break;
        case 0xFF38C7:
            key = "OK";
            break;
        default:
            display.print("ERROR: Invalid button!");
            break;
    }

    return key;
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

void initialiseOLED()
{
    display.clearDisplay();
    code = "";
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
}

void handleInvalidCode()
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

void checkKey(String remoteKey)
{
    initialiseRemote();
    int wrongCodeCounter = 0;

    if (remoteKey == "*") { state = 1; }
    else if (remoteKey == "#") { initialiseOLED(); }
    else if (remoteKey == "OK")
    {
        if (code == "911") { state = 1; }
        else
        {
            handleInvalidCode();
            wrongCodeCounter++;
        }
    }
    else if (code.length() < 3 && remoteKey != "" && remoteKey != "#")
    {
        code += remoteKey;
        display.print(remoteKey);
    }
    else
    {
        display.print("Guru mediation!");
        alarmLights();
        delay(30000);
        initialiseOLED();
        exit(0);
    }

    if (wrongCodeCounter >= 3) { state = 4; }
}

int offState()
{
    initialiseRemote();
    digitalWrite(ledGreen, LOW);
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    display.clearDisplay();

    while (true)
    {
        if (key == "#")
        {
            state = 1;
            return state;
        }
        return 0;
    }
}

int onState()
{
    initialiseRemote();
    initialiseOLED();
    myServo.write(180);
    display.ssd1306_command(SSD1306_DISPLAYON);
    display.print("Security code:");
    display.setCursor(0, 1);
    int wrongCodeCounter = 0;

    while (true)
    {
        if (key == "*")
        {
            state = 0;
            return state;
        }
        else if (key == "#") { initialiseOLED(); }
        else if (key == "OK")
        {
            if (code == "911")
            {
                state = 2;
                return state;
            }
            else
            {
                handleInvalidCode();
                wrongCodeCounter++;
            }
        }
        else if (code.length() < 3 && key != "")
        {
            code += key;
            display.print(key);
        }
        else if (wrongCodeCounter >= 3)
        {
            state = 4;
            return state;
        }
        return 0;
    }
}

int standbyState() //Bekapcsolt, készenléti állapot
{
    initialiseOLED();
    display.print("Ready to serve");
    myServo.write(0);

    while (true)
    {
        checkKey(key);
        pirValue = analogRead(pir);
        if (pirValue < 750)
        {
            state = 3;
            return state;
        }
        return 1;
    }
}

int alarmState() //Riasztási állapot
{
    initialiseOLED();
    display.print("INTRUDER HERE");

    while (true)
    {
        //LED + Hangszóró
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            tone(speaker, 500, 100);
            delay(300);
            tone(speaker, 440, 150);
            delay(300);
            alarmLights();
            digitalWrite(ledRed, ledState);
        }
        checkKey(key);
        if (state == 4)
        {
            return state;
        }
        return 1;
    }
}

void lockDownState() // Lezárt riasztási állapot
{
    initialiseOLED();
    display.print("CLOSED");

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

void setup()
{
    Serial.begin(9600);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(pir,INPUT);
    pinMode(speaker,OUTPUT);
    myServo.attach(servo);
    myServo.write(180);
    reciever.enableIRIn();
    display.ssd1306_command(SSD1306_DISPLAYON);
    delay(100);
    display.clearDisplay();
    display.display();
    delay(100);
}

void loop()
{
    switch (state)
    {
        case 0:
            offState();
            break;
        case 1:
            onState();
            break;
        case 2:
            standbyState();
            break;
        case 3:
            alarmState();
            break;
        case 4:
            lockDownState();
            break;
        default:
            display.print("Guru meditation!");
            exit(30000);
    }
}
