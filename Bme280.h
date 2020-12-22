/*
 * Bme280.h
 *
 *  Created on: 08.12.2020
 *      Author: thomas
 */

#ifndef BME280_H_
#define BME280_H_

#include <Msp430Wire.h>

class Bme280 {
public:
	Bme280();
	virtual ~Bme280();
	void begin();
	void measure();
	float getHumidity();
	float getTemperature();
	float getPreassure();
private:
	uint32_t t_fine;
	uint32_t h;
	uint32_t t;
	uint32_t p;

	uint16_t dig_T1;   // 88 89
	int16_t dig_T2;    // 8A 8B
	int16_t dig_T3;    // 8C 8D
	uint16_t dig_P1;   // 8E 8F
	int16_t dig_P2;    // 90 91
	int16_t dig_P3;    // 92 93
	int16_t dig_P4;    // 94 95
	int16_t dig_P5;    // 96 97
	int16_t dig_P6;    // 98 99
	int16_t dig_P7;    // 9A 9B
	int16_t dig_P8;    // 9C 9D
	int16_t dig_P9;    // 9E 9F
	uint8_t dig_H1;    // A1

    int16_t dig_H2;    // E1 E2
    int8_t dig_H3;     // D3
    int16_t dig_H4;    // E4 E5
    int16_t dig_H5;    // E5 E6
    int16_t dig_H6;    // E7

    bool measureTemperature();
    bool measureHumidity();
    bool measurePreassure();
};

#endif /* BME280_H_ */
