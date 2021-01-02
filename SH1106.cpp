/*
 * SH1106.cpp
 *
 * Based on https://www.velleman.eu/downloads/29/infosheets/sh1106_datasheet.pdf
 *
 *
 *  Created on: 30.12.2020
 *      Author: thomas
 */

#include <SH1106.h>

#define MAX_INT_DIGITS 10

#define CHARGEPUMP 0x8D
#define COLUMNADDR_H 0x10	// 0b0001aaaa  7..4
#define COLUMNADDR_L 0x00       // 0b0000aaaa  3..0
#define COMSCANDEC 0xC8
#define COMSCANINC 0xC0
#define DISPLAYALLON 0xA5
#define DISPLAYALLON_RESUME 0xA4
#define DISPLAYOFF 0xAE
#define DISPLAYON 0xAF
#define EXTERNALVCC 0x1
#define INVERTDISPLAY 0xA7
#define MEMORYMODE 0x20
#define NORMALDISPLAY 0xA6
#define PAGEADDR 0xB0
#define PAGESTARTADDRESS 0xB0
#define SEGREMAP 0xA1
#define SETCOMPINS 0xDA
#define SETCONTRAST 0x81
#define SETDISPLAYCLOCKDIV 0b11010101		// 0b11010101
#define SETDISPLAYOFFSET 0xD3
#define SETHIGHCOLUMN 0x10
#define SETLOWCOLUMN 0x00
#define SETMULTIPLEX 0xA8
#define SETPRECHARGE 0xD9
#define SETSEGMENTREMAP 0xA1
#define SETSTARTLINE 0x40 		//  0b01aaaaaa
#define SETVCOMDETECT 0xDB
#define SWITCHCAPVCC 0x2

const uint8_t mapSize2[]
  { 0b00000000, // 0
      0b00000011, // 1
      0b00001100, // 2
      0b00001111, // 3
      0b00110000, // 4
      0b00110011, // 5
      0b00111100, // 6
      0b00111111, // 7
      0b11000000, // 8
      0b11000011, // 9
      0b11001100, // 10
      0b11001111, // 11
      0b11110000, // 12
      0b11110011, // 13
      0b11111100, // 14
      0b11111111, // 15
  };

const uint8_t mapSize4[]
  { 0b00000000, // 0
      0b00001111, // 1
      0b11110000, // 2
      0b11111111, // 3
  };

const uint8_t symbol_space[] =
  { 0, 0, 0, 0, 0 };

const uint8_t symbol_dot[] =
  { 0b0000000, 0b1100000, 0b1100000, 0b0000000, 0b0000000 };

const uint8_t symbol_degree[] =
  { 0b0000000, 0b0000110, 0b0001001, 0b0000110, 0b0000000 };

const uint8_t symbol_plus[] =
  { 0b0001000,
    0b0001000,
    0b0111110,
    0b0001000,
    0b0001000 };

const uint8_t symbol_minus[] =
  { 0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000 };

