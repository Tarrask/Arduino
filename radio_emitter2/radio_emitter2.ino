#include <SPI.h>
#include <Wire.h>
#include <LowPower.h>
#include <RF24.h>

#include <tarhSensors.h>
#include <TPH_board.h>

#define DEBUG 0

#define LED_PIN 9
#define ONBOARD_TEMP_PIN A2
#define RF24_CE_PIN 8
#define RF24_CS_PIN 7

#define RF_CHANNEL 10

const byte address[6] = "1tarh";

RF24 radio = RF24(RF24_CE_PIN, RF24_CS_PIN);
TPH_board tph = TPH_board();

uint16_t counter = 0;
payload_t payload;

void setup() {
  #if DEBUG
    Serial.begin(9600);
    Serial.println("Starting...");
  #endif

  // initialise la led, la led sera alumé le temps du démarrage
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // initialise la radio
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(RF_CHANNEL);
  radio.setPayloadSize(sizeof(payload_t));
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0,15);
  radio.openWritingPipe(address);

  // le capteur de température onBoard
  pinMode(ONBOARD_TEMP_PIN, INPUT);
  
  // initialise les capteurs de la tph Board
  Wire.begin();
  tph.begin();
  
  // éteind la led
  digitalWrite(LED_PIN, LOW);
  #if DEBUG
    Serial.println("Starting done.");
  #endif
}

void loop() {
  #if DEBUG
    Serial.println("Retrieving values...");
  #endif

  counter++;

  // lit la tension actuelle
  uint16_t tension = readVcc();

  // lit les données de la tph board
  TPH_data data = tph.readSensors();

  // prépare les données à transmettre
  payload.sequence = counter;
  payload.tension = tension/10 - 150;
  payload.temperature = data.temperature;
  payload.pressure = data.pressure - 55000;
  payload.humidity = data.humidity;

  // transmet les données par radio
  radio.powerUp();
  if(!radio.write(&payload, sizeof(payload_t))) {
    #if DEBUG
      Serial.println("Write error!");
      blink(LED_PIN, 5);
    #endif
  }
  radio.powerDown();

  #if DEBUG
    Serial.print("Tension:     "); Serial.println(tension);
    Serial.print("Temperature: "); Serial.println(data.temperature);
    Serial.print("Pression:    "); Serial.println(data.pressure);
    Serial.print("Humidite:    "); Serial.println(data.humidity);
    Serial.print("--->"); Serial.print(payload.sequence);
    Serial.print(";");Serial.print(payload.tension);
    Serial.print(";");Serial.print(payload.temperature);
    Serial.print(";");Serial.print(payload.pressure);
    Serial.print(";");Serial.println(payload.humidity);
    Serial.println("*********************");
  #endif

  // dort 30 secondes
  #if DEBUG
    Serial.flush();
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  #else
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  #endif
}


uint16_t readVcc() {
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

  uint16_t result = ADCL; // must read ADCL first - it then locks ADCH  
  result |= ADCH<<8; // unlocks both

  return 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
}
