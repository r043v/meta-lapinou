/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _GAMEBUINO_META_GRAPHICS_ST7735_H_
#define _GAMEBUINO_META_GRAPHICS_ST7735_H_

#include <Arduino.h>

#define spiWait while( !( REG_SERCOM4_SPI_INTFLAG & 0x1 ) )
#define spiSend8(b) REG_SERCOM4_SPI_DATA=(b);spiWait;
#define spiSend16(a,b) spiSend8(a)spiSend8(b)
#define spiSend16x8(a,b) spiSend16(a,b)spiSend16(a,b)spiSend16(a,b)spiSend16(a,b)spiSend16(a,b)spiSend16(a,b)spiSend16(a,b)spiSend16(a,b)
#define spiSend16x32(a,b) spiSend16x8(a,b)spiSend16x8(a,b)spiSend16x8(a,b)spiSend16x8(a,b)
#define spiSend16x128(a,b) spiSend16x32(a,b)spiSend16x32(a,b)spiSend16x32(a,b)spiSend16x32(a,b)


#define tftCsPinMask ( 1 << 22 )
#define tftDcPinMask ( 1 << 23 )
#define tftCsDcPinMask (tftCsPinMask|tftDcPinMask)
#define tftPort &(PORT->Group[1].OUT.reg);

//#include "../../config/config.h"

//#include "../Image.h"

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define INITR_18GREENTAB    INITR_GREENTAB
#define INITR_18REDTAB      INITR_REDTAB
#define INITR_18BLACKTAB    INITR_BLACKTAB
#define INITR_144GREENTAB   0x1

#define ST7735_TFTWIDTH  128
// for 1.44" display
#define ST7735_TFTHEIGHT_144 128
// for 1.8" display
#define ST7735_TFTHEIGHT_18  160

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_RGB	0x00
#define MADCTL_MH		0x04
#define MADCTL_BGR	0x08
#define MADCTL_ML		0x10
#define MADCTL_MV		0x20
#define MADCTL_MX		0x40
#define MADCTL_MY		0x80

#define ST7735_ROTATE_LEFT ( MADCTL_MX | MADCTL_MY | MADCTL_RGB )
#define ST7735_ROTATE_UP ( MADCTL_MV | MADCTL_MY | MADCTL_RGB )
#define ST7735_ROTATE_RIGHT MADCTL_RGB
#define ST7735_ROTATE_DOWN ( MADCTL_MX | MADCTL_MV | MADCTL_RGB )

class Display_ST7735 {
public:
	//using Graphics::drawImage;
	Display_ST7735(int8_t CS, int8_t RS);

	void init();
	void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
//	void pushColor(uint16_t c);
//	void _drawPixel(int16_t x, int16_t y);
//	void drawFastVLine(int16_t x, int16_t y, int16_t h);
//	void drawFastHLine(int16_t x, int16_t y, int16_t w);
//	void drawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, Image& img);
	void drawBuffer(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, uint16_t h);
	void sendBuffer(uint8_t *buffer, uint16_t n);
	uint32_t clrScr( uint16_t *buffer, uint32_t l );
	void hello();
	void bye();
	void startLine( uint8_t L );
	void fullScreenSpi();
	void dataMode();
	void commandMode();
	void idleMode();
//	void drawImage(int16_t x, int16_t y, Image& img);
//	void drawImage(int16_t x, int16_t y, Image& img, int16_t w2, int16_t h2);
	void fillRect(uint16_t color, int16_t x, int16_t y, int16_t xx, int16_t yy);
	void setRotation(uint8_t r);
	void invertDisplay(bool i);
	void setPower(bool i);
//	ColorMode colorMode = ColorMode::rgb565;

//	Rotation getRotation();
private:
//	Rotation rotation;

	void spiwrite(uint8_t);
//	void writecommand(uint8_t c);
//	void writedata(uint8_t d);
	void commandList(const uint8_t *addr);
	void commonInit();

	volatile uint32_t  *dataport, *clkport, *csport, *rsport;
	uint32_t  datapinmask, clkpinmask, cspinmask, rspinmask;

};

/*using Gamebuino_Meta::Rotation;

const Rotation ROTATION_LEFT = Rotation::left;
const Rotation ROTATION_UP = Rotation::up;
const Rotation ROTATION_RIGHT = Rotation::right;
const Rotation ROTATION_DOWN = Rotation::down;*/

#endif // _GAMEBUINO_META_GRAPHICS_ST7735_H_
