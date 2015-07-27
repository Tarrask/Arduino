
#include "Wire.h"
#include "TPH_board.h"

#define SHT21_ADDRESS        B01000000
#define SHT21_READ_T_HOLD    B11100011
#define SHT21_READ_RH_HOLD   B11100101
#define SHT21_READ_T_NOHOLD  B11110011
#define SHT21_READ_RH_NOHOLD B11110101
#define SHT21_READ_REG       B11100111
#define SHT21_WRITE_REG      B11100110
#define SHT21_SOFT_RESET     B11111110

#define BMP180_ADDRESS       B01110111
#define BMP180_CMD_REG       0xF4
#define BMP180_CALC_T        0x2E   //  4.5ms
#define BMP180_CALC_P_OSS0   0x34   //  4.5ms
#define BMP180_CALC_P_OSS1   0x74   //  7.5ms
#define BMP180_CALC_P_OSS2   0xB4   // 13.5ms
#define BMP180_CALC_P_OSS3   0xF4   // 25.5ms

#define BMP180_READ_DATA_REG 0xF6
#define BMP180_SCO_MASK      B00100000

#define BMP180_READ_AC1      0xAA
#define BMP180_READ_AC2      0xAC
#define BMP180_READ_AC3      0xAE
#define BMP180_READ_AC4      0xB0
#define BMP180_READ_AC5      0xB2
#define BMP180_READ_AC6      0xB4
#define BMP180_READ_B1       0xB6
#define BMP180_READ_B2       0xB8
#define BMP180_READ_MB       0xBA
#define BMP180_READ_MC       0xBC
#define BMP180_READ_MD       0xBE


TPH_board::TPH_board() {

}

void TPH_board::begin() {
	beginSHT21();
	beginBMP180();
}

TPH_data TPH_board::readSensors() {
	TPH_data data = TPH_data();

	// lance la lecture de l'humidité du sht21 (resolution 10bit duré max 9ms)
	startHumidityReading();
	
	// récupère la température du bmp180 (plus rapide et plus précise que celle du sht21)
	data.temperature = readTemperature();
	
	// récupère la pression
	data.pressure = readPressure();
	
	// récupère l'humidité qui devrait être disponible
	data.humidity = readHumidity();
	
	return data;
}

void TPH_board::beginSHT21() {
	// selon datasheet, pour que le capteur ai le temps de démarrer.
	delay(15);
	
	// récupère le registre utilisateur
	uint8_t reg = read8(SHT21_ADDRESS, SHT21_READ_REG);
	bitSet(reg, 7);
	bitSet(reg, 0);
	write8u(SHT21_ADDRESS, SHT21_WRITE_REG, reg);
}

void TPH_board::beginBMP180() {
	// récupère les valeurs d'étalonage du capteur BMP180
  ac1 = read16(BMP180_READ_AC1);
  ac2 = read16(BMP180_READ_AC2);
  ac3 = read16(BMP180_READ_AC3);
  
  ac4 = read16u(BMP180_READ_AC4);
  ac5 = read16u(BMP180_READ_AC5);
  ac6 = read16u(BMP180_READ_AC6);
  
  b1 = read16(BMP180_READ_B1);
  b2 = read16(BMP180_READ_B2);
  
  mb = read16(BMP180_READ_MB);
  mc = read16(BMP180_READ_MC);
  md = read16(BMP180_READ_MD);
}

void TPH_board::startHumidityReading() {
	Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(SHT21_READ_RH_NOHOLD);
  Wire.endTransmission();
	delayMicroseconds(20);
}

int16_t TPH_board::readTemperature() {
	int32_t ut = readData(BMP180_CALC_T);
	
	// calcule la température réel
	x1 = ((ut - ac6) * ac5)>>15;
  x2 = ((int32_t)mc<<11) / (x1 + md);
  b5 = x1 + x2;
  return (b5 + 8)>>4;
}

int32_t TPH_board::readPressure() {
	int32_t up = readData(BMP180_CALC_P_OSS0);
	
	// calcule la pression réelle
	b6 = b5 - 4000;
  x1 = (b2 * ((b6*b6)>>12))>>11;
  x2 = ac2 * b6>>11;
  x3 = x1 + x2;
  b3 = ((((int32_t)ac1 * 4 + x3) << 0) + 2) / 4;
  x1 = ((int32_t)ac3 * b6)>>13;
  x2 = (b1 * ((b6*b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (uint32_t)(x3 + 32768))>>15;
  b7 = ((uint32_t)up - b3) * (50000>>0);
  int32_t p;
  if(b7 < 0x80000000) {
    p = (b7 * 2) / b4;
  }
  else {
    p = (b7 / b4) * 2;
  }
  x1 = (p>>8)*(p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  return p + ((x1 + x2 + (int32_t)3791)>>4);
}

int16_t TPH_board::readHumidity() {
	// 
	while(Wire.requestFrom(SHT21_ADDRESS, 3) < 3) {
		delayMicroseconds(100);
	}
	
	uint8_t msb = Wire.read();
	uint8_t lsb = Wire.read();
	uint8_t crc = Wire.read();
	
	if(checkCRC(msb, lsb, crc)) {
		uint16_t humidityRaw = ((msb<<8) + lsb) & ~0x0003;
		return (uint16_t)(0.19073486328125 * humidityRaw) - 600;
	}
	else {
		return 0;
	}
}

uint8_t TPH_board::checkCRC(uint8_t b1, uint8_t b2, uint8_t checksum) {
  uint8_t crc = 0;
  uint8_t byteCtr;
  //calculates 8-Bit checksum with given polynomial
  crc ^= b1;
  for (uint8_t bit = 8; bit > 0; --bit) { 
    if(crc & 0x80) crc = (crc << 1) ^ 0x131;
    else crc = (crc << 1);
  }
  crc ^= b2;
  for (uint8_t bit = 8; bit > 0; --bit) { 
    if(crc & 0x80) crc = (crc << 1) ^ 0x131;
    else crc = (crc << 1);
  }
  
  return crc == checksum;
}

int8_t TPH_board::read8(uint8_t address, uint8_t reg) {
	Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(address, (uint8_t)1);
  return Wire.read();
}


int16_t TPH_board::read16(uint8_t reg) {
	Wire.beginTransmission(BMP180_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(BMP180_ADDRESS, 2);
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  return (msb<<8) | lsb;
}

uint16_t TPH_board::read16u(uint8_t reg) {
	Wire.beginTransmission(BMP180_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(BMP180_ADDRESS, 2);
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  return (msb<<8) | lsb;
}

void TPH_board::write8u(uint8_t address, uint8_t reg, uint8_t value) {
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

int32_t TPH_board::readData(uint8_t key) {
	// ordonne la mesure d'une donnée
	write8u(BMP180_ADDRESS, BMP180_CMD_REG, key);
  // Wire.beginTransmission(BMP180_ADDRESS);
  // Wire.write(BMP180_CMD_REG);
  // Wire.write(key);
  // Wire.endTransmission();

  // attend que le capteur ai effecté la mesure
  uint8_t reg = BMP180_SCO_MASK;
  while(reg & BMP180_SCO_MASK) {
    delayMicroseconds(100);
    reg = read8(BMP180_ADDRESS, BMP180_CMD_REG);
  }

  // lit et retourne la valeur
  return read16u(BMP180_READ_DATA_REG);
}