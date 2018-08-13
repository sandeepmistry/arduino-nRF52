/*
  Copyright (c) 2015 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*
 * Defines for 16 bit timers used with  Servo library
 *
 * If _useTimerX is defined then TimerX is a 16 bit timer on the current board
 * timer16_Sequence_t enumerates the sequence that the timers should be allocated
 * _Nbr_16timers indicates how many 16 bit timers are available.
 */

#ifndef __SERVO_TIMERS_H__
#define __SERVO_TIMERS_H__

#include "nrf.h"

/**
 * NRF5 Only definitions
 * ---------------------
 */

// Uncomment to select allocatable timers, 3 and 4 are only available in nRF52
//#define _useTimer0
//#define _useTimer1
#define _useTimer2
//#define _useTimer3
//#define _useTimer4


// Timer IRQ priority should take in consideration other interrupts
#define TMR_PRIORITY                 1

// Register pre-scaler value is calculated from: TMR_FREQ = 16 MHz / (2 ^ reg_prescaler)
// To avoid preprocessor float imprecision manually calculate: log(16 MHz / TMR_FREQ) / log(2)
// Note that a TMR_FREQ > 3 MHz will not fit REFRESH_INTERVAL in a 16 bit timer
#define TMR_FREQ                     2000000
#define TMR_FREQ_REG_PRESCALER       3

#define TIMER_SHORTS_DISABLE_ALL     ( \
    (TIMER_SHORTS_COMPARE0_CLEAR_Disabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos) | \
    (TIMER_SHORTS_COMPARE1_CLEAR_Disabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos) | \
    (TIMER_SHORTS_COMPARE2_CLEAR_Disabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos) | \
    (TIMER_SHORTS_COMPARE3_CLEAR_Disabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos) | \
    (TIMER_SHORTS_COMPARE0_STOP_Disabled  << TIMER_SHORTS_COMPARE0_STOP_Pos)  | \
    (TIMER_SHORTS_COMPARE1_STOP_Disabled  << TIMER_SHORTS_COMPARE1_STOP_Pos)  | \
    (TIMER_SHORTS_COMPARE2_STOP_Disabled  << TIMER_SHORTS_COMPARE2_STOP_Pos)  | \
    (TIMER_SHORTS_COMPARE3_STOP_Disabled  << TIMER_SHORTS_COMPARE3_STOP_Pos))

// Each timer has 4 Capture/Compare set of registers
#define TMR_CC_REG0                  0
#define TMR_CC_REG1                  1
#define TMR_CC_REG2                  2
#define TMR_CC_REG3                  3

// Timers 0, 1 and 2 available in both NRF51 and NRF52
#if defined (_useTimer0)
  #define TMR0_HANDLER               TIMER0_IRQHandler
  #define TMR0_POINTER               NRF_TIMER0
  #define TMR0_IRQN                  TIMER0_IRQn
  #define TMR0_CC_REG                TMR_CC_REG0
#endif
#if defined (_useTimer1)
  #define TMR1_ISR                   TIMER1_IRQHandler
  #define TMR1_POINTER               NRF_TIMER1
  #define TMR1_IRQN                  TIMER1_IRQn
  #define TMR1_CC_REG                TMR_CC_REG0
#endif
#if defined (_useTimer2)
  #define TMR2_HANDLER               TIMER2_IRQHandler
  #define TMR2_POINTER               NRF_TIMER2
  #define TMR2_IRQN                  TIMER2_IRQn
  #define TMR2_CC_REG                TMR_CC_REG0
#endif
// Timers 3 and 4 only available in NRF52
#if defined (_useTimer3)
  #ifndef NRF52
    #define TMR3_HANDLER             TIMER4_IRQHandler
    #define TMR3_POINTER             NRF_TIMER4
    #define TMR3_IRQN                TIMER3_IRQn
    #define TMR3_CC_REG              TMR_CC_REG0
  #else
    #error "Servo timer 3 is only available in NRF52 processors."
  #endif
#endif
#if defined (_useTimer4)
  #ifndef NRF52
    #define TMR4_HANDLER             TIMER4_IRQHandler
    #define TMR4_POINTER             NRF_TIMER4
    #define TMR4_IRQN                TIMER4_IRQn
    #define TMR4_CC_REG              TMR_CC_REG0
  #else
    #error "Servo timer 4 is only available in NRF52 processors."
  #endif
#endif

// The timers here are organised in allocation sequence
typedef enum {
#if defined (_useTimer4)
  _timer4,
#endif
#if defined (_useTimer3)
  _timer3,
#endif
#if defined (_useTimer2)
  _timer2,
#endif
#if defined (_useTimer1)
  _timer1,
#endif
#if defined (_useTimer0)
  _timer0,
#endif
  _Nbr_16timers
} timer16_Sequence_t;

#endif    // __SERVO_TIMERS_H__
