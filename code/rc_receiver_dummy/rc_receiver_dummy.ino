// A simple receiver sketch
// 
#include <SPI.h> 
#include <nRF24L01.h>
#include <RF24.h>

#define NRF_CE 7
#define NRF_CSN 8

RF24 radio(NRF_CSN,NRF_CE);
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  Serial.print(radio.begin());
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() { 
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.print("Rec: ");
    Serial.println(text); 
  }  
}
