#include <RingBuf.h>
#include <RF24.h>
#include <LiquidCrystal.h>
// Keep in Mind: Bluetooth modules need pairing before they send / receive data.
#include <ArduinoBlue.h>

// Pin for battery monitoring
#define BATTERY_STATUS_PIN A0

// Pins for the potis of the 2 joysticks
#define AXIS1_X_PIN A1
#define AXIS1_Y_PIN A2
#define AXIS1_Z_PIN A3
#define AXIS2_X_PIN A4
#define AXIS2_Y_PIN A5
#define AXIS2_Z_PIN A6

// Using interrupt capable digital pins for the 4 pushbuttons
#define BUTTON_1 18
#define BUTTON_2 19
#define BUTTON_3 20
#define BUTTON_4 21

// Connectors for the NRF24L01
#define NRF_MISO 50 
#define NRF_MOSI 51
#define NRF_SS 52 
#define NRF_SCK 53
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
const byte address[6] = "00001";

void setup(){
    Serial.begin(9600);
    // Bluetooth Serial on Serial2
    Serial2.begin(9600);
    pinMode(BUTTON_1,INPUT_PULLUP);
    pinMode(BUTTON_2,INPUT_PULLUP);
    pinMode(BUTTON_3,INPUT_PULLUP);
    pinMode(BUTTON_4,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1),button1Pressed,FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2),button2Pressed,FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_3),button3Pressed,FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_4),button4Pressed,FALLING);
    // Initially fill the ringbuffers
    for (int i=0 ; i<BUFFER_SIZE ;i++){
        x1.push(0); 
        y1.push(0);
        z1.push(0);
        x2.push(0);
        y2.push(0);
        z2.push(0);
    }
    
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    lcd.begin(16, 2);
    lcd.clear();
}   

int average(RingBuf<int,BUFFER_SIZE> *buff){
    long sum = 0;
    for (int i=0 ; i<BUFFER_SIZE ; i++){        
        int val = buff->operator[](i);
        sum += val;    
    }
    return sum/BUFFER_SIZE;
}

int removed;
void loop(){
    lcd.setCursor(0,0);
    lcd.print("Universal Remote");
    lcd.setCursor(0, 1);
    // Print a message to the LCD.
    lcd.print("LCD Check");
    
    axis1_x = analogRead(AXIS1_X_PIN);
    axis1_y = analogRead(AXIS1_Y_PIN);
    axis1_z = analogRead(AXIS1_Z_PIN);
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
    
    Serial.print(average(&x1));Serial.print(',');
    Serial.print(average(&y1));Serial.print(',');
    Serial.print(average(&z1));Serial.print(',');
    Serial.print(average(&x2));Serial.print(',');
    Serial.print(average(&y2));Serial.print(',');
    Serial.print(average(&z2));Serial.print(',');
    Serial.print(b1);Serial.print(',');
    Serial.print(b2);Serial.print(',');
    Serial.print(b3);Serial.print(',');
    Serial.print(b4);Serial.print('\n');
    
}

void button1Pressed(){
    b1 = (b1 == 0 ? 1 : 0);
}
void button2Pressed(){
    b2 = (b2 == 0 ? 1 : 0);
}
void button3Pressed(){
    b3 = (b3 == 0 ? 1 : 0);
}
void button4Pressed(){
    b4 = (b4 == 0 ? 1 : 0);
}
