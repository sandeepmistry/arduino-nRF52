/*
  SoftwareSerial TX Only test

  If your application is transmit only then setting
  #define _SS_TX_ONLY 1
  Will stop the library allocating a GPIO interrupt and save 1364 bytes of memory
 */
#define _SS_TX_ONLY 1 // Transmit only (Rx pin is unused/unassigned, but still needs a number)
#define _SS_MAX_RX_BUFF 128 // Increase the Rx buffer to 128 bytes (default is 64 bytes)
#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(2, 3); // RX, TX

uint32_t t;
bool hb_state = false; // heartbeat

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  SoftSerial.begin(9600);
  SoftSerial.println(__FILE__);
  SoftSerial.println(__TIME__);
  t = millis();
}

void loop()
{
  if (millis() - t > 1000)
  {
    t = millis();
    digitalWrite(LED_BUILTIN, hb_state);
    hb_state = !hb_state;
    SoftSerial.print(".");
  }

  yield();
}