const uint8_t num1[]
  {
  // 0
      0b0111110, 0b1001001, 0b1000101, 0b1000011, 0b0111110,
      // 1
      0b0000000, 0b0000100, 0b0000010, 0b1111111, 0b0000000,
      // 2
      0b1000010, 0b1100001, 0b1010001, 0b1001001, 0b1000110,
      // 3
      0b0100010, 0b1000001, 0b1001001, 0b1001001, 0b0110110,
      // 4
      0b0011000, 0b0010100, 0b0010010, 0b1111111, 0b0010000,
      // 5
      0b0100111, 0b1000101, 0b1000101, 0b1000101, 0b0111001,
      // 6
      0b0111110, 0b1001001, 0b1001001, 0b1001001, 0b0110010,
      // 7
      0b0000001, 0b1110001, 0b0001001, 0b0000101, 0b0000011,
      // 8
      0b0110110, 0b1001001, 0b1001001, 0b1001001, 0b0110110,
      // 9
      0b0100110, 0b1001001, 0b1001001, 0b1001001, 0b0111110,
      // A
      0b1111110, 0b0010001, 0b0010001, 0b0010001, 0b1111110,
      // B
      0b1111111, 0b1001001, 0b1001001, 0b1001001, 0b0110110,
      // C
      0b0111110, 0b1000001, 0b1000001, 0b1000001, 0b0100010,
      // D
      0b1111111, 0b1000001, 0b1000001, 0b1000001, 0b0111110,
      // E
      0b1111111, 0b1001001, 0b1001001, 0b1000001, 0b1000001,
      // F
      0b1111111, 0b0001001, 0b0001001, 0b0000001, 0b0000001,
      // G
      0b0111110, 0b1000001, 0b1011001, 0b1001001, 0b0111010,
      // H
      0b1111111, 0b0001000, 0b0001000, 0b0001000, 0b1111111,
      //
      0b0000000, 0b1000001, 0b1111111, 0b1000001, 0b0000000,
      //
      0b0100000, 0b1000001, 0b0111111, 0b0000001, 0b0000000,
      //
      0b1111111, 0b0001000, 0b0010100, 0b0100010, 0b1000001,
      //
      0b1111111, 0b1000000, 0b1000000, 0b1000000, 0b1000000,
      //
      0b1111111, 0b0000010, 0b0000100, 0b0000010, 0b1111111,
      //
      0b1111111, 0b0001000, 0b0010000, 0b0100000, 0b1111111,
      //
      0b0111110, 0b1000001, 0b1000001, 0b1000001, 0b0111110,
      //
      0b1111111, 0b0001001, 0b0001001, 0b0001001, 0b0000110,
      // Q
      0b0111110, 0b1000001, 0b1010001, 0b1100001, 0b0111110,
      //
      0b1111111, 0b0001001, 0b0001001, 0b0001001, 0b1110110,
      //
      0b1000110, 0b1001001, 0b1001001, 0b1001001, 0b0110001,
      //
      0b0000001, 0b0000001, 0b1111111, 0b0000001, 0b0000001,
      //
      0b0111111, 0b1000000, 0b1000000, 0b1000000, 0b0111111,
      //
      0b0011111, 0b0100000, 0b1000000, 0b0100000, 0b0011111,
      //
      0b1111111, 0b0100000, 0b0010000, 0b0100000, 0b1111111,
      //
      0b1100011, 0b0010100, 0b0001000, 0b0010100, 0b1100011,
      //
      0b0000011, 0b0000100, 0b1111000, 0b0000100, 0b0000011,
      //
      0b1100001, 0b1010001, 0b1001001, 0b1000101, 0b1000011 };

void copyPattern (uint8_t *dest, uint8_t destPos, const uint8_t *src,
		  uint16_t srcPos) {
  uint8_t i;
  for (i = 0; i < 5; i++) {
    dest[destPos + i] = src[srcPos + i];
  }
  dest[destPos + 5] = 0;
}

SH1106::SH1106 () {
  // TODO Auto-generated constructor stub
}

SH1106::~SH1106 () {
  // TODO Auto-generated destructor stub
}

bool SH1106::begin () {
  sendCommand (DISPLAYOFF);
  sendCommand (NORMALDISPLAY);
  sendCommand (SETDISPLAYCLOCKDIV, 0x80);
  sendCommand (SETMULTIPLEX, 0x3F);
  sendCommand (SETDISPLAYOFFSET, 0x00);
  sendCommand (SETSTARTLINE | 0x00);
  sendCommand (CHARGEPUMP, 0x14);
  sendCommand (MEMORYMODE, 0x00);
  sendCommand (SEGREMAP);
  sendCommand (COMSCANDEC);
  sendCommand (SETCOMPINS, 0x12);
  sendCommand (SETCONTRAST, 0xCF);
  sendCommand (SETPRECHARGE, 0xF1);
  sendCommand (SETVCOMDETECT, 0x40);
  sendCommand (DISPLAYALLON_RESUME);
  sendCommand (NORMALDISPLAY, 0x2e);
  sendCommand (DISPLAYON);
  return true;
}

void SH1106::setContrast (char contrast) {
  sendCommand (0x81, contrast);
}

