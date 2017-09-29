/*
  Copyright (c) 2016 Arduino Srl.  All right reserved.

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
 *
 * | Pin number |  PRIMO Board pin |  PIN   | Label/Name      |
 * +------------+------------------+--------+-----------------+
 * |            | Digital          |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 0          | D0               |  P031  |                 |
 * | 1          | D1               |  P030  |                 |
 * | 2          | D2               |  P029  |                 |
 * | 3          | D3               |  P028  |                 |
 * | 4          | D4               |  P002  |                 |
 * | 5          | D5               |  P003  |                 |
 * | 6          | D6               |  P004  |                 |
 * | 7          | D7               |  P005  |                 |
 * | 8          | D8               |  P023  |                 |
 * | 9          | D9               |  P022  |                 | 
 * +------------+------------------+--------+-----------------+
 * |            | LEDs             |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 10         | 8                |  P008  | USER LED        |
 * | 11         | 9                |  P025  | RED LED         |
 * | 12         | 10               |  P026  | GREEN LED       |
 * | 13         | 11               |  P027  | BLUE LED        |
 * +------------+------------------+--------+-----------------+
 * |            | UART             |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 14         | TX               |  P011  |                 |
 * | 15         | RX               |  P012  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | I2C              |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 16         | SDA              |  P013  |                 |
 * | 17         | SCL              |  P014  |                 |
 * | 18         | SDA1(SDA2)       |  P015  |                 |
 * | 19         | SCL1(SCL2)       |  P016  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | INT              |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 20         | INT              |  P024  |                 |
 * | 21         | INT1             |  P020  |                 |
 * | 22         | INT2             |  P019  |                 |
 * | 23         | INT3             |  P006  |                 |
 * | 24         | INT4             |  P007  |                 |
 * +------------+------------------+--------+-----------------+
 * |            | TEST POINT       |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 25         |                  |  P017  | TP4             |
 * | 26         |                  |  P018  | TP5             |
 * +------------+------------------+--------+-----------------+
 * |            | NFC              |        |                 |
 * +------------+------------------+--------+-----------------+
 * | 27         |                  |  P009  | NFC1            |
 * | 28         |                  |  P010  | NFC2            |
 * +------------+------------------+--------+-----------------+
 * |            | 32.768KHz Crystal|        |                 |
 * +------------+------------------+--------+-----------------+
 * |            |                  |  P000  | XL1             | 
 * |            |                  |  P001  | XL2             | 
 * +------------+------------------+--------+-----------------+
 */

#include "variant.h"

/*
 * Pins descriptions
 */
//const PinDescription g_APinDescription[]=
const uint32_t g_ADigitalPinMap[] = 
{
  // 0 .. 9 - Digital pins
  // ----------------------
  // 0..9
  31, // { PORT0,  31, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A5, PWM0, NOT_ON_TIMER},
  30, // { PORT0,  30, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A4, PWM1, NOT_ON_TIMER},
  29, // { PORT0,  29, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A3, PWM2, NOT_ON_TIMER},
  28, // { PORT0,  28, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A2, PWM3, NOT_ON_TIMER},
  2, // { PORT0,  2, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A6, PWM4, NOT_ON_TIMER},
  3, // { PORT0,  3, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A0, PWM5, NOT_ON_TIMER},
  4, // { PORT0,  4, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A1, PWM6, NOT_ON_TIMER},
  5, // { PORT0,  5, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), ADC_A7, PWM7, NOT_ON_TIMER},
  23, // { PORT0,  23, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM8, NOT_ON_TIMER},
  22, // { PORT0,  22, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM9, NOT_ON_TIMER},

  // 10 .. 13 - LEDs
  // --------------------
  8, // { PORT0,  8, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM10, NOT_ON_TIMER},    //USER_LED
  25, // { PORT0,  25, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM11, NOT_ON_TIMER},   //RED_LED
  26, // { PORT0,  26, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM11, NOT_ON_TIMER},  //GREEN_LED
  27, // { PORT0,  27, PIO_DIGITAL, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM), No_ADC_Channel, PWM11, NOT_ON_TIMER},  //BLUE_LED

  // 14..15 - UART (Serial)
  // --------------------
  11, // { PORT0, 11, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TX
  12, // { PORT0, 12, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // RX

  // 16..17 I2C pins (SDA/SCL)
  // ----------------------
  13, // { PORT0,  13, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SDA
  14, // { PORT0,  14, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SCL

  // 18..19 I2C pins (SDA1/SCL1)
  // ----------------------
  15, // { PORT0,  15, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SDA1
  16, // { PORT0,  16, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER }, // SCL1

  // 20..24 - INT
  // --------------------
  24, // { PORT0, 24, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // INT
  20, // { PORT0, 20, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // INT1
  19, // { PORT0, 19, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // INT2
  6, // { PORT0, 6, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // INT3
  7, // { PORT0, 7, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // INT4
  
  // 25..26 - TEST POINT
  // --------------------
  17, // { PORT0, 17, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TP4
  18, // { PORT0, 18, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // TP5
  
  //27..28 - NFC
  // --------------------
  9, // { PORT0, 9, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER}, // NFC1
  10, // { PORT0, 10, PIO_DIGITAL, PIN_ATTR_DIGITAL, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER} // NFC2

} ;

//Uart Serial(14, 15);