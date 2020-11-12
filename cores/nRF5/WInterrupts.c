/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
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

#include <nrf.h>

#include "Arduino.h"
#include "wiring_private.h"

#include <string.h>

#ifdef NRF52
#define NUMBER_OF_GPIO_TE 8
#else
#define NUMBER_OF_GPIO_TE 4
#endif

static voidFuncPtr callbacksInt[NUMBER_OF_GPIO_TE] = { NULL };
static int8_t channelMap[NUMBER_OF_GPIO_TE] = { -1 };
static int enabled = 0;

/* Configure I/O interrupt sources */
static void __initialize()
{
  NVIC_DisableIRQ(GPIOTE_IRQn);
  NVIC_ClearPendingIRQ(GPIOTE_IRQn);
  NVIC_SetPriority(GPIOTE_IRQn, 3); // Same priority as Uart
  NVIC_EnableIRQ(GPIOTE_IRQn);
}

/*
 * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
 *        Replaces any previous function that was attached to the interrupt.
 */
int attachInterrupt(uint32_t pin, voidFuncPtr callback, uint32_t mode)
{
  uint32_t polarity;
  uint8_t ch;

  if (pin >= PINS_COUNT)
  {
    return -1;
  }

  pin = g_ADigitalPinMap[pin];

  switch (mode)
  {
    //gpiote channel does not support LOW and HIGH mode. These are mantained for compatibility
    case LOW:
      polarity = GPIOTE_CONFIG_POLARITY_HiToLo; //same as FALLING
    break;
    
    case HIGH:
      polarity = GPIOTE_CONFIG_POLARITY_LoToHi; //same as RISING
    break;

    case CHANGE:
      polarity = GPIOTE_CONFIG_POLARITY_Toggle;
      break;

    case FALLING:
      polarity = GPIOTE_CONFIG_POLARITY_HiToLo;
      break;

    case RISING:
      polarity = GPIOTE_CONFIG_POLARITY_LoToHi;
      break;

    default:
      return -1;
  }

  for (ch = 0; ch < NUMBER_OF_GPIO_TE; ch++)
  {
    if (channelMap[ch] == -1 || (uint32_t)channelMap[ch] == pin)
    {
      channelMap[ch] = pin;
      callbacksInt[ch] = callback;

      NRF_GPIOTE->CONFIG[ch] &= ~(GPIOTE_CONFIG_PSEL_Msk | GPIOTE_CONFIG_POLARITY_Msk);
      NRF_GPIOTE->CONFIG[ch] |= ((pin << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PSEL_Msk) |
                              ((polarity << GPIOTE_CONFIG_POLARITY_Pos) & GPIOTE_CONFIG_POLARITY_Msk);

      NRF_GPIOTE->CONFIG[ch] |= GPIOTE_CONFIG_MODE_Event;

      NRF_GPIOTE->INTENSET = (1 << ch);

      break;
    }
  }

  // enable the interrupt after the first one is configured
  if (!enabled)
  {
    __initialize();
    enabled = 1;
  }

  // return the interrupt mask
  return (1 << ch);
}

/*
 * \brief Turns off the given interrupt.
 */
void detachInterrupt(uint32_t pin)
{
  if (pin >= PINS_COUNT) {
    return;
  }

  pin = g_ADigitalPinMap[pin];

  for (int ch = 0; ch < NUMBER_OF_GPIO_TE; ch++) {
    if ((uint32_t)channelMap[ch] == pin) {
      channelMap[ch] = -1;
      callbacksInt[ch] = NULL;

      NRF_GPIOTE->CONFIG[ch] &= ~GPIOTE_CONFIG_MODE_Event;

      NRF_GPIOTE->INTENCLR = (1 << ch);

      break;
    }
  }
}

void GPIOTE_IRQHandler()
{
  uint32_t event;
  uint8_t ch;

  for (ch = 0; ch < NUMBER_OF_GPIO_TE; ch++)
  {
    event = offsetof(NRF_GPIOTE_Type, EVENTS_IN[ch]);
    if ((*(uint32_t *)((uint32_t)NRF_GPIOTE + event) == 0x1UL) && (NRF_GPIOTE->INTENSET & (1 << ch)))
    {
      break;
    }
  }

  // clear event
  *(uint32_t *)((uint32_t)NRF_GPIOTE + event) = 0;
  #if __CORTEX_M == 0x04
  volatile uint32_t dummy = *((volatile uint32_t *)((uint32_t)NRF_GPIOTE + event));
  (void)dummy;
  #endif
  
  // disable the interrupt
  NRF_GPIOTE->INTENCLR = (1 << ch);

  // initiate the callback
  callbacksInt[ch]();

  // enable the interrupt
  NRF_GPIOTE->INTENSET = (1 << ch);
}