void SH1106::clear () {
  sendCommand (0x10 | 0x00, 0x00 | 0x00);
  sendCommand ( PAGEADDR | 0x00);

  sendCommand (SETSTARTLINE | 0x00);

  uint8_t out[133];
  uint8_t in[0];
  out[0] = 0x40;
  int i = 0;
  for (i = 1; i < 133; i++) {
    out[i] = 0;
  }

  for (i = 0; i < 8; i++) {
    sendCommand (0x10 | 0x00, 0x00 | 0x00);
    sendCommand ( PAGEADDR | i);
    Wire.transfer (0x3C, out, 133, in, 0);
  }
}

void SH1106::pos (uint8_t col, uint8_t row) {
  col = col + 2;
  sendCommand (0x10 | col >> 4, 0x00 | (col & 0xF));
  sendCommand ( PAGEADDR | row);
}

void SH1106::draw (uint8_t *buf, uint8_t len) {
  uint8_t out[133];
  uint8_t in[0];
  out[0] = 0x40;
  int i = 0;
  for (i = 0; i < len && i < 132; i++) {
    out[i + 1] = buf[i];
  }
  Wire.transfer (0x3C, out, len + 1, in, 0);
}

uint8_t SH1106::draw (uint8_t col, uint8_t row, uint8_t size, uint8_t *buf,
		      uint8_t len) {
  if (size == 4) {
    return draw4 (col, row, buf, len);
  }
  else if (size == 2) {
    return draw2 (col, row, buf, len);
  }
  else {
    return draw1 (col, row, buf, len);
  }
}

uint8_t SH1106::draw1 (uint8_t col, uint8_t row, uint8_t *buf, uint8_t len) {
  pos (col, row);
  draw (buf, len);
  return col + len;
}

uint8_t SH1106::draw2 (uint8_t col, uint8_t row, uint8_t *buf, uint8_t len) {
  pos (col, row);
  uint8_t out[133];
  uint8_t in[0];
  out[0] = 0x40;
  int i = 0;
  for (i = 0; i < len && i * 2 < 130; i++) {
    uint8_t d = mapSize2[buf[i] & 0x0F];
    out[i * 2 + 1] = d;
    out[i * 2 + 2] = d;
  }
  Wire.transfer (0x3C, out, len * 2 + 1, in, 0);

  pos (col, row + 1);
  for (i = 0; i < len && i * 2 < 130; i++) {
    uint8_t d = mapSize2[buf[i] >> 4];
    out[i * 2 + 1] = d;
    out[i * 2 + 2] = d;
  }
  Wire.transfer (0x3C, out, len * 2 + 1, in, 0);
  return col + len * 2;
}

uint8_t SH1106::draw4 (uint8_t col, uint8_t row, uint8_t *buf, uint8_t len) {
  pos (col, row);
  uint8_t out[133];
  uint8_t in[0];
  out[0] = 0x40;
  int i = 0;
  for (i = 0; i < len && i * 4 < 128; i++) {
    uint8_t d = mapSize4[buf[i] & 0x03];
    out[i * 4 + 1] = d;
    out[i * 4 + 2] = d;
    out[i * 4 + 3] = d;
    out[i * 4 + 4] = d;
  }
  Wire.transfer (0x3C, out, len * 4 + 1, in, 0);

  pos (col, row + 1);
  for (i = 0; i < len && i * 4 < 128; i++) {
    uint8_t d = mapSize4[(buf[i] >> 2) & 0x03];
    out[i * 4 + 1] = d;
    out[i * 4 + 2] = d;
    out[i * 4 + 3] = d;
    out[i * 4 + 4] = d;
  }
  Wire.transfer (0x3C, out, len * 4 + 1, in, 0);

  pos (col, row + 2);
  for (i = 0; i < len && i * 4 < 128; i++) {
    uint8_t d = mapSize4[(buf[i] >> 4) & 0x03];
    out[i * 4 + 1] = d;
    out[i * 4 + 2] = d;
    out[i * 4 + 3] = d;
    out[i * 4 + 4] = d;
  }
  Wire.transfer (0x3C, out, len * 4 + 1, in, 0);

  pos (col, row + 3);
  for (i = 0; i < len && i * 4 < 128; i++) {
    uint8_t d = mapSize4[buf[i] >> 6];
    out[i * 4 + 1] = d;
    out[i * 4 + 2] = d;
    out[i * 4 + 3] = d;
    out[i * 4 + 4] = d;
  }
  Wire.transfer (0x3C, out, len * 4 + 1, in, 0);
  return col + len * 4;
}

