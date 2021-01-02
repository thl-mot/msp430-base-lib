/*
 * Bme280.cpp
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
 *  Created on: 08.12.2020
 *      Author: thomas
 */

#include <Bme280.h>

Bme280::Bme280() {
}

Bme280::~Bme280() {
}

bool Bme280::begin() {

	uint8_t out[3];
	uint8_t in[30];

	// Reset
	out[0] = 0xE0;
	out[1] = 0xB6;
	int err = Wire.transfer(0x76, out, 2, in, 0);

	int r = 10;
	do {
		// read status
		out[0] = 0xF3;
		err = Wire.transfer(0x76, out, 1, in, 1);
	}
	while (r-- > 0 && (in[0] & BIT0));
	if (r==0) {
		return false;
	}

	// read trimming
	out[0] = 0x88;
	err = Wire.transfer(0x76, out, 1, in, 28);

	this->dig_T1 = (in[0] & 0xFF) | (in[1] & 0xFF) << 8;   // 88 89
	this->dig_T2 = in[2] | in[3] << 8;                   // 8A 8B
	this->dig_T3 = in[4] | in[5] << 8;                   // 8C 8D
	this->dig_P1 = (in[6] & 0xFF) | (in[7] & 0xFF) << 8;   // 8E 8F
	this->dig_P2 = in[8] | in[9] << 8;                      // 90 91
	this->dig_P3 = in[10] | in[11] << 8;                    // 92 93
	this->dig_P4 = in[12] | in[13] << 8;                    // 94 95
	this->dig_P5 = in[14] | in[15] << 8;                    // 96 97
	this->dig_P6 = in[16] | in[17] << 8;                    // 98 99
	this->dig_P7 = in[18] | in[19] << 8;                    // 9A 9B
	this->dig_P8 = in[20] | in[21] << 8;                    // 9C 9D
	this->dig_P9 = in[22] | in[23] << 8;                    // 9E 9F
	this->dig_H1 = (in[25] & 0xFF);                         // A1

	out[0] = 0xE1;
	err = Wire.transfer(0x76, out, 1, in, 7);
	this->dig_H2 = in[0] | in[1] << 8;                   // E1 E2
	this->dig_H3 = in[2];                                 // D3
	this->dig_H4 = in[3] << 4 | (in[4] & 0x0F);          // E4 E5
	this->dig_H5 = (in[4] & 0xF0) >> 4 | in[5] << 4;     // E5 E6
	this->dig_H6 = in[6];                                // E7

	// Oversampling 16
	// ctrl_hum
	out[0] = 0xF2;
	out[1] = 0b101;
	err = Wire.transfer(0x76, out, 2, in, 0);

	// Oversampling 16
	// ctrl_meas
	out[0] = 0xF4;
	out[1] = 0b10110111;
	err = Wire.transfer(0x76, out, 2, in, 0);

	return true;
}
void Bme280::measure() {
	burstReadout();
//	measureTemperature();
//	measureHumidity();
//	measurePreassure();
}
float Bme280::getHumidity() {
	return (float) h;
}
float Bme280::getTemperature() {
	return (float) t / 100;
}
float Bme280::getPreassure() {
	return (float) p / 100;
}

bool Bme280::burstReadout() {
	uint8_t out[1];
	uint8_t in[8];

	out[0] = 0xF7;
	int err = Wire.transfer(0x76, out, 1, in, 8);

	if (err == 0) {
		calculatePreassure(in[0], in[1], in[2]);
		calculateTemperature(in[3], in[4], in[5]);
		calculateHumidity(in[6], in[7]);
		return true;
	} else {
		return false;
	}
}

bool Bme280::calculateTemperature(uint8_t msb, uint8_t lsb, uint8_t xlsb) {
	volatile uint32_t adc_T = ((uint32_t) msb << 12) | (lsb << 4) | (xlsb >> 4);

	volatile int32_t var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1)))
			* ((int32_t) dig_T2)) >> 11;

	volatile int32_t var2 = (((((adc_T >> 4) - ((int32_t) dig_T1))
			* ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3))
			>> 14;

	t_fine = var1 + var2;

	this->t = ((t_fine) * 5 + 128 >> 8);
	return true;
}

bool Bme280::calculateHumidity(uint8_t msb, uint8_t lsb) {
	uint32_t adc_H = (msb << 8) | lsb;
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t) 76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t) dig_H4) << 20)
			- (((int32_t) dig_H5) * v_x1_u32r)) + ((int32_t) 16384)) >> 15)
			* (((((((v_x1_u32r * ((int32_t) dig_H6)) >> 10)
					* (((v_x1_u32r * ((int32_t) dig_H3)) >> 11)
							+ ((int32_t) 32768))) >> 10) + ((int32_t) 2097152))
					* ((int32_t) dig_H2) + 8192) >> 14));

	v_x1_u32r = (v_x1_u32r
			- (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
					* ((int32_t) dig_H1)) >> 4));

	v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	float rh = (v_x1_u32r >> 12);
	this->h = rh / 1024;
	return true;
}

