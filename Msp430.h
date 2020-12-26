/*
 * Msp430.h
 *
 *  Created on: 22.12.2020
 *      Author: thomas
 */

#ifndef MSP430_H_
#define MSP430_H_

#include <inttypes.h>

class Msp430 {
public:
	Msp430();
	virtual ~Msp430();
	void delay_us( uint32_t us);
	void delay_ms( uint32_t ms);
};

#endif /* MSP430_H_ */
