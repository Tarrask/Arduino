#include <SPI.h>

//#include <nRF24L01.h>
#include <RF24.h>

#include <tarhSensors.h>


#define LED_PIN 13
#define RF_CHANNEL 10
#define ADDRESS ((const uint8_t*)"1tarh")

const uint8_t address[6] = "1tarh";

RF24 radio(8,7);

payload_t payload;

void setup() { 
  Serial.begin(9600);
  Serial.println("Starting ...");
  
  pinMode(LED_PIN, OUTPUT);
  blink(LED_PIN, 1);
  
  payload.sequence = 0;
  
  // initialise le module radio. En écoute uniquement.
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(sizeof(payload_t));
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1,ADDRESS);
  radio.startListening();
  
  // some diagnostic
  Serial.print("Payload Size: ");
  Serial.println(radio.getPayloadSize());
  Serial.print("Dynamic payload Size: ");
  Serial.println(radio.getDynamicPayloadSize());
  Serial.print("PVariant: ");
  Serial.println(radio.isPVariant());
  Serial.print("PA Level: ");
  Serial.println(radio.getPALevel());
  Serial.print("Data rate: ");
  Serial.println(radio.getDataRate());
  Serial.print("Signal: ");
  Serial.println(radio.testRPD());
  Serial.print("Radio available: ");
  Serial.println(radio.available());
  //radio.printDetails();
  
  blink(LED_PIN, 1);
  Serial.println("done.");
}

void loop() {
  if(radio.available()) {
    //byte payload[32] = {0};
    //radio.read(payload, 32);
    radio.read(&payload, sizeof(payload_t));
    
   printValues(payload);
   //printBuffer((byte*)&payload, sizeof(payload_t));
  }
  
  //delay(1000);
}

void printValues(payload_t payload) {
    const char separator = ';';
   /*  Serial.print(payload.sequence); 
    Serial.print(separator);
    Serial.print(payload.tension);
    Serial.print(separator);
    Serial.print(payload.temperature1);
    Serial.print(separator);
    Serial.print(payload.humidity);
    Serial.print(separator);
    Serial.print(payload.temperature2);
    Serial.print(separator);
    Serial.print(payload.pression);
    Serial.print(separator);
    Serial.print(payload.temperature3);
    Serial.print("\t\t\t###\t\t\t");*/
    Serial.print(millis());
    Serial.print(separator);
    Serial.print(payload.sequence); 
    Serial.print(separator);
    Serial.print((float)(payload.tension + 150.0) / 100.0);
    Serial.print(separator);
    Serial.print((float)(payload.temperature) / 500.0 - 40.0);
    Serial.print(separator);
    Serial.print((float)payload.humidity / 100.0);
    Serial.print(separator);
    Serial.print((float)(payload.temperature2) / 500.0 - 40.0);
    Serial.print(separator);
    Serial.print(payload.pression + 80000);
    Serial.print(separator);
    Serial.print((float)(payload.temperature3) / 500.0 - 40.0);
    Serial.println();
}

void printValues(byte payload[]) {
  
  
  #if defined (VERBOSE)
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print(" Seq#: ");
    Serial.print(((unsigned long*)payload)[0]);
    Serial.print(" Tension: ");
    Serial.print(((float*)payload)[1]);
    Serial.print(" Temperature 1: ");
    Serial.print(((float*)payload)[2]);
    Serial.print(" Humidity: ");
    Serial.print(((float*)payload)[3]);
    Serial.print(" Temperature 2: ");
    Serial.print(((float*)payload)[4]);
    Serial.print(" Pressure: ");
    Serial.print(((float*)payload)[5]);
    Serial.print(" Temperature 3: ");
    Serial.print(((float*)payload)[6]);
    Serial.println();
  #else
    const char separator = ';';
    Serial.print(millis());
    Serial.print(separator);
    Serial.print(((unsigned long*)payload)[0]);
    Serial.print(separator);
    Serial.print(((float*)payload)[1]);
    Serial.print(separator);
    Serial.print(((float*)payload)[2]);
    Serial.print(separator);
    Serial.print(((float*)payload)[3]);
    Serial.print(separator);
    Serial.print(((float*)payload)[4]);
    Serial.print(separator);
    Serial.print(((float*)payload)[5]);
    Serial.print(separator);
    Serial.print(((float*)payload)[6]);
    Serial.println();
  #endif
}

void printBuffer(byte* buf, int len) {
  for(int i = 0; i < len; i++) {
    //if(buf[i]) {
      Serial.print((unsigned byte)buf[i]);  
      Serial.print(' ');
    //}
    //else {
     // Serial.print('_');
    //}
  }
  Serial.println();
}
