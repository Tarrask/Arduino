#include <avr/pgmspace.h>
#include <TimerOne.h>
#include <SPI.h>

#define DEBUG 1
#define NOP __asm__ __volatile__("nop\n\t")

#define DATA_PIN 11
#define CLOCK_PIN 13
#define STROBE_PIN 4
#define PWM_PIN 6

#define byteAt(array, i, j) pgm_read_byte_near(array + i*4 + j)
#define arrayByteAt(array, x, i, j) pgm_read_byte_near(array + x*16 + i*4 + j)

// les 4 groupes de led. Permet de les allumer les unes après les autres.
const byte group[] = {B10000000, B01000000, B00100000, B00010000};

const PROGMEM byte pressureUp[] = {
  B00000000, B00000000, B00000000, B01010000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte pressureEqal[] = {
  B00000000, B00000000, B00000000, B00000101,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte pressureDown[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B01010000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte sun[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B01010101, B00000000
};
const PROGMEM byte rain[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B01010101,
  B00000000, B00000000, B00000000, B01010101
};
const PROGMEM byte clouds[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B01010101, B00000000,
  B00000000, B00000000, B01010101, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte lightning[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000101,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};

// les unités des heures
const PROGMEM byte h1[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B01010000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00010100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B01010001, B01000100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B00000100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00010100, B00010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B01000101, B00010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B01000101, B01010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B01010100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B01010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B00010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// les dixaines des heures
const PROGMEM byte h10[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00010100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B01010001, B01000100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000  
};
// les unités des minutes
const PROGMEM byte m1[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B01010000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00010100, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010001, B01000100, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B00000100, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00010100, B00010100, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01000101, B00010100, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01000101, B01010100, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010100, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B01010100, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B00010100, B00000000, B00000000
};
// les dixaines des minutes
const PROGMEM byte m10[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B01010000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00010100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010001, B01000100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B00000100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00010100, B00010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01000101, B00010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01000101, B01010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B01010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B01010101, B00010100, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// les 100aine de degres intérieur
const PROGMEM byte int100[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00101000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// les 10aine de degrés intérieur
const PROGMEM byte int10[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00001010, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00101000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100010, B10001010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100000, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B00101000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B10100010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101010, B10100010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00101010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101010, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// les unités de degrés intérieur
const PROGMEM byte int1[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00001010, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00101000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100010, B10001010,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100000, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B00101000,
  B00000000, B00000000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B10100010,
  B00000000, B00000000, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101010, B10100010,
  B00000000, B00000000, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00101010,
  B00000000, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101010, B10101010,
  B00000000, B00000000, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B10101010,
  B00000000, B00000000, B00000000, B00000000
};
// les 10èmes de degrés intérieur
const PROGMEM byte int01[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00001010, B10101010,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00101000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100010, B10001010,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100000, B10101010,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B00101000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B10100010,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101010, B10100010,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00101010,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101010, B10101010,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00101000, B10101010  
};
const PROGMEM byte intNeg100[] = {
  B00000000, B00000000, B00100000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte intNeg10[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00100000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte ext100[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00101000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte ext10[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00001010, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00101000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00100010, B10001010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00100000, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B00101000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B10100010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00101010, B10100010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00101010, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
const PROGMEM byte ext1[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00001010, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00101000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00100010, B10001010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00100000, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B00101000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B10100010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101010, B10100010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101010, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B10101010, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};

const PROGMEM byte ext01[] = {
  // 0
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00001010, B10101010, B00000000, B00000000,
  // 1
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00101000, B00000000, B00000000,
  // 2
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00100010, B10001010, B00000000, B00000000,
  // 3
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00100000, B10101010, B00000000, B00000000,
  // 4
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B00101000, B00000000, B00000000,
  // 5
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B10100010, B00000000, B00000000,
  // 6
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101010, B10100010, B00000000, B00000000,
  // 7
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00101010, B00000000, B00000000,
  // 8
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101010, B10101010, B00000000, B00000000,
  // 9
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00101000, B10101010, B00000000, B00000000
};
const PROGMEM byte extNeg100[] = {
  B00100000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// éteind tout
const PROGMEM byte off[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// les 2 points de l'heure
const PROGMEM byte dot[] = {
  B00000000, B00010001, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole de la batterie de l'heure
const PROGMEM byte batInt[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000001, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole am
const PROGMEM byte am[] = {
  B00000000, B00000000, B00001000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole pm
const PROGMEM byte pm[] = {
  B00000000, B00000000, B10000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole alarme
const PROGMEM byte alarm[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000001, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole radio
const PROGMEM byte radio[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000001, B00000000, B00000000
};
// le symbole température intérieure en hausse 
const PROGMEM byte intDot[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B10000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole température intérieure en hausse 
const PROGMEM byte intUp[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B10000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole température intérieure en baisse 
const PROGMEM byte intDown[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B10000000, B00000000
};
// les 3 numéros de channel
const PROGMEM byte channel[] = {
  // 1
  B00001000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 2
  B00000010, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  // 3
  B00000000, B10000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole de batterie extérieure
const PROGMEM byte batExt[] = {
  B10000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole température extérieure en hausse
const PROGMEM byte extDot[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B10000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole température extérieure en hausse
const PROGMEM byte extUp[] = {
  B00000000, B00000000, B00000000, B00000000,
  B10000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000
};
// le symbole température extérieure en baisse
const PROGMEM byte extDown[] = {
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000,
  B10000000, B00000000, B00000000, B00000000
};
volatile byte current[4][4];
volatile byte currentGroup = 0;
boolean toggle = true;
int32_t offset = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("Starting ...");

  pinMode(7, OUTPUT);

  unsigned long start = micros();
  
  __asm__ __volatile__("nop\n\t");
  __asm__ __volatile__("nop\n\t");
  __asm__ __volatile__("nop\n\t");
  __asm__ __volatile__("nop\n\t");
  Serial.println(micros() - start);
  start = micros();
  Serial.println(micros() - start);
  

  pinMode(10, OUTPUT);
  SPI.usingInterrupt(1);
  setupShiftRegistry(DATA_PIN, CLOCK_PIN, STROBE_PIN, PWM_PIN);
  analogWrite(PWM_PIN, 128);

  Serial.println("Done.");
  Timer1.initialize(5000);
  //Timer1.pwm(9, 512);
  Timer1.attachInterrupt(callback);
}

void callback() {
    currentGroup = (currentGroup+1) & 0x03;
    shift(current[currentGroup][0], current[currentGroup][1], current[currentGroup][2], current[currentGroup][3], group[currentGroup]);
}

void loop() {
  if(Serial.available()) {
    Serial.print("Updating time, offset was: ");
    Serial.print(offset);
    long stamp = Serial.parseInt();
    offset = stamp - millis()/1000;
    Serial.print(", new stamp: ");
    Serial.print(stamp);
    Serial.print(", new offset: ");
    Serial.println(offset);
  }
  
  toggle = !toggle;
  int32_t seconds = offset + (millis()+10)/1000;
  if(seconds > 86400) {
    seconds -= 86400;
    offset -= 86400;
  }
  int32_t minutes = seconds / 60;
  seconds -= minutes * 60;
  int32_t hours = minutes / 60;
  minutes -= hours*60;
  byte sec1 = seconds%10;
  byte sec10 = seconds/10;
  byte hour1 = hours%10;
  byte hour10 = hours/10;
  byte min1 = minutes%10;
  byte min10 = minutes/10;

  Serial.print("current time: (");
  Serial.print(millis());
  Serial.print(") ");
  Serial.print(hour10);
  Serial.print(hour1);
  Serial.print(":");
  Serial.print(min10);
  Serial.print(min1);
  Serial.print(":");
  Serial.print(sec10);
  Serial.println(sec1);
  
  cli();
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      current[i][j] = arrayByteAt(h1, hour1, i, j) | arrayByteAt(h10, hour10, i, j);
      current[i][j] |= arrayByteAt(m1, min1, i, j) | arrayByteAt(m10, min10, i, j);
      current[i][j] |= arrayByteAt(int1, sec1, i, j) | arrayByteAt(int10, sec10, i, j);
      if(toggle) {
        current[i][j] |= byteAt(dot, i, j);
        
        /*current[i][j] |= byteAt(batInt, i, j);
        current[i][j] |= byteAt(pm, i, j);
        current[i][j] |= byteAt(am, i, j);
        current[i][j] |= byteAt(alarm, i, j);
        current[i][j] |= byteAt(radio, i, j);
        current[i][j] |= byteAt(intDot, i, j);
        current[i][j] |= byteAt(intUp, i, j);
        current[i][j] |= byteAt(intDown, i, j);
        current[i][j] |= arrayByteAt(channel, 0, i, j);
        current[i][j] |= arrayByteAt(channel, 1, i, j);
        current[i][j] |= arrayByteAt(channel, 2, i, j);
        current[i][j] |= byteAt(batExt, i, j);
        current[i][j] |= byteAt(extDot, i, j);
        current[i][j] |= byteAt(extUp, i, j);
        current[i][j] |= byteAt(extDown, i, j);*/
      }
      else {
      /*  current[i][j] |= byteAt(pressureUp, i, j);
        current[i][j] |= byteAt(pressureEqal, i, j);
        current[i][j] |= byteAt(pressureDown, i, j);
        current[i][j] |= byteAt(sun, i, j);
        current[i][j] |= byteAt(clouds, i, j);
        current[i][j] |= byteAt(rain, i, j);
        current[i][j] |= byteAt(lightning, i, j);
        
        current[i][j] |= arrayByteAt(int100, 1, i, j);
        current[i][j] |= arrayByteAt(int10, 8, i, j);
        current[i][j] |= arrayByteAt(int1, 8, i, j);
        current[i][j] |= arrayByteAt(int01, 8, i, j);
        current[i][j] |= byteAt(intNeg100, i, j); 
        
        current[i][j] |= arrayByteAt(ext100, 1, i, j);
        current[i][j] |= arrayByteAt(ext10, 8, i, j);
        current[i][j] |= arrayByteAt(ext1, 8, i, j);
        current[i][j] |= arrayByteAt(ext01, 8, i, j);
        current[i][j] |= byteAt(extNeg100, i, j);*/
      }
    }
  }
  sei();

  unsigned long c = offset + millis();
  c = c % 1000;
  delay(1000 - c);

  
/*  for(int i = 0; i < 4; i++) {
    shift(current[i][0], current[i][1], current[i][2], current[i][3], group[i]);
    digitalWrite(7, HIGH);
   // delayMicroseconds(1000000);
    delay(1000);
    Serial.println(".");
    digitalWrite(7, LOW);
  } */
}


void setupShiftRegistry(uint8_t data, uint8_t clk, uint8_t strobe, uint8_t pwm) {
  pinMode(strobe, OUTPUT);
  digitalWrite(strobe, LOW);
  pinMode(data, OUTPUT);
  digitalWrite(data, LOW);
  pinMode(clk, OUTPUT);
  digitalWrite(clk, LOW);
  pinMode(pwm, OUTPUT);
  digitalWrite(pwm, LOW);

  shift(B00000000, B00000000, B00000000, B00000000, B00000000);
}


void shift(byte b1, byte b2, byte b3, byte b4, byte b5) {
  digitalWrite(STROBE_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
  
 /* 
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, b5);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, b4);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, b3);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, b2);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, b1);
  digitalWrite(DATA_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);
 */
  SPI.beginTransaction(SPISettings(5000000, LSBFIRST, SPI_MODE0));
  SPI.transfer(b5);
  SPI.transfer(b4);
  SPI.transfer(b3);
  SPI.transfer(b2);
  SPI.transfer(b1);
  SPI.endTransaction();

  digitalWrite(STROBE_PIN, HIGH);
}
