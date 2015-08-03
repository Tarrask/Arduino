#include <SPI.h>
#include <printf.h>
#include <RF24.h>

#define RF_CHANNEL 0x55
#define ADDRESS ((const uint8_t*)"1test")

RF24 radio(8,7);

int counter = 0;

void setup() {
  Serial.begin(9600);
  printf_begin();
  Serial.println("Starting ...");

  radio.begin();
  delay(1000);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(8);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(0,0);
  radio.openWritingPipe(ADDRESS);
  delay(1000);
  
  Serial.print("Size of payload: ");
  Serial.println(sizeof("hello !"));
  
  Serial.println("start blasting data!");

  radio.printDetails();
}

void loop() {
  counter = 0;
  for(int i = 0; i < 100; i++) {
     if(radio.write("hello !", 8)) {
      counter++;
    }
  }
  Serial.println(counter);
  radio.txStandBy();
  delay(1000);
}
