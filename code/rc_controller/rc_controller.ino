#include <RingBuf.h>
#include <SPI.h> 
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>
// Keep in Mind: Bluetooth modules need pairing before they send / receive data.
#include <ArduinoBlue.h>

#define DEBUG false
// Pin for battery monitoring
#define BATTERY_STATUS_PIN A0

// Pins for the potis of the 2 joysticks
#define AXIS1_X_PIN A4
#define AXIS1_Y_PIN A5
#define AXIS1_Z_PIN A6
#define AXIS2_X_PIN A3
#define AXIS2_Y_PIN A2
#define AXIS2_Z_PIN A1

// Threshold for interrupt handling of button presses to avoid multiple triggers
#define BUTTON_THRESHOLD 200
// Using interrupt capable digital pins for the 4 pushbuttons
#define BUTTON_1 19
#define BUTTON_2 20
#define BUTTON_3 18
#define BUTTON_4 21

// Connectors for the NRF24L01
#define NRF_MISO 50 
#define NRF_MOSI 51
#define NRF_SCK 52 
#define NRF_CE 2
#define NRF_CSN 3

// Connectors for the LCD Display
#define LCD_RS 13
#define LCD_ENABLE 12
#define LCD_D0 8
#define LCD_D1 9
#define LCD_D2 10
#define LCD_D3 11
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

// Size of the ringbuffer which will be used for a running average to smooth out
// the jittering of the joystick knobs. 
#define BUFFER_SIZE 3
 
int axis1_x;
RingBuf<int,BUFFER_SIZE> x1;
int axis1_y;
RingBuf<int,BUFFER_SIZE> y1;
int axis1_z;
RingBuf<int,BUFFER_SIZE> z1;
int axis1_b;

int axis2_x;
RingBuf<int,BUFFER_SIZE> x2;
int axis2_y;
RingBuf<int,BUFFER_SIZE> y2;
int axis2_z;
RingBuf<int,BUFFER_SIZE> z2;
int axis2_b;

// Button state
byte b1 = 0;
byte b2 = 0;
byte b3 = 0;
byte b4 = 0;

// Using 8 PIN Mode since faster
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

RF24 radio(NRF_CE,NRF_CSN);
const byte address[5] = {'0','0','0','0','1'};
byte dataToSend[16];

#define MODE_SETUP 0
#define MODE_BT 1
#define MODE_24GHZ 2

int mode = MODE_SETUP;
int cursorPosition = 0;
int removed;
long lastButtonPressDetected;
void modeSelection() {
    lcd.setCursor(2,0);
    lcd.print("Bluetooth Mode");
    lcd.setCursor(2,1);
    lcd.print("Wifi Mode");

    lcd.setCursor(0,0);
    lcd.print(">");

    while (mode == MODE_SETUP){
      lcd.setCursor(0,0);
      lcd.print(" ");
      lcd.setCursor(0,1);
      lcd.print(" ");
      lcd.setCursor(0,cursorPosition);
      lcd.print(">");
      delay(50);
    }
    lcd.clear();
    lcd.setCursor(14,0);
    if (mode == MODE_BT){
      lcd.print("BT");
    } else {
      lcd.print("24");
    }
}
void configurePushbuttons() {
    pinMode(BUTTON_1,INPUT_PULLUP);
    pinMode(BUTTON_2,INPUT_PULLUP);
    pinMode(BUTTON_3,INPUT_PULLUP);
    pinMode(BUTTON_4,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1),button1Pressed,FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2),button2Pressed,FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_3),button3Pressed,FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_4),button4Pressed,FALLING);
}

void setup(){
    Serial.begin(9600);
    // Bluetooth Serial on Serial2
    Serial2.begin(9600);
    configurePushbuttons();
    // Initially fill the ringbuffers
    for (int i=0 ; i<BUFFER_SIZE ;i++){
        x1.push(0); 
        y1.push(0);
        z1.push(0);
        x2.push(0);
        y2.push(0);
        z2.push(0);
    }
    lcd.begin(16, 2);
    lcd.clear();
    modeSelection();
    Serial.println(radio.begin());
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.setRetries(3,5);
    radio.openWritingPipe(address);
}   

int average(RingBuf<int,BUFFER_SIZE> *buff){
    long sum = 0;
    for (int i=0 ; i<BUFFER_SIZE ; i++){        
        int val = buff->operator[](i);
        sum += val;    
    }
    return sum/BUFFER_SIZE;
}
void readAllAnalogPositions() {
    axis1_x = analogRead(AXIS1_X_PIN);
    lcd.setCursor(0, 1);
    lcd.print(map(axis1_x,0,1024,0,99));
    axis1_y = analogRead(AXIS1_Y_PIN);
    lcd.setCursor(3,1);
    lcd.print(map(axis1_y,0,1024,0,99));
    axis1_z = analogRead(AXIS1_Z_PIN);
    lcd.setCursor(6,1);
    lcd.print(map(axis1_z,0,1024,0,99));
    axis1_x = analogRead(AXIS1_X_PIN);
    lcd.setCursor(9, 1);
    lcd.print(map(axis2_x,0,1024,0,99));
    axis1_y = analogRead(AXIS1_Y_PIN);
    lcd.setCursor(12,1);
    lcd.print(map(axis2_y,0,1024,0,99));
    axis1_z = analogRead(AXIS1_Z_PIN);
    lcd.setCursor(15,1);
    lcd.print(map(axis2_z,0,1024,0,99));
    x1.pop(removed);
    y1.pop(removed);
    z1.pop(removed);
    x1.push(axis1_x);
    y1.push(axis1_y);
    z1.push(axis1_z);
    
    axis2_x = analogRead(AXIS2_X_PIN);
    axis2_y = analogRead(AXIS2_Y_PIN);
    axis2_z = analogRead(AXIS2_Z_PIN);
    x2.pop(removed);
    y2.pop(removed);
    z2.pop(removed);
    x2.push(axis2_x);
    y2.push(axis2_y);
    z2.push(axis2_z);
}