uint8_t intToBuffer (int32_t value, uint8_t base, uint8_t *imgBuf,
		     uint8_t bufferSize) {
  bool neg= value<0;
  if (neg) {
    value*=-1;
  }

  uint8_t buf[MAX_INT_DIGITS];
  int8_t i = 0;
  if (value != 0) {
    while (value > 0 && i < MAX_INT_DIGITS) {
      buf[i] = value % base;
      i++;
      value = value / base;
    }
  }
  else {
    i = 1;
    buf[0] = 0;
  }

  uint8_t imgPos = 0;
  if (neg) {
    copyPattern (imgBuf, imgPos, symbol_minus, 0);
    imgPos += 6;
  }

  i--;
  while (i >= 0 && imgPos<bufferSize) {
    copyPattern (imgBuf, imgPos, num1, buf[i] * 5);
    imgPos += 6;
    i--;
  }
  return imgPos;
}

uint8_t SH1106::drawInteger (uint8_t col, uint8_t row, uint8_t size,
			     int32_t value, uint8_t base) {
  uint8_t imgBuf[60];
  uint8_t imgPos = 0;
  imgPos= intToBuffer(value, base, imgBuf, 60);
  return draw (col, row, size, imgBuf, imgPos);
}

uint8_t SH1106::drawFloat (uint8_t col, uint8_t row, uint8_t size,
			   float value) {
  int32_t ivalue = (int32_t) value;

  uint8_t imgBuf[MAX_INT_DIGITS*6+6+12];
  uint8_t imgPos = 0;
  imgPos= intToBuffer(value, 10, imgBuf, MAX_INT_DIGITS*6);

  copyPattern (imgBuf, imgPos, symbol_dot, 0);
  imgPos += 6;

  uint8_t tenth =0;
  if (value<0) {
    value*=-1;
  }
  tenth = (int8_t) ((value - (int32_t) value) * 10);
  copyPattern (imgBuf, imgPos, num1, tenth * 5);
  imgPos += 6;

  return draw (col, row, size, imgBuf, imgPos);
}

uint8_t SH1106::drawString (uint8_t col, uint8_t row, uint8_t size, char *str,
			    uint8_t len) {
  uint8_t imgBuf[133];
  uint8_t i;
  for (i = 0; i < len && i * 6 < 132; i++) {
    switch (str[i]) {
      case '-':
	copyPattern (imgBuf, i * 6, symbol_minus, 0);
	break;
      case '+':
	copyPattern (imgBuf, i * 6, symbol_plus, 0);
	break;
      case 0xA1:	// °
	copyPattern (imgBuf, i * 6, symbol_degree, 0);
	break;
      case '.':
	copyPattern (imgBuf, i * 6, symbol_dot, 0);
	break;
      default:
	if (str[i] >= 'A' && str[i] <= 'Z') {
	  copyPattern (imgBuf, i * 6, num1, (str[i] - 'A' + 10) * 5);
	}
	else if (str[i] >= 'a' && str[i] <= 'z') {
	  copyPattern (imgBuf, i * 6, num1, (str[i] - 'a' + 10) * 5);
	}
	else {
	  copyPattern (imgBuf, i * 6, symbol_space, 0);
	}
    }

  }
  return draw (col, row, size, imgBuf, len * 6);
}

void SH1106::sendCommand (uint8_t cmd) {
  uint8_t out[2];
  uint8_t in[0];
  out[0] = 0x80;
  out[1] = cmd;
  Wire.transfer (0x3C, out, 2, in, 0);
}

void SH1106::sendCommand (uint8_t cmd, uint8_t value) {
  uint8_t out[4];
  uint8_t in[0];
  out[0] = 0x80;
  out[1] = cmd;
  out[2] = 0x80;
  out[3] = value;
  Wire.transfer (0x3C, out, 4, in, 0);
}

void SH1106::sendCommand (uint8_t cmd, uint8_t v1, uint8_t v2) {
  uint8_t out[6];
  uint8_t in[0];
  out[0] = 0x80;
  out[1] = cmd;
  out[2] = 0x80;
  out[3] = v1;
  out[4] = 0x80;
  out[5] = v2;
  Wire.transfer (0x3C, out, 6, in, 0);
}

