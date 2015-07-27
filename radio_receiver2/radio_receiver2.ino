#include <Wire.h>

#include <SPI.h>
#include <RF24.h>

#include <tarhSensors.h>

#define LED_PIN 13

#define RF24_CE_PIN 8
#define RF24_CS_PIN 7

#define RF_CHANNEL 10

const byte address[6] = "1tarh";

RF24 radio = RF24(RF24_CE_PIN, RF24_CS_PIN);
payload_t payload;


void setup() {
  Serial.begin(9600);
  Serial.println("Starting ...");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // initialise le module radio. En écoute uniquement.
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(sizeof(payload_t));
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1,address);
  radio.startListening();

  digitalWrite(LED_PIN, LOW);
  Serial.println("Starting done.");
}

void loop() {
  if(radio.available()) {
    radio.read(&payload, sizeof(payload_t));
    
   printValues(payload);
  }
}

void printValues(payload_t payload) {
    const char separator = ';';
    Serial.print(millis());
    Serial.print(separator);
    Serial.print(payload.sequence); 
    Serial.print(separator);
    Serial.print((float)(payload.tension + 150.0) / 100.0);
    Serial.print(separator);
    Serial.print((float)(payload.temperature) / 10.0);
    Serial.print(separator);
    Serial.print(payload.pressure + 55000);
    Serial.print(separator);
    Serial.print((float)payload.humidity / 100.0);
    Serial.println();
}
