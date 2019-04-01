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

#ifndef _VARIANT_GENERIC_NRF52840_
#define _VARIANT_GENERIC_NRF52840_

/** Master clock frequency */
#define VARIANT_MCK       (64000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define GPIO_COUNT 2

// Number of pins defined in PinDescription array
#define PINS_COUNT           (48u)
#define NUM_DIGITAL_PINS     (48u)
#define NUM_ANALOG_INPUTS    (8u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LED              (13) // P0.13
#define LED_BUILTIN          PIN_LED

/*
 * Analog pins
 */
#define PIN_A0               (3) // P0.03
#define PIN_A1               (28) // P0.28
#define PIN_A2               (2) // P0.02
#define PIN_A3               (29) // P0.29
#define PIN_A4               (31) // P0.31
#define PIN_A5               (30) // P0.30
#define PIN_A6               (5) // P0.05
#define PIN_A7               (4) // P0.04


static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
static const uint8_t A2  = PIN_A2 ;
static const uint8_t A3  = PIN_A3 ;
static const uint8_t A4  = PIN_A4 ;
static const uint8_t A5  = PIN_A5 ;
static const uint8_t A6  = PIN_A6 ;
static const uint8_t A7  = PIN_A7 ;


#define ADC_RESOLUTION    14

/*
 * Serial interfaces
 */
// Serial
#define PIN_SERIAL_RX       (30) // P0.30
#define PIN_SERIAL_TX       (31) // P0.31

/*
 * SPI Interfaces
 */
//TODO: Определиться позднее
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
//TODO: Определиться позднее
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (20u) // P0.20
#define PIN_WIRE_SCL         (21u) // P0.21

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#ifdef __cplusplus
}
#endif

#endif
