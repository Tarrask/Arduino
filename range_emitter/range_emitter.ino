#include <SPI.h>

#include <RF24.h>

#define RF_CHANNEL 10
#define ADDRESS ((const uint8_t*)"1tarh")

RF24 radio(8,7);

int counter = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting ...");

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(32);
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0,0);
  radio.openWritingPipe(ADDRESS);
  
  Serial.print("Size of payload: ");
  Serial.println(sizeof("hello great and beautiful world"));
  
  Serial.println("start blasting data!");
}

void loop() {
  counter = 0;
  for(int i = 0; i < 100; i++) {
     if(radio.write("hello great and beautiful world", 32)) {
      counter++;
    }
  }
  Serial.println(counter);
  radio.txStandBy();
  delay(1000);
}
