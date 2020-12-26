/*
 * OneWire.h
 *
 *  Created on: 24.12.2020
 *      Author: thomas
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <inttypes.h>

class OneWire {
public:
	OneWire( uint8_t *_regIN, uint8_t *_regOUT, uint8_t *_regREN, uint8_t *_regDIR, uint8_t _bit);
	virtual ~OneWire();
	void begin();
	bool reset();
	bool readBit();
	void writeBit( bool state);
	void writeByte( uint8_t data);
	uint8_t readByte();
private:
	uint8_t *regIN;
	uint8_t *regOUT;
	uint8_t *regREN;
	uint8_t *regDIR;
	uint8_t bit;
};

#endif /* ONEWIRE_H_ */
