#include "Adafruit_HT1632.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

// Constructor for single GFX matrix
Adafruit_HT1632LEDMatrix::Adafruit_HT1632LEDMatrix(uint8_t data, uint8_t wr,
  uint8_t cs1) : Adafruit_GFX(24, 16), matrices(NULL), matrixNum(0) {
  if((matrices = (Adafruit_HT1632 *)malloc(sizeof(Adafruit_HT1632)))) {
    matrices[0] = Adafruit_HT1632(data, wr, cs1);
    matrixNum   = 1;
  }
}

// Constructor for two matrices, tiled side-by-side for GFX
Adafruit_HT1632LEDMatrix::Adafruit_HT1632LEDMatrix(uint8_t data, uint8_t wr,
  uint8_t cs1, uint8_t cs2) : Adafruit_GFX(48, 16), matrices(NULL),
  matrixNum(0) {
  if((matrices = (Adafruit_HT1632 *)malloc(2 * sizeof(Adafruit_HT1632)))) {
    matrices[0] = Adafruit_HT1632(data, wr, cs1);
    matrices[1] = Adafruit_HT1632(data, wr, cs2);
    matrixNum   = 2;
  }
}

// Constructor for three matrices, tiled side-by-side for GFX
Adafruit_HT1632LEDMatrix::Adafruit_HT1632LEDMatrix(uint8_t data, uint8_t wr,
  uint8_t cs1, uint8_t cs2, uint8_t cs3) : Adafruit_GFX(72, 16),
  matrices(NULL), matrixNum(0) {
  if((matrices = (Adafruit_HT1632 *)malloc(3 * sizeof(Adafruit_HT1632)))) {
    matrices[0] = Adafruit_HT1632(data, wr, cs1);
    matrices[1] = Adafruit_HT1632(data, wr, cs2);
    matrices[2] = Adafruit_HT1632(data, wr, cs3);
    matrixNum   = 3;
  }
}

// Constructor for four matrices, tiled side-by-side for GFX
Adafruit_HT1632LEDMatrix::Adafruit_HT1632LEDMatrix(uint8_t data, uint8_t wr,
  uint8_t cs1, uint8_t cs2, uint8_t cs3, uint8_t cs4) : Adafruit_GFX(96, 16),
  matrices(NULL), matrixNum(0) {
  if((matrices = (Adafruit_HT1632 *)malloc(4 * sizeof(Adafruit_HT1632)))) {
    matrices[0] = Adafruit_HT1632(data, wr, cs1);
    matrices[1] = Adafruit_HT1632(data, wr, cs2);
    matrices[2] = Adafruit_HT1632(data, wr, cs3);
    matrices[3] = Adafruit_HT1632(data, wr, cs4);
    matrixNum   = 4;
  }
}

void Adafruit_HT1632LEDMatrix::setPixel(uint8_t x, uint8_t y) {
  drawPixel(x, y, 1);
}

void Adafruit_HT1632LEDMatrix::clrPixel(uint8_t x, uint8_t y) {
  drawPixel(x, y, 0);
}

void Adafruit_HT1632LEDMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

  switch(rotation) { // Rotate pixel into device-specific coordinates
   case 1:
    _swap_int16_t(x, y);
    x = WIDTH  - 1 - x;
    break;
   case 2:
    x = WIDTH  - 1 - x;
    y = HEIGHT - 1 - y;
    break;
   case 3:
    _swap_int16_t(x, y);
    y = HEIGHT - 1 - y;
    break;
  }

  uint8_t m = x / 24; // Which matrix controller is pixel on?
  x %= 24;            // Which column in matrix?

  uint16_t i;

  if(x < 8)       i =       7;
  else if(x < 16) i = 128 + 7;
  else            i = 256 + 7;
  i -= (x & 7);

  if(y < 8) y *= 2;
  else      y  = (y-8) * 2 + 1;
  i += y * 8;

  if(color) matrices[m].setPixel(i);
  else      matrices[m].clrPixel(i);
}

boolean Adafruit_HT1632LEDMatrix::begin(uint8_t type) {
  if(matrixNum) { // Did malloc() work OK?
    for(uint8_t i=0; i<matrixNum; i++) matrices[i].begin(type);
    return true;
  }
  return false;
}

void Adafruit_HT1632LEDMatrix::clearScreen() {
  for(uint8_t i=0; i<matrixNum; i++) matrices[i].clearScreen();
}

void Adafruit_HT1632LEDMatrix::fillScreen() {
  for(uint8_t i=0; i<matrixNum; i++) matrices[i].fillScreen();
}

void Adafruit_HT1632LEDMatrix::setBrightness(uint8_t b) {
  for(uint8_t i=0; i<matrixNum; i++) matrices[i].setBrightness(b);
}

void Adafruit_HT1632LEDMatrix::blink(boolean b) {
  for(uint8_t i=0; i<matrixNum; i++) matrices[i].blink(b);
}

void Adafruit_HT1632LEDMatrix::writeScreen() {
  for(uint8_t i=0; i<matrixNum; i++) matrices[i].writeScreen();
}

//////////////////////////////////////////////////////////////////////////

