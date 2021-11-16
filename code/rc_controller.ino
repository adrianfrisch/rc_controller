#include <RingBuf.h>

// Pins for the potis of the 2 joysticks
#define AXIS1_X_PIN A1
#define AXIS1_Y_PIN A2
#define AXIS1_Z_PIN A3
#define AXIS2_X_PIN A1
#define AXIS2_Y_PIN A2
#define AXIS2_Z_PIN A3

// Using interrupt capable digital pins for the 4 pushbuttons
#define BUTTON_1 18
#define BUTTON_2 19
#define BUTTON_3 20
#define BUTTON_4 21

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

void setup(){
    Serial.begin(9600);
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
}

int average(RingBuf* buff){
    long sum = 0;
    for (int i=0 ; i<BUFFER_SIZE ; i++)
        sum += buff[i];    
    return sum/BUFFER_SIZE;
}

void loop(){
    axis1_x = analogRead(AXIS1_X_PIN);
    axis1_y = analogRead(AXIS1_Y_PIN);
    axis1_z = analogRead(AXIS1_Z_PIN);
    x1.pop();
    y1.pop();
    z1.pop();
    x1.push(axis1_x);
    y1.push(axis1_y);
    z1.push(axis1_z);
    
    axis2_x = analogRead(AXIS2_X_PIN);
    axis2_y = analogRead(AXIS2_Y_PIN);
    axis2_z = analogRead(AXIS2_Z_PIN);
    x2.pop();
    y2.pop();
    z2.pop();
    x2.push(axis2_x);
    y2.push(axis2_y);
    z2.push(axis2_z);
    
    Serial.print(average(x1);Serial.print(',');
    Serial.print(average(y1);Serial.print(',');
    Serial.print(average(z1);Serial.print(',');
    Serial.print(average(x2);Serial.print(',');
    Serial.print(average(y2);Serial.print(',');
    Serial.print(average(z2);Serial.print(',');
    Serial.print(average(b1);Serial.print(',');
    Serial.print(average(b2);Serial.print(',');
    Serial.print(average(b3);Serial.print(',');
    Serial.print(average(b4);Serial.print('\n');
    
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