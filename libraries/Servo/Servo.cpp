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

#include <Arduino.h>
#include <Servo.h>

// Converts microseconds to timer tick and vice versa
#define usToTicks(_us)     ((TMR_FREQ / 1000000L) * (_us))
#define ticksToUs(_ticks)  ((unsigned)_ticks / (TMR_FREQ / 1000000L))

#define TRIM_DURATION  5              // compensation ticks to trim adjust for digitalWrite delays

static servo_t servos[MAX_SERVOS];    // static array of servo structures

static uint8_t ServoCount = 0;        // the total number of attached servos

// Index for the servo being pulsed for each timer (or -1 if refresh interval)
static volatile int8_t currentServoIndex[_Nbr_16timers];
#define REFRESH_INTERVAL_PERIOD            (-1)

// Timer controlling this servo
#define SERVO_INDEX_TO_TIMER(_servo_nbr)   ((timer16_Sequence_t)(_servo_nbr / SERVOS_PER_TIMER))
// Index of the servo on this timer
#define SERVO_INDEX_TO_CHANNEL(_servo_nbr) (_servo_nbr % SERVOS_PER_TIMER)
// Servo index by timer and channel
#define SERVO_INDEX(_timer, _channel)      ((_timer * SERVOS_PER_TIMER) + _channel)
// Servo structure by timer and channel
#define SERVO(_timer, _channel)            (servos[SERVO_INDEX(_timer,_channel)])

/************ static functions common to all instances ***********************/

#ifdef __cplusplus
extern "C" {
#endif

static __inline__ void Servo_Handler(timer16_Sequence_t timer, NRF_TIMER_Type *nrfTimer, uint32_t ccReg) __attribute__((always_inline));
static __inline__ void Servo_Handler(timer16_Sequence_t timer, NRF_TIMER_Type *nrfTimer, uint32_t ccReg)
{
  // Clear the interrupt event flag
  nrfTimer->EVENTS_COMPARE[ccReg] = 0;

  if (currentServoIndex[timer] == REFRESH_INTERVAL_PERIOD) {
    // Refresh interval completed so reset the timer
    nrfTimer->TASKS_CLEAR = 1;
    nrfTimer->CC[ccReg] = 0;
  } else {
    if (SERVO_INDEX(timer, currentServoIndex[timer]) < ServoCount &&
        SERVO(timer, currentServoIndex[timer]).Pin.isActive == true) {
      // It's an active channel so pulse it low
      NRF_GPIO->OUTCLR = digitalPinToBitMask(SERVO(timer, currentServoIndex[timer]).Pin.nbr);
    }
  }

  // Select the next servo controlled by this timer
  currentServoIndex[timer]++;
  if (SERVO_INDEX(timer, currentServoIndex[timer]) < ServoCount &&
      currentServoIndex[timer] < SERVOS_PER_TIMER) {
    if (SERVO(timer, currentServoIndex[timer]).Pin.isActive == true) {
      // It's an active channel so pulse it high
      NRF_GPIO->OUTSET = digitalPinToBitMask(SERVO(timer,currentServoIndex[timer]).Pin.nbr);
    }
    // Get the counter value
    unsigned int ticks = SERVO(timer, currentServoIndex[timer]).ticks;
    nrfTimer->CC[ccReg] += ticks + (ticks >> 6);
  } else {
    // Finished all channels so wait for the refresh period to expire before starting over
    // Allow a few ticks to ensure the next timer event is not missed
    if (nrfTimer->CC[ccReg] + 4 < usToTicks(REFRESH_INTERVAL)) {
      nrfTimer->CC[ccReg] = (uint32_t)usToTicks(REFRESH_INTERVAL) + (REFRESH_INTERVAL >> 5);
    } else {
      // Refresh interval has elapsed
      nrfTimer->CC[ccReg] += 4;
    }
    // Will get incremented at the end of the refresh period to start again at the first channel
    currentServoIndex[timer] = REFRESH_INTERVAL_PERIOD;
  }
}

#if defined (_useTimer0)
void TMR0_HANDLER(void) {
  Servo_Handler(_timer0, TMR0_POINTER, TMR0_CC_REG);
}
#endif
#if defined (_useTimer1)
void TMR1_HANDLER(void) {
  Servo_Handler(_timer1, TMR1_POINTER, TMR1_CC_REG);
}
#endif
#if defined (_useTimer2)
void TMR2_HANDLER(void) {
  Servo_Handler(_timer2, TMR2_POINTER, TMR2_CC_REG);
}
#endif
#if defined (_useTimer3)
void TMR3_HANDLER(void) {
  Servo_Handler(_timer3, TMR3_POINTER, TMR3_CC_REG);
}
#endif
#if defined (_useTimer4)
void TMR4_HANDLER(void) {
  Servo_Handler(_timer4, TMR4_POINTER, TMR4_CC_REG);
}
#endif

#ifdef __cplusplus
}
#endif

