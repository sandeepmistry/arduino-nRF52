/*
  SoftwareSerial.h - library for Arduino Primo
  Copyright (c) 2016 Arduino. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifndef SoftwareSerial_h
#define SoftwareSerial_h

#include <Arduino.h>
#include <Stream.h>
#include <WInterrupts.h>
#include <inttypes.h>
#include <variant.h>

/******************************************************************************
* Definitions
******************************************************************************/

#ifndef _SS_MAX_RX_BUFF
#define _SS_MAX_RX_BUFF 64  // RX buffer size
#endif

#ifndef _SS_TX_ONLY
#define _SS_TX_ONLY 0
#endif

class SoftwareSerial : public Stream
{
  private:
   // transmit data
   uint8_t _transmitPin;
   uint32_t _transmitBitMask;
   volatile uint32_t* _transmitPortRegister;

   // Expressed as micro seconds
   uint16_t _tx_delay_us;

#if (_SS_TX_ONLY == 0)
   // receive data
   uint8_t _receivePin;
   uint32_t _receiveBitMask;
   volatile uint32_t* _receivePortRegister;

   volatile uint32_t _intMask;

   // Expressed as micro seconds
   uint16_t _rx_delay_centering_us;
   uint16_t _rx_delay_intrabit_us;
   uint16_t _rx_delay_stopbit_us;

  uint16_t _buffer_overflow : 1;

   // static data
   uint8_t _receive_buffer[_SS_MAX_RX_BUFF] = {0};
   volatile uint8_t _receive_buffer_tail;
   volatile uint8_t _receive_buffer_head;

   static SoftwareSerial* active_object;
#endif

   uint16_t _inverse_logic : 1;

   // private methods
   inline void recv() __attribute__((__always_inline__));
   uint32_t rx_pin_read();
   void tx_pin_write(uint8_t pin_state) __attribute__((__always_inline__));
   void setTX(uint8_t transmitPin);
   void setRX(uint8_t receivePin);
   void setRxIntMsk(bool enable) __attribute__((__always_inline__));

  public:
   // public methods
   SoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false);
   ~SoftwareSerial();
   void begin(long speed);
   bool listen();
   void end();
   bool isListening()
   {
#if (_SS_TX_ONLY == 0)
      return this == active_object;
#else
      return 0;
#endif
   }
   bool stopListening();
   bool overflow()
   {
#if (_SS_TX_ONLY == 0)
      bool ret                  = _buffer_overflow;
      if (ret) _buffer_overflow = false;
      return ret;
#else
      return 0;
#endif
   }
   int peek();

   virtual size_t write(uint8_t byte);
   virtual int read();
   virtual int available();
   virtual void flush();
   operator bool() { return true; }

   using Print::write;

   // public only for easy access by interrupt handlers
   static inline void handle_interrupt() __attribute__((__always_inline__));
};

#if (_SS_TX_ONLY == 0)
SoftwareSerial* SoftwareSerial::active_object = NULL;
#endif

SoftwareSerial::SoftwareSerial(uint8_t receivePin, uint8_t transmitPin,
                               bool inverse_logic /* = false */)
    :
#if (_SS_TX_ONLY == 0)
      _rx_delay_centering_us(0),
      _rx_delay_intrabit_us(0),
      _rx_delay_stopbit_us(0),
      _buffer_overflow(false),
      _intMask(0),
      _receive_buffer_tail(0),
      _receive_buffer_head(0),
#endif
      _tx_delay_us(0),
      _inverse_logic(inverse_logic)
{
#if (_SS_TX_ONLY == 0)
   _receivePin = receivePin;
#endif
   _transmitPin = transmitPin;

   //_half_duplex = (_receivePin == _transmitPin) ? !_SS_TX_ONLY : 0;
}

SoftwareSerial::~SoftwareSerial() { end(); }

