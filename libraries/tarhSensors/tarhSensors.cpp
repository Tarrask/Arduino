#include <arduino.h>
#include "tarhSensors.h"

#define BLINK_HIGH 10
#define BLINK_LOW 50

void blink(uint8_t pin) {
  blink(pin, 3);  
}

void blink(uint8_t pin, uint8_t count) {
	digitalWrite(pin, HIGH);
  delay(BLINK_HIGH);
  digitalWrite(pin, LOW);
  for(uint8_t i = 1; i < count; i++) {
		delay(BLINK_LOW);
    digitalWrite(pin, HIGH);
    delay(BLINK_HIGH);
    digitalWrite(pin, LOW);
  }
}
