/*
 * OneWire.h
 *
 *  Created on: 24.12.2020
 *      Author: thomas
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <msp430.h>
#include <inttypes.h>

void oneWire_reset(uint8_t bit);
void oneWire_writeBit(uint8_t bit, bool state);
bool oneWire_readBit(uint8_t bit);

void oneWire_writeByte(uint8_t bit, uint8_t data);
uint8_t oneWire_readByte(uint8_t bit);

#endif /* ONEWIRE_H_ */