void SoftwareSerial::begin(long speed)
{
   setTX(_transmitPin);
#if (_SS_TX_ONLY == 0)
   setRX(_receivePin);
#endif

   // Precalculate the various delays
   // Calculate the distance between bit in micro seconds
   uint32_t bit_delay = (float(1) / speed) * 1000000;

   _tx_delay_us = bit_delay;
#if (_SS_TX_ONLY == 0)
   // Wait 1/2 bit - 2 micro seconds (time for interrupt to be served)
   _rx_delay_centering_us = (bit_delay / 2) - 2;
   // Wait 1 bit - 2 micro seconds (time in each loop iteration)
   _rx_delay_intrabit_us = bit_delay - 1;  // 2
   // Wait 1 bit (the stop one)
   _rx_delay_stopbit_us = bit_delay;
#endif
   delayMicroseconds(_tx_delay_us);

   listen();
}

bool SoftwareSerial::listen()
{
#if (_SS_TX_ONLY == 0)
   if (!_rx_delay_stopbit_us) return false;

   if (active_object != this)
   {
      if (active_object) active_object->stopListening();

      _buffer_overflow     = false;
      _receive_buffer_head = _receive_buffer_tail = 0;
      active_object                               = this;

      if (_inverse_logic)
      {
         // Start bit high
         _intMask = attachInterrupt(_receivePin, handle_interrupt, RISING);
      }
      else
      {
         // Start bit low
         _intMask = attachInterrupt(_receivePin, handle_interrupt, FALLING);
      }
      return true;
   }
#endif
   return false;
}

bool SoftwareSerial::stopListening()
{
#if (_SS_TX_ONLY == 0)
   if (active_object == this)
   {
      detachInterrupt(_receivePin);
      active_object = NULL;
      return true;
   }
#endif
   return false;
}

void SoftwareSerial::end() { stopListening(); }

int SoftwareSerial::read()
{
#if (_SS_TX_ONLY == 1)
   this->println(F("read() is invalid - _SS_TX_ONLY is defined."));
#else
   if (isListening() == 0)
   {
      return -1;
   }

   // Empty buffer?
   if (_receive_buffer_head == _receive_buffer_tail)
   {
      return -1;
   }

   // Read from "head"
   uint8_t d            = _receive_buffer[_receive_buffer_head];  // grab next byte
   _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
   return d;
#endif
}

int SoftwareSerial::available()
{
#if (_SS_TX_ONLY == 0)
   if (isListening() == 0)
      return 0;
   else
      return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
#else
   return 0;
#endif
}

size_t SoftwareSerial::write(uint8_t b)
{
   if (_tx_delay_us == 0)
   {
      setWriteError();
      return 0;
   }

   // By declaring these as local variables, the compiler will put them
   // in registers _before_ disabling interrupts and entering the
   // critical timing sections below, which makes it a lot easier to
   // verify the cycle timings
   volatile uint32_t* reg = _transmitPortRegister;
   uint32_t reg_mask      = _transmitBitMask;
   uint32_t inv_mask      = ~_transmitBitMask;
   bool inv               = _inverse_logic;
   uint16_t delay         = _tx_delay_us;

   if (inv) b = ~b;

// turn off interrupts for a clean txmit
#ifndef _SS_TX_ONLY
   NRF_GPIOTE->INTENCLR = _intMask;
#endif

   // Write the start bit
   if (inv)
      *reg |= reg_mask;
   else
      *reg &= inv_mask;

   delayMicroseconds(delay);

   // Write each of the 8 bits
   for (uint8_t i = 8; i > 0; --i)
   {
      if (b & 1)            // choose bit
         *reg |= reg_mask;  // send 1
      else
         *reg &= inv_mask;  // send 0

      delayMicroseconds(delay);
      b >>= 1;
   }

   // restore pin to natural state
   if (inv)
      *reg &= inv_mask;
   else
      *reg |= reg_mask;

// turn interrupts back on
#ifndef _SS_TX_ONLY
   NRF_GPIOTE->INTENSET = _intMask;
#endif

   delayMicroseconds(delay);

   return 1;
}

