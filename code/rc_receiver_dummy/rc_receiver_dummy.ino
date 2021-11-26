#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   7
#define CSN_PIN 8

const byte address[5] = {'0','0','0','0','1'};

RF24 radio(CE_PIN, CSN_PIN);

byte dataReceived[16]; // this must match dataToSend in the TX
bool newData = false;

void setup() {

    Serial.begin(9600);

    Serial.println("Receiver Dummy Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, address);
    radio.startListening();
}

void loop() {
    readData();
    printData();
}

void readData() {
    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;
    }
}

void printData() {
    if (newData == true) {
        Serial.print("Data received ");
        for (int i = 0; i < 11; i+= 2) {
          int val = int(dataReceived[i]<<8) + int(dataReceived[i+1]);
          Serial.print(val);
          Serial.print(',');
        }
        Serial.print(dataReceived[12]);
        Serial.print(',');
        Serial.print(dataReceived[13]);
        Serial.print(',');
        Serial.print(dataReceived[14]);
        Serial.print(',');
        Serial.print(dataReceived[15]);        
        Serial.println();        
        
        newData = false;
    }
}
