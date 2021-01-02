/*
 * SH1106.h
 *
 *  Created on: 30.12.2020
 *      Author: thomas
 */

#ifndef SH1106_H_
#define SH1106_H_

#include <Msp430Wire.h>

class SH1106 {
  public:
    SH1106 ();
    virtual ~SH1106 ();
    bool begin ();
    void clear ();
    void setContrast (char contrast);
    void pos( uint8_t col, uint8_t row);
    void draw (uint8_t *buf, uint8_t len);
    uint8_t draw( uint8_t col, uint8_t row, uint8_t size ,uint8_t *buf, uint8_t len);
    uint8_t drawInteger( uint8_t col, uint8_t row, uint8_t size, int32_t value, uint8_t base=10);
    uint8_t drawFloat (uint8_t col, uint8_t row, uint8_t size, float value);
    uint8_t drawString(uint8_t col, uint8_t row, uint8_t size,char *str, uint8_t len);
  private:
    void sendCommand (uint8_t cmd);
    void sendCommand (uint8_t cmd, uint8_t v1);
    void sendCommand (uint8_t cmd, uint8_t v1, uint8_t v2);
    uint8_t draw1( uint8_t col, uint8_t row, uint8_t *buf, uint8_t len);
    uint8_t draw2( uint8_t col, uint8_t row, uint8_t *buf, uint8_t len);
    uint8_t draw4( uint8_t col, uint8_t row, uint8_t *buf, uint8_t len);
};

#endif /* SH1106_H_ */
