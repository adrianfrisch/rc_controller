// A simple receiver sketch
// 

#include <RF24.h>

#define NRF_CE 7
#define NRF_CSN 8

RF24 radio(NRF_CE,NRF_CSN);
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}