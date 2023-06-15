#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <IRremote.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <MFRC522.h>

// Mezők

#define ledGreen 6
#define ledRed 7
#define servo 8
#define pir A0
#define speaker 3
#define SS_PIN 10
#define RST_PIN 9
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define recvPin 2
Adafruit_SSD1306 display(OLED_RESET);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;
int initialPirValue = 0; // Riasztóhoz
unsigned long previousMillis = 0;
int interval = 30000;
int state = 0;
String key;
String code = "";
int redLedState = LOW;
int greenLedState = LOW;
decode_results results;
unsigned long keyCodes = IrReceiver.decodedIRData.command;
void(*resetFunc)(void)=0;

// Metódusok

String initialiseRemote() // Gombkiválasztás
{
    switch (keyCodes)
    {
        case 69:
            key = "1";
            break;
        case 70:
            key = "2";
            break;
        case 71:
            key = "3";
            break;
        case 68:
            key = "4";
            break;
        case 64:
            key = "5";
            break;
        case 67:
            key = "6";
            break;
        case 7:
            key =  "7";
            break;
        case 21:
            key =  "8";
            break;
        case 9:
            key =  "9";
            break;
        case 25:
            key =  "0";
            break;
        case 22:
            key =  "*";
            break;
        case 13:
            key =  "#";
            break;
        case 28:
            key =  "OK";
            break;
        default:
            key = "";
            break;
    }

    IrReceiver.resume();
    return key;
}

void decodeIRSignals() //Infravörös fény dekódolása
{
    if (IrReceiver.decode())
    {
        key = initialiseRemote();
    }
}

void alarmLights() // LED-fények kezelése
{
    if (redLedState == LOW && greenLedState == LOW)
    {
        redLedState = HIGH;
    }
    else if (redLedState == HIGH && greenLedState == LOW)
    {
        redLedState = LOW;
    }
    else if (redLedState == LOW && greenLedState == HIGH)
    {
        greenLedState = LOW;
    }
    else
    {
        redLedState = LOW;
        greenLedState = LOW;
    }
}

void initialiseOLED() // OLED kiürítése
{
    code = "";
    display.clearDisplay();
    display.display();
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
}

void handleInvalidCode() // Helytelen gombkombináció logikája
{
    code = "";
    display.clearDisplay();
    display.display();
    display.setCursor(0, 0);
    display.print("Invalid code!");
    delay(2000);
    display.clearDisplay();
    display.display();
    display.setCursor(0, 0);
    display.print("Try again!");
    delay(2000);
    display.clearDisplay();
    display.display();
}

void checkKey(String remoteKey) // Gombnyomások kiértékelése
{
    remoteKey = initialiseRemote();
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
    else if ((remoteKey != "" || remoteKey != "#") && code.length() <= 3)
    {
        initialiseOLED();
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

int offState() // Kikapcsolt állapot
{
    decodeIRSignals();
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, LOW);
    display.ssd1306_command(SSD1306_DISPLAYOFF);

    while (true)
    {
        decodeIRSignals();
        if (key == "#")
        {
            state = 1;
            return state;
        }
        return 0;
    }
}

int onState() // Bekapcsolt készenléti állapot
{
    decodeIRSignals();
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
        else if (key == "#")
        {
            initialiseOLED();
        }
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
    }

    return 0;
}

int standbyState() //Bekapcsolt, készenléti állapot
{
    initialiseOLED();
    display.print("Ready to serve");
    myServo.write(0);
    int newPirValue = analogRead(pir);

    while (true)
    {

        key = initialiseRemote();
        checkKey(key);
        initialPirValue = analogRead(pir);
        if (abs(newPirValue - initialPirValue) > 200)
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
    int wrongCodeCounter = 0;
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
            digitalWrite(ledRed, redLedState);
            digitalWrite(ledGreen, greenLedState);
            alarmLights();
        }
        key = initialiseRemote();
        checkKey(key);
        if (state == 4)
        {
            return state;
        }
        return 1;
    }
}

[[noreturn]] void lockDownState() // Lezárt riasztási állapot
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
            digitalWrite(ledRed, redLedState);
            digitalWrite(ledGreen, greenLedState);
        }
    }
}

// Főprogram

void setup()
{
    Serial.begin(9600);
    SPIClass::begin();
    rfid.PCD_Init();
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(pir,INPUT);
    pinMode(speaker,OUTPUT);
    myServo.attach(servo);
    myServo.write(180);
    IrReceiver.begin(recvPin);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    delay(100);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.display();
}

void loop()
{
    decodeIRSignals();
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
        default:
            display.print("Guru meditation!");
            exit(30000);
    }
}
