/*
  Copyright (c) 2014 Arduino LLC.  All right reserved.
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

#ifdef NRF51

#include "nrf.h"

#include "Arduino.h"
#include "wiring_private.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TIMERS_COUNT 4

struct TimerContext {
  uint32_t timer_val;
  void   (*timer_handler)();
  const uint32_t timer_pos;   
  const uint32_t timer_mask;
  const uint32_t timer_enabled;
  
};

static struct TimerContext TContext[TIMERS_COUNT] = {
  {0, NULL, TIMER_INTENSET_COMPARE0_Pos, TIMER_INTENSET_COMPARE0_Msk, TIMER_INTENSET_COMPARE0_Enabled  },
  {0, NULL, TIMER_INTENSET_COMPARE1_Pos, TIMER_INTENSET_COMPARE1_Msk, TIMER_INTENSET_COMPARE1_Enabled  },
  {0, NULL, TIMER_INTENSET_COMPARE2_Pos, TIMER_INTENSET_COMPARE2_Msk, TIMER_INTENSET_COMPARE2_Enabled  },
  {0, NULL, TIMER_INTENSET_COMPARE3_Pos, TIMER_INTENSET_COMPARE3_Msk, TIMER_INTENSET_COMPARE3_Enabled  },
};

static int      timerEnabled    = 0;
static uint32_t timerPrescaler  = 1UL; 
static uint32_t timerMode       = TIMER_MODE_MODE_Timer; 

void setPrescaler( uint32_t prescaler  )
{
  timerPrescaler = prescaler;
}

uint32_t getPrescaler(  )
{
  return (timerPrescaler);
}

void setTimerInstance( uint32_t timer_instance, uint32_t timer_value, void (* timer_handle)()   )
{
  if (timer_instance < TIMERS_COUNT) {
    TContext[timer_instance].timer_val = timer_value;
    TContext[timer_instance].timer_handler = timer_handle;
  }

}



void setTimerMode( uint32_t mode  )
{
  timerMode = mode;
}

uint32_t getTimerMode(  )
{
  return (timerMode);
}

void  timerStart(void)
{
  NVIC_DisableIRQ(TIMER2_IRQn); // stop the IRQs

  NRF_TIMER2->MODE = timerMode;                          // Set the timer  Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                           // clear the task first to be usable for later
  NRF_TIMER2->PRESCALER   = timerPrescaler;              // f TIMER = 16 MHz / (2 ^ PRESCALER ) : 4 -> 1 MHz, 1 uS
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;     // Set counter to 16 bit resolution

  for (int i = 0; i < TIMERS_COUNT; i++)  {
    /*/ debug 
    Serial.print("start timer ");
    Serial.print(i);
    Serial.print(" at value ");
    Serial.print(TContext[i].timer_val);
    Serial.println();
    /*/

    // Set value for TIMER2 compare register 0 for the matrix events
    NRF_TIMER2->CC[i] = TContext[i].timer_val;  

    // Enable interrupt on Timer 2,  compare match events 
    if (TContext[i].timer_handler != NULL ) NRF_TIMER2->INTENSET |=  (TContext[i].timer_enabled << TContext[i].timer_pos);
  }
    
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
  // if (TContext[0].timer_handler != NULL ) NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos); 
  // if (TContext[1].timer_handler != NULL ) NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos); 
  // if (TContext[2].timer_handler != NULL ) NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos); 
  // if (TContext[3].timer_handler != NULL ) NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Pos); 

  NVIC_EnableIRQ(TIMER2_IRQn); // enable the IRQs
    
  NRF_TIMER2->TASKS_START = 1; // Start TIMER2
  
}



void TIMER2_IRQHandler(void)
{

  // scan the possible timers
  for (int i = 0; i < TIMERS_COUNT; i++){
    if ((NRF_TIMER2->EVENTS_COMPARE[i] != 0) && ((NRF_TIMER2->INTENSET & TContext[i].timer_mask) != 0)) {
      NRF_TIMER2->EVENTS_COMPARE[i] = 0;         //Clear compare register 0 event 
      NRF_TIMER2->CC[i] += TContext[i].timer_val;
      
      TContext[i].timer_handler();               // call the function registered
       
    }
  }

}

#ifdef __cplusplus
}
#endif

#endif
