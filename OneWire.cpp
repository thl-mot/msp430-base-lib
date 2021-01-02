/*
 * OneWire.cpp
 *
 *  Created on: 24.12.2020
 *      Author: thomas
 */

#include <OneWire.h>

#define CYCLES_PER_US 1L // depends on the CPU speed
#define CYCLES_PER_MS (CYCLES_PER_US * 1000L)
//
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

void oneWire_writeBit(const uint8_t bit, bool state) {

	P2DIR |= bit;		// P1.1 as output
	P2REN &= ~bit;		// P1.1 read disable

	P2OUT &= ~bit;		// P1.1 low

	if (state) {
		DELAY_US(WRITE_1_US);
		P2OUT |= bit;		// P1.1 high
		DELAY_US(RECOVER_AFTER_WRITE_1);
	} else {
		DELAY_US(WRITE_0_US);
		P2OUT |= bit;		// P1.1 high
		DELAY_US(RECOVER_AFTER_WRITE_0);
	}

}

bool oneWire_readBit(const uint8_t bit) {
	P2DIR |= bit;		// P1.1 as output
	P2REN &= ~bit;		// P1.1 read disable

	P2OUT &= ~bit;		// P1.1 low
	DELAY_US(READ_US);
	P2DIR &= ~bit;			    // P1.1 as input
	P2REN |= bit;			    // P1.1 read enable
	P2OUT |= bit;			    // P1.1 as input with pullup
	DELAY_US(SAMPLE_AFTER_READ_US);
	uint8_t r = P2IN & bit;
	DELAY_US(RECOVER_AFTER_READ);
	return r;
}

void oneWire_reset(const uint8_t bit) {
	P2OUT &= ~bit;				// P1.1 Onewire low
	DELAY_US(RESET_US);

	P2OUT |= bit;				// P1.1 Onewire high
	P2DIR &= ~bit;			    // P1.1 as input
	P2REN |= bit;			    // P1.1 read enable
	P2OUT |= bit;			    // P1.1 as input with pullup

	DELAY_US(SAMPLE_AFTER_RESET_US);

	uint8_t r = P2IN & bit;

	DELAY_US(RECOVER_AFTER_RESET_US);

}

void oneWire_writeByte(const uint8_t bit, uint8_t b) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		oneWire_writeBit(bit, b & 1);
		b = b >> 1;
	}
}

uint8_t oneWire_readByte(const uint8_t bit) {
	uint8_t i;
	uint8_t r = 0;
	for (i = 0; i < 8; i++) {
		r = r >> 1;
		if (oneWire_readBit(bit)) {
			r |= BIT7;
		}
	}
	return r;
}



