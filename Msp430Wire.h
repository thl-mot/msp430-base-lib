/*
 * Wire.h
 *
 *  Created on: 06.12.2020
 *      Author: thomas
 */

#ifndef MSP430WIRE_H_
#define MSP430WIRE_H_

#include <msp430g2553.h>
#include <inttypes.h>

class Msp430Wire
{
public:
    Msp430Wire();
    virtual ~Msp430Wire();
    void begin();
    int transfer( uint8_t adr, uint8_t *outBuffer, uint8_t outSize, uint8_t *inBuffer, uint8_t inSize);
    int ping( uint8_t adr);
private:
    int check_ack();
    int transmit( uint8_t *buffer, uint8_t size);
    int receive( uint8_t *buffer, uint8_t size);
};

extern Msp430Wire Wire;

#endif /* MSP430WIRE_H_ */
