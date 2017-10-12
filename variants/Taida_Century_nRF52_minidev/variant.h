/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  Copyright (c) 2016 Frank Holtz. All right reserved.

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

#ifndef _VARIANT_STCT_NRF52_minidev_
#define _VARIANT_STCT_NRF52_minidev_

/** Master clock frequency */
#ifdef NRF52
#define VARIANT_MCK       (64000000ul)
#else
#define VARIANT_MCK       (16000000ul)
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
#define NUM_DIGITAL_PINS     (20u)
#define NUM_ANALOG_INPUTS    (6u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LED1                (5u)
#define PIN_LED2                (6u)
#define LED_BUILTIN             PIN_LED1

// Buttons
// KEY
#define PIN_BUTTON1             (9u)
// Reset -> read nordic documentation for disabling reset function
#define PIN_BUTTON2             (31u)

/*
 * Analog pins
 */
#define PIN_A0               (25u)
#define PIN_A1               (26u)
#define PIN_A2               (27u)
#define PIN_A3               (28u)
#define PIN_A4               (29u)
#define PIN_A5               (30u)

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
#ifdef NRF52
#define ADC_RESOLUTION    14
#else
#define ADC_RESOLUTION    10
#endif

// Other pins
#define PIN_AREF           (24)
static const uint8_t AREF = PIN_AREF;

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (21u)
#define PIN_SPI_MOSI         (22u)
#define PIN_SPI_SCK          (23u)

static const uint8_t SS   = 20u;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Serial interfaces
 */
// Serial
#ifdef STCT_NRF52_minidev_RGZ  // J11 - J12 soldered (RGZ)
#define PIN_SERIAL_RX       (2u)
#define PIN_SERIAL_TX       (3u)
#elif defined (STCT_NRF52_minidev_RSM)  // J15 - J16 soldered (RSM)
#define PIN_SERIAL_RX       (1u)
#define PIN_SERIAL_TX       (2u)
#else // original configuration
#define PIN_SERIAL_RX       (18u)
#define PIN_SERIAL_TX       (19u)
#endif

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1
#ifdef STCT_NRF52_minidev_RGZ
#define PIN_WIRE_SDA         (7u)
#define PIN_WIRE_SCL         (8u)
#elif defined (STCT_NRF52_minidev_RSM)
#define PIN_WIRE_SDA         (7u)
#define PIN_WIRE_SCL         (8u)
#else // original configuration
#define PIN_WIRE_SDA         (2u)
#define PIN_WIRE_SCL         (3u)
#endif

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#ifdef __cplusplus
}
#endif

#endif
