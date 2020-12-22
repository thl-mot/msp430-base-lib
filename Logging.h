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


class Logging
{
public:
    Logging();
    virtual ~Logging();
    void begin();
    void write( char c);
    void log_i( int32_t i,uint8_t base=10);
    void log_f( float f, uint8_t decimals=2);
    void log_Hex( uint8_t i);
    void logTxt(char *TxArray, unsigned char ArrayLength);
    void CRLF();
};

extern Logging Log;

#endif /* LOGGING_H_ */
