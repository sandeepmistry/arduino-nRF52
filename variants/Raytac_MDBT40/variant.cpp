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

#include "variant.h"


// Pin map from 0-indexed pin number on the MDBT40 chip to the 
// digital P0.XX pin numbering scheme
// taken from https://www.tinyosshop.com/datasheet/MDBT40%20spec-Version%20A4.pdf
// Lines without comments are P0.XX pins.
const uint32_t g_ADigitalPinMap[] = {
    // 0 - 1
    (uint32_t)-1, // GND
    (uint32_t)-1, // GND

    // 2 - 11
    (uint32_t)-1, // AVDD
    21,
    22,
    23,
    24,
    25,
    (uint32_t)-1, // XL2 - crystal
    (uint32_t)-1, // XL1 - crystal
    28,
    29,

    // 12 - 24
    (uint32_t)-1, // GND
    (uint32_t)-1, // VDD
    (uint32_t)-1, // DCC
    30,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    (uint32_t)-1, // GND
    
    // 25 - 33
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,

    // 34 - 35
    (uint32_t)-1, // SWDIO
    (uint32_t)-1, // SWCLK,
    
    // 36 - 41
    17,
    18,
    19,
    20,
    (uint32_t)-1, // DEC2
    (uint32_t)-1, // GND
};
