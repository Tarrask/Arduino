#include "SPI.h"
#include "TimerOne.h"
#include "LedDisplay.h"
#include "LedDisplayLookup.h"

// les pin configurable, cs: chipSelect, str: strobe/latch, pwm: Pulse Width Modulation
uint8_t csPin;
uint8_t strPin;
uint8_t pwmPin;
// le groupe de led actuellement allumé
uint8_t currentGroup = 0;
// le front et backBuffer de l'affichage
uint8_t *frontBuffer = (uint8_t[16]){0};
uint8_t *backBuffer = (uint8_t[16]){0};

void shift(byte b1, byte b2, byte b3, byte b4, byte b5) {
  digitalWrite(strPin, LOW);
  
  SPI.beginTransaction(SPISettings(LED_SPI_SPEED, LSBFIRST, SPI_MODE0));
		digitalWrite(csPin, HIGH);
		SPI.transfer(b5);
		SPI.transfer(b4);
		SPI.transfer(b3);
		SPI.transfer(b2);
		SPI.transfer(b1);
		digitalWrite(csPin, LOW);
  SPI.endTransaction();

  digitalWrite(strPin, HIGH);
}

void shiftCurrentGroup() {
  digitalWrite(strPin, LOW);
	digitalWrite(csPin, HIGH);
	
	uint8_t grp = currentGroup * 4;
  SPI.beginTransaction(SPISettings(LED_SPI_SPEED, LSBFIRST, SPI_MODE0));
		SPI.transfer(ledGroup[currentGroup]);
		SPI.transfer(frontBuffer[grp + 3]);
		SPI.transfer(frontBuffer[grp + 2]);
		SPI.transfer(frontBuffer[grp + 1]);
		SPI.transfer(frontBuffer[grp + 0]);
  SPI.endTransaction();
	
	digitalWrite(csPin, LOW);
  digitalWrite(strPin, HIGH);
}

void groupSwitchCallback() {
	currentGroup = (currentGroup+1) & 0x03;
	shiftCurrentGroup();
}


void led_begin(uint8_t _csPin, uint8_t _strPin, uint8_t _pwmPin) {
	// les différents pin configurable. Les autres sont définit au niveau hardware
	csPin = _csPin;
	strPin = _strPin;
	pwmPin = _pwmPin;
	
	// on initialise la communication SPI, les pin SCK, MOSI, MISO, SS sont configuré
	SPI.begin();
	
	// on indique qu'on utilise l'interrupt 1 pour autre chose
	SPI.usingInterrupt(1);

	// on désactive par défault la communication avec le circuit du panel
	pinMode(csPin, OUTPUT);
  digitalWrite(csPin, LOW);
	
	// on initialise le pin qui provoque l'affichage du registre
	pinMode(strPin, OUTPUT);
	digitalWrite(strPin, LOW);
	
	// on initialise le pin de modulation d'impulsion
	pinMode(pwmPin, OUTPUT);
	digitalWrite(pwmPin, LOW);
	
	// on vide le registre
	shift(B00000000, B00000000, B00000000, B00000000, B00000000);
	
	// on zero les buffers
	memset(backBuffer, 0, 16);
	memset(frontBuffer, 0, 16);
	
	// initialise le timer 1 pour provoquer des interrupts permettant de varier le groupe de leds allumées
	Timer1.initialize(LED_GROUP_SWITCH_TIMER);
  Timer1.attachInterrupt(groupSwitchCallback);
}

void led_cleanBackBuffer() {
	memset(backBuffer, 0, 16);
}

void led_flipBuffers() {
	noInterrupts();
		uint8_t *tmp;
		tmp	= frontBuffer;
		frontBuffer = backBuffer;
		backBuffer = tmp;
	interrupts();
}

void led_setIntensity(uint8_t val) {
	analogWrite(pwmPin, val);
}


