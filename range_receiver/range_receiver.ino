#include <SPI.h>

#include <RF24.h>

#define RF_CHANNEL 0x55
#define ADDRESS ((const uint8_t*)"1test")

RF24 radio(8,7);

int counter = 0;

void setup() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(8);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(0,0);
  radio.openReadingPipe(1,ADDRESS);
  radio.startListening();
}

void loop() {
  if(radio.available()) {
    byte payload[8] = {0};
    radio.read(payload, 8);
  }
}
