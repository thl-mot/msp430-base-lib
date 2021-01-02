/*
 * Logging.h
 *
 *  Created on: 08.12.2020
 *      Author: thomas
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <msp430g2553.h>
#include <inttypes.h>

const uint16_t BAUDSETTING_1MHZ_9600=(1000000l/9600l);
const uint16_t BAUDSETTING_8MHZ_9600 (8000000l/9600l);
const uint16_t BAUDSETTING_12MHZ_9600 (12000000l/9600l);
const uint16_t BAUDSETTING_16MHZ_9600 (16000000l/9600l);

class Logging
{
public:
    Logging();
    virtual ~Logging();
    void begin( uint16_t baudSetting);
    void write( char c);
    void log_i( int32_t i,uint8_t base=10);
    void log_f( float f, uint8_t decimals=2);
    void log_Hex( uint8_t i);
    void logTxt(char *TxArray, unsigned char ArrayLength);
    void CRLF();
};

extern Logging Log;

#endif /* LOGGING_H_ */
