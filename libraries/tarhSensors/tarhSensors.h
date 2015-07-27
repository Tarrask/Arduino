#include <stdint.h>

struct payload_t {
  uint16_t sequence;      // 0..65535 tourne en boucle
  uint8_t  tension;       // 1.50V..3.84V (150..384)    (uint_8)(Vcc/10 - 150)
  int16_t temperature;		// -20°C..80°C (0..60000)     (int_16)(°C*10)
  uint16_t pressure;			// 55000..120000 Pa						(uint_16)(Ph-55000)
  uint16_t humidity;      // 0%.100% 										(uint_16)(%*100)
};

void blink(uint8_t pin);
void blink(uint8_t pin, uint8_t count);




