#ifndef TPH_board_H
#define TPH_board_H

#include "Arduino.h"

struct TPH_data {
	int16_t temperature;
	int32_t pressure;
	int16_t humidity;
};

class TPH_board {
	public:
		TPH_board();
		void begin();
		TPH_data readSensors();
	private:
		void beginSHT21();
		void beginBMP180();
		void startHumidityReading();
		int16_t readTemperature();
		int32_t readPressure();
		int16_t readHumidity();
		
		uint8_t  checkCRC(uint8_t b1, uint8_t b2, uint8_t checksum);
		int8_t   read8(uint8_t address, uint8_t reg);
		int16_t  read16(uint8_t reg);
		uint16_t read16u(uint8_t reg);
		void     write8u(uint8_t address, uint8_t reg, uint8_t value);
		int32_t  readData(uint8_t key);
		
		int16_t  ac1, ac2, ac3;
		uint16_t ac4, ac5, ac6;
		int16_t   b1,  b2;
		int32_t   b3,  b5,  b6;
		uint32_t  b4,  b7;
		int16_t   mb,  mc,  md;
		int32_t   x1 , x2,  x3;
};

#endif // TPH_board_H