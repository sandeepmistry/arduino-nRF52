/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  This particular adaption for ID115 was originally created by @rbaron
  https://github.com/rbaron/arduino-nRF5
*/

#ifndef _VARIANT_ID115_
#define _VARIANT_ID115_

/** Master clock frequency */
#ifdef NRF52
#define VARIANT_MCK       (64000000ul)
#else
#define VARIANT_MCK       (16000000ul)
#define F_CPU VARIANT_MCK
#endif

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT           (32u)
#define NUM_DIGITAL_PINS     (32u)
#define NUM_ANALOG_INPUTS    (6u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs

// TX pad on the ID115 board. The board doesn't have a built-in LED,
// so it's just a good place to put one
#define PIN_LED              (0) // P0.0
#define LED_BUILTIN          PIN_LED

/*
 * Analog pins
 */
#define PIN_A0               (1) // P0.01
#define PIN_A1               (2) // P0.02
#define PIN_A2               (3) // P0.03
#define PIN_A3               (4) // P0.04
#define PIN_A4               (5) // P0.05
#define PIN_A5               (6) // P0.06

static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
static const uint8_t A2  = PIN_A2 ;
static const uint8_t A3  = PIN_A3 ;
static const uint8_t A4  = PIN_A4 ;
static const uint8_t A5  = PIN_A5 ;
#ifdef NRF52
#define ADC_RESOLUTION    14
#else
#define ADC_RESOLUTION    10
#endif

/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (30) // P0.30
#define PIN_SERIAL_TX       (0) // P0.00

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (22) // P0.22
#define PIN_SPI_MOSI         (23) // P0.23
#define PIN_SPI_SCK          (24) // P0.24

static const uint8_t SS   = 25 ;  // P0.25
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (21u) // P0.21
#define PIN_WIRE_SCL         (22u) // P0.22
#define PIN_WIRE_RST         (24u) // P0.22

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
static const uint8_t RST = PIN_WIRE_RST;

#ifdef __cplusplus
}
#endif

#endif