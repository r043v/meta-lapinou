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

#include "Display-ST7735.h"
#include <limits.h>
#include <SPI.h>
#include "FastLED.h"

inline void Display_ST7735::dataMode() {
//	tftCsPinLo;	tftDcPinHi;
	*rsport |= rspinmask; *rsport &= ~cspinmask;
}

inline void Display_ST7735::commandMode() {
//	tftCsPinLo;	tftDcPinLo;
	*rsport &= ~tftCsDcPinMask;
}

void Display_ST7735::idleMode() {
//	tftCsPinHi;
	*rsport |= tftCsPinMask;
}

#define DELAY 0x80
static const uint8_t
	RcmdReset[] = {
		2,
		ST7735_SWRESET, DELAY,      //  1: Software reset, 0 args, w/delay
		150,                    //     150 ms delay
		ST7735_SLPOUT, DELAY,       //  2: Out of sleep mode, 0 args, w/delay
		150,                    //     150 ms delay
	};
static const uint8_t
	Rcmd[] = {                      // Init for 7735R, part 1 (red or green tab)
		19,                         // 15 commands in list:
		ST7735_FRMCTR1, 3,          //  3: Frame rate ctrl - normal mode, 3 args:
			0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
		ST7735_FRMCTR2, 3,          //  4: Frame rate control - idle mode, 3 args:
			0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
		ST7735_FRMCTR3, 6,          //  5: Frame rate ctrl - partial mode, 6 args:
			0x01, 0x2C, 0x2D,       //     Dot inversion mode
			0x01, 0x2C, 0x2D,       //     Line inversion mode
		ST7735_INVCTR, 1,           //  6: Display inversion ctrl, 1 arg, no delay:
			0x07,                   //     No inversion
		ST7735_PWCTR1, 3,           //  7: Power control, 3 args, no delay:
			0xA2,
			0x02,                   //     -4.6V
			0x84,                   //     AUTO mode
		ST7735_PWCTR2, 1,           //  8: Power control, 1 arg, no delay:
			0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
		ST7735_PWCTR3, 2,           //  9: Power control, 2 args, no delay:
			0x0A,                   //     Opamp current small
			0x00,                   //     Boost frequency
		ST7735_PWCTR4, 2,           // 10: Power control, 2 args, no delay:
			0x8A,                   //     BCLK/2, Opamp current small & Medium low
			0x2A,
		ST7735_PWCTR5, 2,           // 11: Power control, 2 args, no delay:
			0x8A, 0xEE,
		ST7735_VMCTR1, 1,           // 12: Power control, 1 arg, no delay:
			0x0E,
		ST7735_INVOFF, 0,           // 13: Don't invert display, no args, no delay
		ST7735_MADCTL, 1,           // 14: Memory access control (directions), 1 arg:
			0xC8,                   //     row addr/col addr, bottom to top refresh
		ST7735_COLMOD, 1,           // 15: set color mode, 1 arg, no delay:
			0x05,                   //     16-bit color

		                            // Init for 7735R, part 2 (red tab only)
		                            //  2 commands in list:
		ST7735_CASET, 4,            //  1: Column addr set, 4 args, no delay:
			0x00, 0x00,             //     XSTART = 0
			0x00, 0x7F,             //     XEND = 127
		ST7735_RASET, 4,            //  2: Row addr set, 4 args, no delay:
			0x00, 0x00,             //     XSTART = 0
			0x00, 0x9F,             //     XEND = 159

		                            // Init for 7735R, part 3 (red or green tab)
		                            //  4 commands in list:
		ST7735_GMCTRP1, 16,         //  1: Magical unicorn dust, 16 args, no delay:
			0x02, 0x1c, 0x07, 0x12,
			0x37, 0x32, 0x29, 0x2d,
			0x29, 0x25, 0x2B, 0x39,
			0x00, 0x01, 0x03, 0x10,
		ST7735_GMCTRN1, 16,         //  2: Sparkles and rainbows, 16 args, no delay:
			0x03, 0x1d, 0x07, 0x06,
			0x2E, 0x2C, 0x29, 0x2D,
			0x2E, 0x2E, 0x37, 0x3F,
			0x00, 0x00, 0x02, 0x10,
		ST7735_NORON, 0,            //  3: Normal display on, no args, no delay
		ST7735_DISPON, 0, };        //  4: Main screen turn on, no args, no delay

