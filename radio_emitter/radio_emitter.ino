
#include <LowPower.h>

#include <SPI.h>
#include <Wire.h>
#include <RF24.h>

//#include <DHT.h>
#include <Barometer.h>
//#include <Sodaq_SHT2x.h>
//#include <Sodaq_BMP085.h>

#include <tarhSensors.h>

#define DEBUG
#define LED_PIN 9
#define DHT_PIN A0
#define ONBOARD_TEMP_PIN A2
#define DHT_TYPE DHT22 // (AM2302)

#define RF_CHANNEL 10

RF24 radio(8,7);
//DHT dht(DHT_PIN, DHT_TYPE, 3);
Barometer barometer;

//Sodaq_BMP085 bmp = Sodaq_BMP085();
//SHT2xClass sht = SHT2xClass();

const byte address[6] = "1tarh";

uint16_t counter = 0UL;
payload_t payload;

void setup() {
  // Serial communication
  Serial.begin(9600);
  Serial.println("Starting...");
  
 // Serial.println(COUNT);
  
  pinMode(LED_PIN, OUTPUT);
  blink(LED_PIN, 1);
  
  // Température et humidité
  //dht.begin();
  
  // baromètre
  barometer.init();
//  bmp.begin(BMP085_ULTRALOWPOWER);
  
  // radio module
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(sizeof(payload_t));
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0,15);
  radio.openWritingPipe(address);
  
  blink(LED_PIN, 1);
  Serial.println("done.");
}

void loop() {
  //blink(LED_PIN, 1);
    
  // met à jour les valeurs
  counter++;
  float vcc = readVcc();
  float temperature1 = readOnBoardTemp(vcc);
  float humidity = 0; //sht.GetHumidity();
  float temperature2 = 0; //sht.GetTemperature();
  float pressure = barometer.bmp085GetPressure(barometer.bmp085ReadUP());
  float temperature3 = barometer.bmp085GetTemperature(barometer.bmp085ReadUT());
  //float temperature3 = bmp.readTemperature();
  //uint16_t pressure = bmp.readPressure();
    
  payload.sequence = counter;
  payload.tension = vcc*100.0 - 150.0;
  payload.temperature1 = (uint16_t)((temperature1+40.0) * 500.0);
  payload.humidity = humidity * 100;
  payload.temperature2 = (uint16_t)((temperature2+40.0) * 500.0);
  payload.pression = pressure-80000;
  payload.temperature3 = (uint16_t)((temperature3+40.0) * 500.0);
    
  radio.powerUp();
  if(!radio.write(&payload, sizeof(payload_t))) {
    Serial.println("Write error!");
 //   blink(LED_PIN);
  }
  radio.powerDown();
  
  blink(LED_PIN, 1);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  //delay(8000);
}

float readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  long result = ADCL; // must read ADCL first - it then locks ADCH  
  result |= ADCH<<8; // unlocks both

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result / 1000.0; // Vcc in millivolts
}

float readOnBoardTemp(float Vcc) {
  float temp = ((float)analogRead(ONBOARD_TEMP_PIN) * Vcc / 1024.0) - 0.5;
  return temp * 100.0;
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

