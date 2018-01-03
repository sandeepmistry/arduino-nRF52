#ifndef SINOBIT_H_
#define SINOBIT_H_

#include "Adafruit_GFX.h"
#include "Adafruit_HT1632.h"
#include "scroll_support.h"


#define SINOBIT_PAD_P0 (0)
#define SINOBIT_PAD_P1 (1)
#define SINOBIT_PAD_P2 (2)
#define SINOBIT_PAD_P3 (3)
#define SINOBIT_PAD_P4 (4)
#define SINOBIT_PAD_P5 (10)

#define SINOBIT_P0 (0)
#define SINOBIT_P1 (1)
#define SINOBIT_P2 (2)
#define SINOBIT_P3 (3)
#define SINOBIT_P4 (4)
#define SINOBIT_P5 (5)
#define SINOBIT_P6 (6)
#define SINOBIT_P7 (7)
#define SINOBIT_P8 (8)
#define SINOBIT_P9 (9)
#define SINOBIT_P10 (10)
#define SINOBIT_P11 (11)
#define SINOBIT_P12 (12)
#define SINOBIT_P13 (13)
#define SINOBIT_P14 (14)
#define SINOBIT_P15 (15)
#define SINOBIT_P16 (16)
/* #define SINOBIT_P17 (17) */
/* #define SINOBIT_P18 (18) */
#define SINOBIT_P19 (19)
#define SINOBIT_P20 (20)

#define SINOBIT_BUTTON_A (5)
#define SINOBIT_BUTTON_B (11)

class Sinobit_HT1632 : public Adafruit_HT1632
{
 public:
  Sinobit_HT1632(int8_t data, int8_t wr, int8_t cs, int8_t rd = -1);
  void blankScreen();
};


class Sinobit : public Adafruit_GFX
{

 public:
   Sinobit();
   boolean begin();
   void clearScreen(void);
   void blankScreen(void);
   void fillScreen(void);
   void blink(boolean b);
   void setBrightness(uint8_t brightness);
   void writeScreen();
   void clrPixel(uint8_t x, uint8_t y);
   void setPixel(uint8_t x, uint8_t y);
   void drawPixel(int16_t x, int16_t y, uint16_t color);
   void setReadingDirection(readingDirection_t dir);
   void scroll(String message, uint16_t interstitialDelay);
 private:
   boolean translate(int16_t x, int16_t y, int16_t *x_out, int16_t *y_out);
   int8_t characterAdvance(char c, ScrollSupport *scroller);
   int8_t fontHeight();
   int8_t fontWidth();
   void printDirectionally(String message, ScrollSupport *scroller);
   Sinobit_HT1632 leds;
   readingDirection_t reading_direction;
};

#endif