bool Bme280::calculatePreassure(uint8_t msb, uint8_t lsb, uint8_t xlsb) {
	uint32_t adc_P = ((uint32_t) msb << 12) | (lsb << 4) | (xlsb >> 4);
	volatile int32_t var4, var5;
	volatile uint32_t p;
	var4 = (((int32_t) t_fine) >> 1) - (int32_t) 0xFA00;
	var5 = (((var4 >> 2) * (var4 >> 2)) >> 11) * ((int32_t) dig_P6);
	var5 = var5 + ((var4 * ((int32_t) dig_P5)) << 1);
	var5 = (var5 >> 2) + (((int32_t) dig_P4) << 16);
	var4 = (((dig_P3 * (((var4 >> 2) * (var4 >> 2)) >> 13)) >> 3)
			+ ((((int32_t) dig_P2) * var4) >> 1)) >> 18;
	var4 = ((((0x8000 + var4)) * ((int32_t) dig_P1)) >> 15);
	if (var4 == 0) {
		return false; // Avoid exception caused by division by zero
	}
	uint32_t p_local = (((uint32_t) (((int32_t) 0x100000) - adc_P)
			- (var5 >> 12))) * 0xC35;
	if (p_local < 0x80000000) {
		p_local = (p_local << 1) / ((uint32_t) var4);
	} else {
		p_local = (p_local / (uint32_t) var4) * 2;
	}
	var4 = (((int32_t) dig_P9)
			* ((int32_t) (((p_local >> 3) * (p_local >> 3)) >> 13))) >> 12;
	var5 = (((int32_t) (p_local >> 2)) * ((int32_t) dig_P8)) >> 13;
	this->p = (uint32_t) ((int32_t) p_local + ((var4 + var5 + dig_P7) >> 4));
	return true;
}

bool Bme280::measureTemperature() {
	uint8_t out[1];
	uint8_t in[3];

	out[0] = 0xFA;
	int err = Wire.transfer(0x76, out, 1, in, 3);
	if (err == 0) {
		uint32_t adc_T = ((uint32_t) in[0] << 12) | (in[1] << 4) | (in[2] >> 4);

		int32_t var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1)))
				* ((int32_t) dig_T2)) >> 11;

		int32_t var2 = (((((adc_T >> 4) - ((int32_t) dig_T1))
				* ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12)
				* ((int32_t) dig_T3)) >> 14;

		t_fine = var1 + var2;

		this->t = ((t_fine) * 5 + 128 >> 8);
		return true;
	} else {
		return false;
	}
}

bool Bme280::measureHumidity() {
	uint8_t out[1];
	uint8_t in[2];

	out[0] = 0xFD;
	int err = Wire.transfer(0x76, out, 1, in, 2);
	if (err == 0) {
		uint32_t adc_H = (in[0] << 8) | in[1];
		int32_t v_x1_u32r;
		v_x1_u32r = (t_fine - ((int32_t) 76800));
		v_x1_u32r = (((((adc_H << 14) - (((int32_t) dig_H4) << 20)
				- (((int32_t) dig_H5) * v_x1_u32r)) + ((int32_t) 16384)) >> 15)
				* (((((((v_x1_u32r * ((int32_t) dig_H6)) >> 10)
						* (((v_x1_u32r * ((int32_t) dig_H3)) >> 11)
								+ ((int32_t) 32768))) >> 10)
						+ ((int32_t) 2097152)) * ((int32_t) dig_H2) + 8192)
						>> 14));

		v_x1_u32r = (v_x1_u32r
				- (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
						* ((int32_t) dig_H1)) >> 4));

		v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
		v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
		float rh = (v_x1_u32r >> 12);
		this->h = rh / 1024;
		return true;
	} else {
		return false;
	}

}
bool Bme280::measurePreassure() {
	uint8_t out[1];
	uint8_t in[2];

	out[0] = 0xF7;
	int err = Wire.transfer(0x76, out, 1, in, 2);
	if (err == 0) {
		uint32_t adc_P = ((uint32_t) in[0] << 12) | (in[1] << 4) | (in[2] >> 4);
		volatile int32_t var4, var5;
		volatile uint32_t p;
		var4 = (((int32_t) t_fine) >> 1) - (int32_t) 0xFA00;
		var5 = (((var4 >> 2) * (var4 >> 2)) >> 11) * ((int32_t) dig_P6);
		var5 = var5 + ((var4 * ((int32_t) dig_P5)) << 1);
		var5 = (var5 >> 2) + (((int32_t) dig_P4) << 16);
		var4 = (((dig_P3 * (((var4 >> 2) * (var4 >> 2)) >> 13)) >> 3)
				+ ((((int32_t) dig_P2) * var4) >> 1)) >> 18;
		var4 = ((((0x8000 + var4)) * ((int32_t) dig_P1)) >> 15);
		if (var4 == 0) {
			return false; // Avoid exception caused by division by zero
		}
		uint32_t p_local = (((uint32_t) (((int32_t) 0x100000) - adc_P)
				- (var5 >> 12))) * 0xC35;
		if (p_local < 0x80000000) {
			p_local = (p_local << 1) / ((uint32_t) var4);
		} else {
			p_local = (p_local / (uint32_t) var4) * 2;
		}
		var4 = (((int32_t) dig_P9)
				* ((int32_t) (((p_local >> 3) * (p_local >> 3)) >> 13))) >> 12;
		var5 = (((int32_t) (p_local >> 2)) * ((int32_t) dig_P8)) >> 13;
		this->p =
				(uint32_t) ((int32_t) p_local + ((var4 + var5 + dig_P7) >> 4));
		return true;
	} else {
		return false;
	}

}

