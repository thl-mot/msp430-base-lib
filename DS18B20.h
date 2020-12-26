/*
 * DS18B20.h
 *
 *  Created on: 22.12.2020
 *      Author: thomas
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include <msp430.h>

class DS18B20 {
public:
	DS18B20();
	virtual ~DS18B20();
	void begin();
};

#endif /* DS18B20_H_ */
