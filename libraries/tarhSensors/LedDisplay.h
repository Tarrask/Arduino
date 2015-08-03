#ifndef LedDisplay_H
#define LedDisplay_H

#include "Arduino.h"

#define LED_SPI_SPEED 2000000
#define LED_GROUP_SWITCH_TIMER 100

enum trend_t { DOWN, UP, EQUAL };

// configuration
void led_begin(uint8_t csPin, uint8_t strPin, uint8_t pwmPin);
void led_setIntensity(uint8_t val);

// gestion des buffers
void led_cleanBackBuffer();
void led_flipBuffers();

// écriture dans le backBuffer
// les valeurs
void led_setTime(int32_t timestamp);
void led_setExternalTemperature(float temperature);
void led_setInternalTemperature(float temperature);

// les symboles
void led_setAlarm();
void led_setExternalBattery();
void led_setExternalDot();
void led_setExternalTrend(trend_t trend);
void led_setInternalBattery();
void led_setInternalDot();
void led_setInternalTrend(trend_t trend);
void led_setNumber(int value);
void led_setRadio();
void led_setAM();
void led_setPM();
void led_setTimeDots();

// les icones météo
void led_setCloud();
void led_setLightning();
void led_setRain();
void led_setSun();
void led_setPressureTrend(trend_t trend);

#endif  //LedDisplay_H