
#include "gdl.h"
#include "FastLED.h"
#include <SPI.h>

#define BTN_CS (25)

#define btnPinHi FastPin<BTN_CS>::hi()
#define btnPinLo FastPin<BTN_CS>::lo()
#define btnPinOutput FastPin<BTN_CS>::setOutput()

SPISettings spiSettings = SPISettings(24000000, MSBFIRST, SPI_MODE0);

struct buttons buttons;

void initButtons(void){
  btnPinOutput;
  btnPinHi;
}

void readButtons(void){
  SPI.beginTransaction( spiSettings );
  btnPinLo;
  buttons.last.raw = buttons.current.raw;
  //delayMicroseconds(1);
  buttons.current.raw = SPI.transfer(1);
  btnPinHi;
  SPI.endTransaction();
}