static void _initISR(NRF_TIMER_Type *nrfTimer, uint32_t ccReg, IRQn_Type timerIRQn)
{
  __disable_irq();
  nrfTimer->TASKS_STOP = 1;

  nrfTimer->MODE = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
  nrfTimer->BITMODE = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
  nrfTimer->PRESCALER = TMR_FREQ_REG_PRESCALER << TIMER_PRESCALER_PRESCALER_Pos;
  nrfTimer->SHORTS = TIMER_SHORTS_DISABLE_ALL;
  nrfTimer->TASKS_CLEAR = 1;            // Clear task 
  nrfTimer->EVENTS_COMPARE[ccReg] = 0;  // Clear interrupt event flag
  nrfTimer->CC[ccReg] = (uint32_t)usToTicks(REFRESH_INTERVAL);

  if (ccReg == TMR_CC_REG0) {
    nrfTimer->INTENSET = (TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos);
  } else if (ccReg == TMR_CC_REG1) {
    nrfTimer->INTENSET = (TIMER_INTENSET_COMPARE1_Set << TIMER_INTENSET_COMPARE1_Pos);
  } else if (ccReg == TMR_CC_REG2) {
    nrfTimer->INTENSET = (TIMER_INTENSET_COMPARE2_Set << TIMER_INTENSET_COMPARE2_Pos);
  } else if (ccReg == TMR_CC_REG3) {
    nrfTimer->INTENSET = (TIMER_INTENSET_COMPARE3_Set << TIMER_INTENSET_COMPARE3_Pos);
  }
  NVIC_ClearPendingIRQ(timerIRQn);
  NVIC_SetPriority(timerIRQn, TMR_PRIORITY);
  NVIC_EnableIRQ(timerIRQn);

  __enable_irq();
  nrfTimer->TASKS_START = 1;
}

static void initISR(timer16_Sequence_t timer)
{
#if defined (_useTimer0)
  if (timer == _timer0) {
    _initISR(TMR0_POINTER, TMR0_CC_REG, TMR0_IRQN);
  }
#endif
#if defined (_useTimer1)
  if (timer == _timer1) {
    _initISR(TMR1_POINTER, TMR1_CC_REG, TMR1_IRQN);
  }
#endif
#if defined (_useTimer2)
  if (timer == _timer2) {
    _initISR(TMR2_POINTER, TMR2_CC_REG, TMR2_IRQN);
  }
#endif
#if defined (_useTimer3)
  if (timer == _timer3) {
    _initISR(TMR3_POINTER, TMR3_CC_REG, TMR3_IRQN);
  }
#endif
#if defined (_useTimer4)
  if (timer == _timer4) {
    _initISR(TMR4_POINTER, TMR4_CC_REG, TMR4_IRQN);
  }
#endif
}

static void finISR(timer16_Sequence_t timer)
{
#if defined (_useTimer0)
  if (timer == _timer0) {
    TMR0_POINTER->TASKS_STOP = 1;
  }
#endif
#if defined (_useTimer1)
  if (timer == _timer1) {
    TMR1_POINTER->TASKS_STOP = 1;
  }
#endif
#if defined (_useTimer2)
  if (timer == _timer2) {
    TMR2_POINTER->TASKS_STOP = 1;
  }
#endif
#if defined (_useTimer3)
  if (timer == _timer3) {
    TMR3_POINTER->TASKS_STOP = 1;
  }
#endif
#if defined (_useTimer4)
  if (timer == _timer4) {
    TMR4_POINTER->TASKS_STOP = 1;
  }
#endif
}

static boolean isTimerActive(timer16_Sequence_t timer)
{
  // Returns true if any servo is active on this timer
  for (uint8_t channel = 0; channel < SERVOS_PER_TIMER; channel++) {
    if (SERVO(timer, channel).Pin.isActive == true) {
      return true;
    }
  }
  return false;
}

/****************** end of static functions ******************************/

Servo::Servo()
{
  if (ServoCount < MAX_SERVOS) {
    this->servoIndex = ServoCount++;                    // assign a servo index to this instance
    servos[this->servoIndex].ticks = usToTicks(DEFAULT_PULSE_WIDTH);   // store default values
  } else {
    this->servoIndex = INVALID_SERVO;  // too many servos
  }
}

uint8_t Servo::attach(int pin)
{
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t Servo::attach(int pin, int min, int max)
{
  if (this->servoIndex < MAX_SERVOS) {
    pinMode(pin, OUTPUT);
    servos[this->servoIndex].Pin.nbr = pin;
    this->min = min < MIN_PULSE_WIDTH ? MIN_PULSE_WIDTH : min;
    this->max = max > MAX_PULSE_WIDTH ? MAX_PULSE_WIDTH : max;
    timer16_Sequence_t timer = SERVO_INDEX_TO_TIMER(this->servoIndex);
    if (isTimerActive(timer) == false) {
      initISR(timer);
    }
    // This must be set after the check for isTimerActive
    servos[this->servoIndex].Pin.isActive = true;
  }
  return this->servoIndex;
}

void Servo::detach()
{
  servos[this->servoIndex].Pin.isActive = false;
  timer16_Sequence_t timer = SERVO_INDEX_TO_TIMER(this->servoIndex);
  if (isTimerActive(timer) == false) {
    finISR(timer);
  }
}

void Servo::write(int value)
{
  // Treat values less than MIN_PULSE_WIDTH as angles clamped to 0-180 degrees 
  if (value < MIN_PULSE_WIDTH) {
    if (value < 0) {
      value = 0;
    } else if (value > 180) {
      value = 180;
    }
    value = map(value, 0, 180, this->min, this->max);
  }
  this->writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
  // calculate and store the values for the given channel
  if (this->servoIndex < MAX_SERVOS) {    // ensure channel is valid
    if (value < this->min) {            // ensure pulse width is valid
      value = this->min;
    } else if (value > this->max) {
      value = this->max;
    }
    servos[this->servoIndex].ticks = usToTicks(value);
  }
}

int Servo::read() // return the value as degrees
{
  return map(this->readMicroseconds() + 1, this->min, this->max, 0, 180);
}

int Servo::readMicroseconds()
{
  return (this->servoIndex != INVALID_SERVO) ?
      ticksToUs(servos[this->servoIndex].ticks) : 0;
}

bool Servo::attached()
{
  return servos[this->servoIndex].Pin.isActive;
}
