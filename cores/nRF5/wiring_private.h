/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "wiring_constants.h"

#ifdef NRF52
#define PWM_COUNT 12
#define PWM_MODULE_COUNT 3 // 3 PWM modules
#define PWM_CHANNEL_COUNT 4 // 4 channels per PWM module
#else
#define PWM_COUNT 6
#define PWM_MODULE_COUNT 2 // 2 TIMER modules (1,2) are used. TIMER0 is used by the softdevice
#define PWM_CHANNEL_COUNT 3 // 4 channels per TIMER module. Channel 0 is used for setting the PWM signal HIGH, so only 3 are available
#endif


#define PIN_FREE 0xffffffff

struct PWMContext {
  uint32_t pin;
  #ifdef NRF51
  uint32_t value;
  uint32_t mask;
  uint32_t event;
  #endif
  uint32_t channel;
  uint32_t module;
};

struct PWMStatus {
  int8_t numActive;
  int8_t irqNumber;
};

#ifdef __cplusplus
} // extern "C"

#include "HardwareSerial.h"

#endif