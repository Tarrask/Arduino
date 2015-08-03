#include <SPI.h>
#include <Wire.h>
#include <TimerOne.h>

#include <LedDisplay.h>
#include <tarhSensors.h>

#define LED_CS_PIN 5
#define LED_STR_PIN 4
#define LED_PWM_PIN 6

boolean blinkToggle;
int32_t offset = 0;
boolean showAlarm = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting ...");
  
  // selon doc, lors de l'utilisation de la librarie SPI, pour que l'arduino ne se prenne pas pour un esclave
  // https://www.arduino.cc/en/Reference/SPI ( Note about Slave Select (SS) pin on AVR based boards )
  pinMode(SS, OUTPUT);

  // démarre le panneau de led
  led_begin(LED_CS_PIN, LED_STR_PIN, LED_PWM_PIN);
  led_setIntensity(64);
}

void loop() {
  ///////////////////////////////////////////////////////////////////
  // Gestion des entrées
  ///////////////////////////////////////////////////////////////////
  // communication serial
  // les octets disponible sont lus les uns après les autres, si l'octet correspond
  // à un code connu, la suite du flux est interprété en fonction, puis on recommence
  // à lire les octets suivants.
  // Pour éviter un long délai de la fonction parseInt, il est recommander de faire suivre
  // un nombre pour un caractère non-numérique, ça peut être un autre code
  while(Serial.available()) {
    int code = Serial.read();
    long stamp;
    char buffer[8];
    switch(code) {
      // mise-à-jour de l'heure
      case 't':
        stamp = Serial.parseInt();
        offset = stamp - millis()/1000;
        break;
        
      // mise-à-jour des prévisions météo
      case 'w':
        Serial.println("Updating weather");
        break;
        
      // affichage du symbole d'alarme
      case 'a':
        if(Serial.readBytes(buffer, 1)) {
          showAlarm = buffer[0] != '0';
        }
        break;
    }
  }
  
  // calcul de l'heure, si minuit est passé, on wrap l'offset.
  int32_t seconds = offset + (millis()+10)/1000;
  if(seconds >= 86400) {
    seconds -= 86400;
    offset -= 86400;
  }

  // permet de faire clignoter l'affichage
  blinkToggle = !blinkToggle;


  ///////////////////////////////////////////////////////////////////
  // "Dessin" du backBuffer et inversion des buffers
  ///////////////////////////////////////////////////////////////////
  // efface le backBuffer des leds
  led_cleanBackBuffer();
  
  // les valeurs
  if(blinkToggle || offset != 0) {
    led_setTime(seconds);
  }
  led_setExternalTemperature(-8.8);
  led_setInternalTemperature(-8.8);

  // les symboles
  if(showAlarm) led_setAlarm();
  led_setExternalBattery();
  led_setExternalTrend(UP);
  led_setExternalTrend(DOWN);
  led_setInternalBattery();
  led_setInternalTrend(UP);
  led_setInternalTrend(DOWN);
  led_setNumber(1);
  led_setNumber(2);
  led_setNumber(3);
  led_setRadio();
  led_setAM();
  led_setPM();
  if(blinkToggle) led_setTimeDots();

  // les icones météo
  led_setCloud();
  led_setLightning();
  led_setRain();
  led_setSun();
  led_setPressureTrend(DOWN);
  led_setPressureTrend(EQUAL);
  led_setPressureTrend(UP);

  // inverse le backBuffer et le frontBuffer
  led_flipBuffers();


  ///////////////////////////////////////////////////////////////////
  // on attend jusque à la seconde suivante
  delay(1000 - (offset + millis())%1000);
  Serial.println(millis());
}
