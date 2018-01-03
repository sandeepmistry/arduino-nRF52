#include "sinobit.h"
#include "scroll_support.h"


#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

Sinobit_HT1632::Sinobit_HT1632(int8_t data, int8_t wr, int8_t cs, int8_t rd)
  : Adafruit_HT1632(data, wr, cs, rd)
{
}

void Sinobit_HT1632::blankScreen()
{
    memset(ledmatrix, 0, sizeof(ledmatrix));
}


Sinobit::Sinobit() : Adafruit_GFX(12, 12), leds(15, 13, 16) {}


boolean Sinobit::translate(int16_t x, int16_t y, int16_t *x_out, int16_t *y_out)
{
  if (y < 0 || y > 11 || x < 0 || x > 11) {
    return false;
  } else if ((y < 8 && x < 8) ||  (y > 7 && x > 7)) {
    *y_out = y;
    *x_out = x;
  } else if (y > 7 && x < 8) {
    *y_out = y - 8;
    *x_out = x + 8;
  } else if (y < 8 && x > 7) {
    *y_out = y + 8;
    *x_out = x - 8;
  }
  return true;
}

void Sinobit::setPixel(uint8_t x, uint8_t y)
{
  drawPixel(x, y, 1);
}

void Sinobit::clrPixel(uint8_t x, uint8_t y)
{
  drawPixel(x, y, 0);
}

void Sinobit::drawPixel(int16_t x, int16_t y, uint16_t color)
{
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

  int16_t real_row, real_col;
  if (!translate(x, y, &real_col, &real_row)) return;

  uint16_t i;

  if (real_row < 8)       i =       7;
  else if (real_row < 16) i = 128 + 7;
  else                    i = 256 + 7;
  i -= (real_row & 7);

  if (real_col < 8) real_col *= 2;
  else              real_col  = (real_col-8) * 2 + 1;
  i += real_col * 8;

  if(color) leds.setPixel(i);
  else      leds.clrPixel(i);
}

boolean Sinobit::begin()
{
  leds.begin(ADA_HT1632_COMMON_16NMOS);
  return true;
}

void Sinobit::clearScreen()
{
  leds.clearScreen();
}

void Sinobit::blankScreen()
{
  leds.blankScreen();
}

void Sinobit::fillScreen()
{
  leds.fillScreen();
}

void Sinobit::setBrightness(uint8_t b)
{
  leds.setBrightness(b);
}

void Sinobit::blink(boolean b)
{
  leds.blink(b);
}

void Sinobit::writeScreen()
{
  leds.writeScreen();
}

void Sinobit::setReadingDirection(readingDirection_t dir)
{
  reading_direction = dir;
}


// Retrieve a character's bounds
// returns true if spicified char as a glyph

int8_t Sinobit::characterAdvance(char c, ScrollSupport *scroller)
{
  if(gfxFont) {
  	if (c != '\n' && c != '\r') { // Not a carriage return; is normal char
  	  uint8_t first = pgm_read_byte(&gfxFont->first);
  	  uint8_t last  = pgm_read_byte(&gfxFont->last);
  	  if((c >= first) && (c <= last)) { // Char present in this font?
  		GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c - first]);
  		return scroller->selectAdvance(pgm_read_byte(&glyph->xAdvance),
									   pgm_read_byte(&glyph->height) + 1);
  	  } else {
  		return 0;
  	  }
  	}
	
  } else { // Default font
	
  	if(c != '\n' && c != '\r') {  // Normal char; ignore carriage returns
  	  return scroller->selectAdvance(7,8);
  	}
  	return true;
  }
}

int8_t Sinobit::fontHeight()
{
  if (gfxFont) {
	return pgm_read_byte(&gfxFont->yAdvance);
  } else {
	return 8;
  }
}


int8_t Sinobit::fontWidth()
{
  if (gfxFont) {
	uint8_t first = pgm_read_byte(&gfxFont->first);
	uint8_t last  = pgm_read_byte(&gfxFont->last);
	uint8_t w = 0;
	for (int c = first; c <= last; c++) {
	  if (c != '\n' && c != '\r') {
		GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
		int8_t glyph_width = pgm_read_byte(&glyph->xAdvance);
		if (glyph_width <= 12) {
		  w = max(w, glyph_width);
		}
	  }
	}
	return w;
  } else {
	return 7;
  }
}


// print a string using the set reading direction

void Sinobit::printDirectionally(String message, ScrollSupport *scroller)
{
  const char *buffer = message.c_str();
  size_t size = message.length();
  int16_t x = cursor_x;
  int16_t y = cursor_y;
  while (size--) {
	drawChar(x, y, *buffer, 1, 0, 1);
	int8_t advance = characterAdvance(*buffer, scroller);
	x = scroller->nextCharacterX(x, advance);
	y = scroller->nextCharacterY(y, advance);
	buffer++;
  }
}


// scroll a string onto the LED array based on the reading direction that's set

void Sinobit::scroll(String message, uint16_t interstitialDelay)
{
  ScrollSupport *scroller = ScrollSupport::makeFor(reading_direction, message);
  int8_t h = fontHeight();
  int8_t w = fontWidth();
  int16_t x = scroller->initialX(w);
  int16_t y = scroller->initialY(gfxFont ? h : -h);
  while (!scroller->isFinished(x, y, w, h)) {
    setCursor(x, y);
    blankScreen();
    printDirectionally(message, scroller);
    writeScreen();
	x = scroller->nextX(x);
	y = scroller->nextY(y);
    delay(interstitialDelay);
  }
  delete scroller;
}  
