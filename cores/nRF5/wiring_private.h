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

#define PWM_COUNT 3
#define PWM_TIMER_COUNT 1 // 3 channels of TIMER1 are used. TIMER2 also could be used for PWM
#define PIN_FREE 0xffffffff

struct PWMContext {
  uint32_t pin;
  uint32_t value;
  #ifdef NRF51
  uint32_t channel;
  uint32_t mask;
  uint32_t event;
  #endif
};

struct PWMStatus {
  int8_t numActive;
  int8_t irqNumber;
};

#ifdef __cplusplus
} // extern "C"

#include "HardwareSerial.h"

#endif