void SoftwareSerial::flush()
{
#ifndef _SS_TX_ONLY
   if (isListening() == 0) return;

   NRF_GPIOTE->INTENCLR = _intMask;

   _receive_buffer_head = _receive_buffer_tail = 0;

   NRF_GPIOTE->INTENSET = _intMask;
#endif
}

int SoftwareSerial::peek()
{
#ifdef _SS_TX_ONLY
   return -1;
#else
   if (isListening() == 0) return -1;

   // Empty buffer?
   if (_receive_buffer_head == _receive_buffer_tail) return -1;

   // Read from "head"
   return _receive_buffer[_receive_buffer_head];
#endif
}

// private methods

inline void SoftwareSerial::recv()
{
#if (_SS_TX_ONLY == 0)
   uint8_t d = 0;

   // If RX line is high, then we don't see any start bit
   // so interrupt is probably not for us
   if (_inverse_logic ? rx_pin_read() : !rx_pin_read())
   {
      NRF_GPIOTE->INTENCLR = _intMask;

      // Wait approximately 1/2 of a bit width to "center" the sample
      delayMicroseconds(_rx_delay_centering_us);

      // Read each of the 8 bits
      for (int8_t i = 8; i > 0; --i)
      {
         delayMicroseconds(_rx_delay_intrabit_us);
         // nRF52 needs another delay less than 1 uSec to be better synchronized
         // with the highest baud rates
         __ASM volatile(
             " NOP\n\t"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n"
             " NOP\n");

         d >>= 1;

         if (rx_pin_read())
         {
            d |= 0x80;
         }
      }
      if (_inverse_logic)
      {
         d = ~d;
      }

      // if buffer full, set the overflow flag and return
      uint8_t next = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
      if (next != _receive_buffer_head)
      {
         // save new data in buffer: tail points to where byte goes
         _receive_buffer[_receive_buffer_tail] = d;  // save new byte
         _receive_buffer_tail                  = next;
      }
      else
      {
         _buffer_overflow = true;
      }

      // skip the stop bit
      delayMicroseconds(_rx_delay_stopbit_us);

      NRF_GPIOTE->INTENSET = _intMask;
   }
#endif
}

uint32_t SoftwareSerial::rx_pin_read()
{
#if (_SS_TX_ONLY == 0)
   return *_receivePortRegister & digitalPinToBitMask(_receivePin);
#else
   return 0;
#endif
}

/* static */
inline void SoftwareSerial::handle_interrupt()
{
#if (_SS_TX_ONLY == 0)
   if (active_object != NULL)
   {
      active_object->recv();
   }
#endif
}

void SoftwareSerial::setTX(uint8_t tx)
{
   // First write, then set output. If we do this the other way around,
   // the pin would be output low for a short while before switching to
   // output high. Now, it is input with pullup for a short while, which
   // is fine. With inverse logic, either order is fine.
   _transmitPin = tx;

   digitalWrite(_transmitPin, _inverse_logic ? LOW : HIGH);
   pinMode(_transmitPin, OUTPUT);

   _transmitBitMask      = digitalPinToBitMask(_transmitPin);
   NRF_GPIO_Type* port   = digitalPinToPort(_transmitPin);
   _transmitPortRegister = portOutputRegister(port);
}

void SoftwareSerial::setRX(uint8_t rx)
{
#if (_SS_TX_ONLY == 0)
   _receivePin = rx;

   digitalWrite(_receivePin, _inverse_logic ? LOW : HIGH);
   pinMode(_receivePin, INPUT);

   _receiveBitMask      = digitalPinToBitMask(_receivePin);
   NRF_GPIO_Type* port  = digitalPinToPort(_receivePin);
   _receivePortRegister = portInputRegister(port);
#endif
}

#endif