// les valeurs
void led_setTime(int32_t timestamp) {
	// isole les heures, minutes et secondes
	uint16_t minutes = timestamp / 60;
	uint16_t seconds = timestamp - minutes*60;
	uint16_t hours   = minutes / 60;
	minutes -= hours*60;
	
	// isole chaque digit des heures, minutes et secondes
	uint8_t sec1  = seconds%10;
	uint8_t sec10 = seconds/10;
	uint8_t min1  = minutes%10;
	uint8_t min10 = minutes/10;
	uint8_t hour1 = hours%10;
	uint8_t hour10= hours/10;
	
	// "dessine" dans le backBuffer
	for(int i = 0; i < 16; i++) {
		backBuffer[i] |= pgm_read_byte_near(HOUR_1 + hour1*16 + i);
		backBuffer[i] |= pgm_read_byte_near(HOUR_10 + hour10*16 + i);
		backBuffer[i] |= pgm_read_byte_near(MINUTE_1 + min1*16 + i);
		backBuffer[i] |= pgm_read_byte_near(MINUTE_10 + min10*16 + i);
	}
}
void led_setExternalTemperature(float temperature) {
	boolean neg = false;
	if(temperature < 0) {
		neg = true;
		temperature = -temperature;
	}
	// isole chaque digit
	uint8_t entier = temperature;
	uint8_t temp01 = (temperature-entier+0.01)*10;
	uint8_t temp1  = entier%10;
	uint8_t temp10 = entier/10;

	// "dessine" dans le backBuffer
	for(int i = 0; i < 16; i++) {
		backBuffer[i] |= pgm_read_byte_near(EXT_01 + temp01*16 + i);
		backBuffer[i] |= pgm_read_byte_near(EXT_1 + temp1*16 + i);
		backBuffer[i] |= pgm_read_byte_near(EXT_10 + temp10*16 + i);
	}
	if(neg) {
		if(temp10 == 0)	backBuffer[4] |= B00100000;
		else 						backBuffer[0] |= B00100000;
	}
	
	// affiche le point
	led_setExternalDot();
}
void led_setInternalTemperature(float temperature) {
	boolean neg = false;
	if(temperature < 0) {
		neg = true;
		temperature = -temperature;
	}
	// isole chaque digit
	uint8_t entier = temperature;
	uint8_t temp01 = (temperature-entier+0.01)*10;
	uint8_t temp1  = entier%10;
	uint8_t temp10 = entier/10;

	// "dessine" dans le backBuffer
	for(int i = 0; i < 16; i++) {
		backBuffer[i] |= pgm_read_byte_near(INT_01 + temp01*16 + i);
		backBuffer[i] |= pgm_read_byte_near(INT_1 + temp1*16 + i);
		backBuffer[i] |= pgm_read_byte_near(INT_10 + temp10*16 + i);
	}
	if(neg) {
		if(temp10 == 0)	backBuffer[6] |= B00100000;
		else 						backBuffer[2] |= B00100000;
	}
	
	// affiche le point
	led_setInternalDot();
}


// les symboles
void led_setAlarm() {
	backBuffer[9] |= B00000001;
}
void led_setExternalBattery() {
	backBuffer[5] |= B00000001;
}
void led_setExternalDot() {
	backBuffer[8] |= B10000000;
}
void led_setExternalTrend(trend_t trend) {
	switch(trend) {
		case UP:
			backBuffer[4] |= B10000000;
			break;
		case DOWN:
			backBuffer[12] |= B10000000;
			break;
	}
}
void led_setInternalBattery() {
	backBuffer[0] |= B10000000;
}
void led_setInternalDot() {
	backBuffer[10] |= B10000000;
}
void led_setInternalTrend(trend_t trend) {
	switch(trend) {
		case UP:
			backBuffer[6] |= B10000000;
			break;
		case DOWN:
			backBuffer[14] |= B10000000;
			break;
	}
}
void led_setNumber(int value) {
	switch(value) {
		case 1:
			backBuffer[0] |= B00001000;
			break;
		case 2:
			backBuffer[0] |= B00000010;
			break;
		case 3:
			backBuffer[1] |= B10000000;
			break;
	}
}
void led_setRadio() {
	backBuffer[13] |= B00000001;
}
void led_setAM() {
	backBuffer[2] |= B00001000;
}
void led_setPM() {
	backBuffer[2] |= B10000000;
}
void led_setTimeDots() {
	backBuffer[1] |= B00010001;
}


// les icones météo
void led_setCloud() {
	backBuffer[6] |= B01010101;
	backBuffer[10] |= B01010101;
}
void led_setLightning() {
	backBuffer[7] |= B00000101;
}
void led_setRain() {
	backBuffer[11] |= B01010101;
	backBuffer[15] |= B01010101;
}
void led_setSun() {
	backBuffer[14] |= B01010101;
}
void led_setPressureTrend(trend_t trend) {
	switch(trend) {
		case DOWN: 
			backBuffer[7] |= B01010000;
			break;
		case EQUAL: 
			backBuffer[3] |= B00000101;
			break;
		case UP: 
			backBuffer[3] |= B01010000;
			break;
	}
}