void Display_ST7735::commandList(const uint8_t *addr){
	uint8_t	numCommands, numArgs, ms;

	SPI.beginTransaction(SPISettings(24000000, MSBFIRST, SPI_MODE0));

	numCommands = *(addr++);      // Number of commands to follow
	while (numCommands--) {       // For each command...
		commandMode();
		spiSend8( *(addr++) );
//		spiwrite(*(addr++));  //   Read, issue command
		numArgs	= *(addr++);      //   Number of args to follow
		ms = numArgs & DELAY;     //   If hibit set, delay follows args
		numArgs &= ~DELAY;        //   Mask out delay bit
		dataMode();
		while (numArgs--) {       //   For each argument...
			//spiwrite(*(addr++)); //     Read, issue argument
			spiSend8( *(addr++) );
		}

		if( ms ){
			ms = *(addr++); // Read post-command delay time (ms)
			delay(ms);
		}
	}

	idleMode();
	SPI.endTransaction();
}
uint32_t spiDma( uint32_t chnltx, void *txdata, size_t n );

static SPISettings tftSPISettings;

Display_ST7735::Display_ST7735(int8_t cs, int8_t rs) {}

void Display_ST7735::startLine( uint8_t L ){
	setAddrWindow(0, L, 159, L);
	SPI.beginTransaction(tftSPISettings);
	dataMode();
}

void Display_ST7735::fullScreenSpi(void){
	setAddrWindow(0, 0, 159, 127);
	SPI.beginTransaction(tftSPISettings);
	dataMode();
}

void Display_ST7735::commonInit() {
	csport = rsport = &(PORT->Group[1].OUT.reg);
	cspinmask = (1 << 22);
	rspinmask = (1 << 23);
	PORT->Group[1].DIR.reg |= tftCsDcPinMask;

	SPI.begin();
	tftSPISettings = SPISettings(24000000, MSBFIRST, SPI_MODE0);

	// toggle RST low to reset; CS low so it'll listen to us
	//*csport &= ~cspinmask;
	*rsport = 0;
}

// Initialization for ST7735R screens (green or red tabs)
void Display_ST7735::init() {
	commonInit();
	if (!PM->RCAUSE.bit.SYST) {
		commandList(RcmdReset);
	}
	commandList(Rcmd);
	commandMode();
	spiSend8( ST7735_MADCTL );
	dataMode();
	spiSend8( 0xC0 );
}

void Display_ST7735::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	commandMode();
	spiSend8( ST7735_CASET );
	dataMode();
	spiSend8( 0 );
	spiSend8( x0 );
	spiSend8( 0 );
	spiSend8( x1 );
	commandMode();
	spiSend8( ST7735_RASET );
	dataMode();
	spiSend8( 0 );
	spiSend8( y0 );
	spiSend8( 0 );
	spiSend8( y1 );
	commandMode();
	spiSend8( ST7735_RAMWR );
//	dataMode();
}

void Display_ST7735::drawBuffer(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, uint16_t h) {
	setAddrWindow(x, y, x + w - 1, y + h - 1);
	SPI.beginTransaction(tftSPISettings);
	dataMode();
	spiDma( 0, buffer, w*h*2 );
	idleMode();
	SPI.endTransaction();
}

extern void dmaStart( uint32_t channel );
extern void dmaWait( uint32_t );
extern void dmaEnd( uint32_t channel );
extern void memcpy32( uint32_t channel, void *dst, const void *src, size_t n);
extern void memcpyTest( void );
extern void dmaWait2( uint32_t channel );
extern void memset32( uint32_t channel, void *dst, uint32_t * value, size_t n);
extern void spiMemset32( uint32_t channel, uint32_t * value, size_t n );
extern void spiMemset16( uint32_t channel, uint32_t * value, size_t n );
extern void spiMemset8( uint32_t * value, size_t lines, size_t line );

// fill a rectangle
void Display_ST7735::fillRect(uint16_t color, int16_t x, int16_t y, int16_t xx, int16_t yy) {
	setAddrWindow( x, y, xx, yy );

	uint32_t n = 160;//(160 * 128) / 32;

	uint8_t * c = (uint8_t *)&color;

	SPI.beginTransaction(tftSPISettings);
	dataMode();
//hello();
	while( n-- ){
		spiSend16x128( c[1], c[0] );
	};
//bye();
	idleMode();
	SPI.endTransaction();
}

void Display_ST7735::setRotation( uint8_t r ) {
	SPI.beginTransaction(tftSPISettings);
	commandMode();
	spiSend8( ST7735_MADCTL );
	dataMode();
	spiSend8( r );
	idleMode();
	SPI.endTransaction();
}

void Display_ST7735::invertDisplay(bool i) {
	commandMode();
	spiSend8(i ? ST7735_INVON : ST7735_INVOFF);
	dataMode();
}

void Display_ST7735::setPower(bool i) {
	commandMode();
	spiSend8(i ? ST7735_DISPON : ST7735_DISPOFF);
	dataMode();
}