void printStatusToSerial() {
    if (DEBUG) {
      Serial.print(average(&x1));Serial.print(',');
      Serial.print(average(&y1));Serial.print(',');
      Serial.print(average(&z1));Serial.print(',');
      Serial.print(average(&x2));Serial.print(',');
      Serial.print(average(&y2));Serial.print(',');
      Serial.print(average(&z2));Serial.print(',');
      Serial.print(b1);Serial.print(',');
      Serial.print(b2);Serial.print(',');
      Serial.print(b3);Serial.print(',');
      Serial.print(b4);Serial.print(',');
      Serial.print(analogRead(BATTERY_STATUS_PIN));Serial.print('\n');
    }
}
void send() {
    int toConvert = average(&x1);
    byte firstByte = byte(toConvert >> 8);
    byte secondByte = byte(toConvert & 0x00FF);
    bool rslt;
    dataToSend[0] = firstByte;
    dataToSend[1] = secondByte;
    toConvert = average(&y1);
    firstByte = byte(toConvert >> 8);
    secondByte = byte(toConvert & 0x00FF);
    dataToSend[2] = firstByte;
    dataToSend[3] = secondByte;
    toConvert = average(&z1);
    firstByte = byte(toConvert >> 8);
    secondByte = byte(toConvert & 0x00FF);
    dataToSend[4] = firstByte;
    dataToSend[5] = secondByte;
    toConvert = average(&x2);
    firstByte = byte(toConvert >> 8);
    secondByte = byte(toConvert & 0x00FF);
    dataToSend[6] = firstByte;
    dataToSend[7] = secondByte;
    toConvert = average(&y2);
    firstByte = byte(toConvert >> 8);
    secondByte = byte(toConvert & 0x00FF);
    dataToSend[8] = firstByte;
    dataToSend[9] = secondByte;
    toConvert = average(&z2);
    firstByte = byte(toConvert >> 8);
    secondByte = byte(toConvert & 0x00FF);
    dataToSend[10] = firstByte;
    dataToSend[11] = secondByte;
    dataToSend[12] = b1;
    dataToSend[13] = b2;
    dataToSend[14] = b3;
    dataToSend[15] = b4;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );    
}
void loop(){
    lcd.setCursor(0,0);
    lcd.print("RC-Remote");    
    readAllAnalogPositions();           
    if (mode == MODE_BT){
        char command = getCommand(axis2_x,axis1_y);
        Serial.println(command);
        Serial2.print(command);
        delay(50);
    } else if (mode == MODE_24GHZ){
        send();
        delay(50);       
    } else {
        Serial.print("DEFAULT");
        delay(2000);               
    }    
    printStatusToSerial();
       
}

void button1Pressed(){
  if (millis() - lastButtonPressDetected < BUTTON_THRESHOLD){
    return;
  }
  lastButtonPressDetected = millis();
  Serial.println("B1");
  switch (mode) {
  case MODE_SETUP:
    cursorPosition = cursorPosition == 0 ? 1 : 0;
    break;
  default:
    b1 = (b1 == 0 ? 1 : 0);
    break;
  }
  delay(50);
}
void button2Pressed(){
  if (millis() - lastButtonPressDetected < BUTTON_THRESHOLD){
    return;
  }
  lastButtonPressDetected = millis();
  Serial.println("B2");
  switch (mode){
  case MODE_SETUP:
    cursorPosition = cursorPosition == 0 ? 1 : 0;
    break;
  default:
    b2 = (b2 == 0 ? 1 : 0);
    break;
  }
  delay(50);
}
void button3Pressed(){
  if (millis() - lastButtonPressDetected < BUTTON_THRESHOLD){
    return;
  }
  lastButtonPressDetected = millis();
    Serial.println("B3");
    b3 = (b3 == 0 ? 1 : 0);
    delay(50);
}
void button4Pressed(){
  if (millis() - lastButtonPressDetected < BUTTON_THRESHOLD){
    return;
  }
  lastButtonPressDetected = millis();
  Serial.println("B4");
  Serial.println(mode);
  switch (mode) {
  case MODE_SETUP:
    if (cursorPosition == 0) {
      mode = MODE_BT;
    } else {
      mode = MODE_24GHZ;
    }
    break;
  default:
    b4 = (b4 == 0 ? 1 : 0);
    break;
  }
  delay(50);
}
