#include <SPI.h>

#include <RF24.h>

#define RF_CHANNEL 10
#define ADDRESS ((const uint8_t*)"1tarh")

RF24 radio(8,7);

int counter = 0;

void setup() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(32);
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0,0);
  radio.openReadingPipe(1,ADDRESS);
  radio.startListening();
}

void loop() {
  if(radio.available()) {
    byte payload[32] = {0};
    radio.read(payload, 32);
  }
}
