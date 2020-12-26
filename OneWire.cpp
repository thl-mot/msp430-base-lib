/*
 * OneWire.cpp
 *
 *  Created on: 24.12.2020
 *      Author: thomas
 */

#include <OneWire.h>

#define CYCLES_PER_US 1L // depends on the CPU speed
#define CYCLES_PER_MS (CYCLES_PER_US * 1000L)

#define DELAY_US(x) __delay_cycles((x * CYCLES_PER_US))
#define DELAY_MS(x) __delay_cycles((x * CYCLES_PER_MS))

#define RESET_US 480
#define SAMPLE_AFTER_RESET_US (550-RESET_US)
#define RECOVER_AFTER_RESET_US (960-SAMPLE_AFTER_RESET_US-RESET_US)
#define SLOT 60
#define WRITE_0_US SLOT
#define RECOVER_AFTER_WRITE_0 (10)
#define WRITE_1_US 6
#define RECOVER_AFTER_WRITE_1 (SLOT-WRITE_1_US+10)
#define READ_US 6
#define SAMPLE_AFTER_READ_US (14-READ_US)
#define RECOVER_AFTER_READ (SLOT-READ_US-SAMPLE_AFTER_READ_US+10)

OneWire::OneWire( uint8_t *_regIN, uint8_t *_regOUT, uint8_t *_regREN, uint8_t *_regDIR, uint8_t _bit) {
	regIN= _regIN;
	regOUT= _regOUT;
	regREN= _regREN;
	regDIR= _regDIR;
	bit= 1 << _bit;
}

OneWire::~OneWire() {
}


void OneWire::begin() {
}

bool OneWire::reset() {
	*regOUT &= ~bit;				// P1.1 Onewire low
	DELAY_US(RESET_US);

	*regOUT |= bit;				// P1.1 Onewire high
	*regDIR &= ~bit;			    // P1.1 as input
	*regREN |= bit;			    // P1.1 read enable
	*regOUT |= bit;			    // P1.1 as input with pullup

	DELAY_US(SAMPLE_AFTER_RESET_US);

	uint8_t r = *regIN & bit;

	DELAY_US(RECOVER_AFTER_RESET_US);
	return r;
}

bool OneWire::readBit() {
	*regDIR |= bit;		// P1.1 as output
	*regREN &= ~bit;		// P1.1 read disable

	*regOUT &= ~bit;		// P1.1 low
	DELAY_US(READ_US);
	//P2OUT |= BIT1;				// P1.1 Onewire high
	*regDIR &= ~bit;			    // P1.1 as input
	*regREN |= bit;			    // P1.1 read enable
	*regOUT |= bit;			    // P1.1 as input with pullup
	DELAY_US(SAMPLE_AFTER_READ_US);
	uint8_t r = *regIN & bit;
	DELAY_US(RECOVER_AFTER_READ);
	return r;
}

void OneWire::writeBit( bool state) {
	*regDIR |= bit;		// P1.1 as output
	*regREN &= ~bit;		// P1.1 read disable

	*regOUT &= ~bit;		// P1.1 low

	if (state) {
		DELAY_US(WRITE_1_US);
		*regOUT |= bit;		// P1.1 high
		DELAY_US(RECOVER_AFTER_WRITE_1);
	} else {
		DELAY_US(WRITE_0_US);
		*regOUT |= bit;		// P1.1 high
		DELAY_US(RECOVER_AFTER_WRITE_0);
	}
}

uint8_t OneWire::readByte() {
	uint8_t i;
	uint8_t r = 0;
	for (i = 0; i < 8; i++) {
		r = r >> 1;
		if (readBit()) {
			r |= 0x80;
		}
	}
	return r;
}

void OneWire::writeByte( uint8_t data) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		writeBit(data & 1);
		data = data >> 1;
	}
}