Adafruit_HT1632::Adafruit_HT1632(int8_t data, int8_t wr, int8_t cs, int8_t rd) {
  _data = data;
  _wr   = wr;
  _cs   = cs;
  _rd   = rd;
  memset(ledmatrix, 0, sizeof(ledmatrix));
}

void Adafruit_HT1632::begin(uint8_t type) {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH);
  pinMode(_data, OUTPUT);

  if(_rd >= 0) {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
  }

#ifdef __AVR__
    csport   = portOutputRegister(digitalPinToPort(_cs));
    csmask   = digitalPinToBitMask(_cs);
    wrport   = portOutputRegister(digitalPinToPort(_wr));
    wrmask   = digitalPinToBitMask(_wr);
    dataport = portOutputRegister(digitalPinToPort(_data));
    datadir  = portModeRegister(digitalPinToPort(_data));
    datamask = digitalPinToBitMask(_data);
#endif

  sendcommand(ADA_HT1632_SYS_EN);
  sendcommand(ADA_HT1632_LED_ON);
  sendcommand(ADA_HT1632_BLINK_OFF);
  sendcommand(ADA_HT1632_MASTER_MODE);
  sendcommand(ADA_HT1632_INT_RC);
  sendcommand(type);
  sendcommand(ADA_HT1632_PWM_CONTROL | 0xF);
}

void Adafruit_HT1632::setBrightness(uint8_t pwm) {
  if(pwm > 15) pwm = 15;
  sendcommand(ADA_HT1632_PWM_CONTROL | pwm);
}

void Adafruit_HT1632::blink(boolean blinky) {
  sendcommand(blinky ? ADA_HT1632_BLINK_ON : ADA_HT1632_BLINK_OFF);
}

void Adafruit_HT1632::setPixel(uint16_t i) {
  ledmatrix[i/8] |= (1 << (i & 7));
}

void Adafruit_HT1632::clrPixel(uint16_t i) {
  ledmatrix[i/8] &= ~(1 << (i & 7));
}

void Adafruit_HT1632::dumpScreen() {
  Serial.println(F("---------------------------------------"));

  for (uint16_t i=0; i<sizeof(ledmatrix); i++) {
    Serial.print("0x");
    Serial.print(ledmatrix[i], HEX);
    Serial.print(" ");
    if (i % 3 == 2) Serial.println();
  }

  Serial.println(F("\n---------------------------------------"));
}

void Adafruit_HT1632::writeScreen() {

#ifdef __AVR__
  *csport  &= ~csmask;
  *datadir |=  datamask; // OUTPUT
#else
  digitalWrite(_cs, LOW);
#endif

  writedata(ADA_HT1632_WRITE, 3);
  // send with address 0
  writedata(0, 7);

  for(uint16_t i=0; i<sizeof(ledmatrix); i+=2) {
    writedata(((uint16_t)ledmatrix[i] << 8) | ledmatrix[i+1], 16);
  }

#ifdef __AVR__
  *csport  |=  csmask;
  *datadir &= ~datamask; // INPUT
#else
  digitalWrite(_cs, HIGH);
#endif
}

void Adafruit_HT1632::clearScreen() {
  memset(ledmatrix, 0, sizeof(ledmatrix));
  writeScreen();
}

void Adafruit_HT1632::writedata(uint16_t d, uint8_t bits) {
#ifdef __AVR__
  for(uint16_t bit = 1<<(bits-1); bit; bit >>= 1) {
    *wrport &= ~wrmask;
    if(d & bit) *dataport |=  datamask;
    else        *dataport &= ~datamask;
    *wrport |=  wrmask;
  }
#else
  pinMode(_data, OUTPUT);
  for(uint16_t bit = 1<<(bits-1); bit; bit >>= 1) {
    digitalWrite(_wr, LOW);
    digitalWrite(_data, (d & bit) ? HIGH : LOW);
    digitalWrite(_wr, HIGH);
  }
  pinMode(_data, INPUT);
#endif
}

void Adafruit_HT1632::writeRAM(uint8_t addr, uint8_t data) {
  //Serial.print("Writing 0x"); Serial.print(data&0xF, HEX);
  //Serial.print(" to 0x"); Serial.println(addr & 0x7F, HEX);

  uint16_t d = ADA_HT1632_WRITE;
  d <<= 7;
  d |= addr & 0x7F;
  d <<= 4;
  d |= data & 0xF;

#ifdef __AVR__
  *csport &= ~csmask;
  writedata(d, 14);
  *csport |=  csmask;
#else
  digitalWrite(_cs, LOW);
  writedata(d, 14);
  digitalWrite(_cs, HIGH);
#endif
}

void Adafruit_HT1632::sendcommand(uint8_t cmd) {
#ifdef __AVR__
  *csport  &= ~csmask;
  *datadir |=  datamask; // OUTPUT
  writedata((((uint16_t)ADA_HT1632_COMMAND << 8) | cmd) << 1, 12);
  *datadir &= ~datamask; // INPUT
  *csport  |=  csmask;
#else
  digitalWrite(_cs, LOW);
  writedata((((uint16_t)ADA_HT1632_COMMAND << 8) | cmd) << 1, 12);
  digitalWrite(_cs, HIGH);
#endif
}

void Adafruit_HT1632::fillScreen() {
  memset(ledmatrix, 0xFF, sizeof(ledmatrix));
  writeScreen();
}
