#include <Wire.h>

#include <DHT.h>
#include <Barometer.h>

#define DHT_PIN A0
#define AIR_PIN A1
#define DHT_TYPE DHT22 // (AM2302)

#define SAMPLING_INTERVAL 60000

DHT dht(DHT_PIN, DHT_TYPE);
Barometer barometer;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  
  // Température et humidité
  dht.begin();  
  
  // Qualité de l'air
  pinMode(AIR_PIN, INPUT);
  
  // baromètre
  barometer.init();
}

void loop() {
  long startTime = millis();
  
  // on récupère toutes les valeurs
  float humidity = dht.readHumidity();
  float temperature1 = dht.readTemperature();
  int   airPollution = analogRead(AIR_PIN);
  float pressure = barometer.bmp085GetPressure(barometer.bmp085ReadUP());
  float temperature2 = barometer.bmp085GetTemperature(barometer.bmp085ReadUT());

  // on écrit le tout
  Serial.print("timestamp: ");
  Serial.print(startTime);
  Serial.print("   humidity: ");
  Serial.print(humidity);
  Serial.print("   temperature 1: ");
  Serial.print(temperature1);
  Serial.print("   temperature 2: ");
  Serial.print(temperature2);
  Serial.print("   air pollution: ");
  Serial.print(airPollution);
  Serial.print("   pressure: ");
  Serial.print(pressure);
  Serial.println();
  
  // on attend x secondes pour la suite
  delay(SAMPLING_INTERVAL - (millis() - startTime));
}
