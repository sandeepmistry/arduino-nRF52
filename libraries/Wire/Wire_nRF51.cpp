/*
 * TWI/I2C library for nRF5x
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef NRF51

extern "C" {
#include <string.h>
}


#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

/* Max cycles approximately to wait on RXDREADY and TXDREADY event,
 * This is optimized way instead of using timers, this is not power aware. */
#define MAX_TIMEOUT_LOOPS             (20000UL)        /**< MAX while loops to wait for RXD/TXD event */

TwoWire::TwoWire(NRF_TWI_Type * p_twi, IRQn_Type IRQn, uint8_t pinSDA, uint8_t pinSCL)
{
  this->_p_twi = p_twi;
  this->_IRQn = IRQn;
  this->_uc_pinSDA = g_ADigitalPinMap[pinSDA];
  this->_uc_pinSCL = g_ADigitalPinMap[pinSCL];
  transmissionBegun = false;
}

void TwoWire::begin(void) {
  //Master Mode
  master = true;

  NRF_GPIO->PIN_CNF[_uc_pinSCL] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  NRF_GPIO->PIN_CNF[_uc_pinSDA] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  _p_twi->EVENTS_RXDREADY = 0;
  _p_twi->EVENTS_TXDSENT = 0;
  _p_twi->PSELSCL = _uc_pinSCL;
  _p_twi->PSELSDA = _uc_pinSDA;
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);

  NVIC_ClearPendingIRQ(_IRQn);
  NVIC_SetPriority(_IRQn, 2);
  NVIC_EnableIRQ(_IRQn);

  clear();
}

void TwoWire::setClock(uint32_t baudrate) {
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos);

  uint32_t frequency;

  if (baudrate <= 100000)
  {
    frequency = TWI_FREQUENCY_FREQUENCY_K100;
  }
  else if (baudrate <= 250000)
  {
    frequency = TWI_FREQUENCY_FREQUENCY_K250;
  }
  else
  {
    frequency = TWI_FREQUENCY_FREQUENCY_K400;
  }

  _p_twi->FREQUENCY = frequency;
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);
}

void TwoWire::end() {
  _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t data_length, bool issue_stop_condition)
{
  uint32_t timeout = MAX_TIMEOUT_LOOPS;   /* max loops to wait for RXDREADY event*/
  rxBuffer.clear();

  _p_twi->ADDRESS = address;

  if (data_length == 0)
  {
      /* Return false for requesting data of size 0 */
      _p_twi->SHORTS = 0; //short are not enabled yet, may not be necessary
      return false;
  }
  else if (data_length == 1)
  {
      _p_twi->SHORTS = TWI_SHORTS_BB_STOP_Enabled << TWI_SHORTS_BB_STOP_Pos;
  }
  else
  {
      _p_twi->SHORTS = TWI_SHORTS_BB_SUSPEND_Enabled << TWI_SHORTS_BB_SUSPEND_Pos;
  }

  _p_twi->EVENTS_RXDREADY = 0;
  _p_twi->TASKS_STARTRX   = 1;

  /** @snippet [TWI HW master read] */
  while (true)
  {
      while(_p_twi->EVENTS_RXDREADY == 0 && _p_twi->EVENTS_ERROR == 0 && (--timeout))
      {
          // Do nothing.
      }
      _p_twi->EVENTS_RXDREADY = 0;

      if (timeout == 0 || _p_twi->EVENTS_ERROR != 0)
      {
        // Recover the peripheral as indicated by PAN 56: "TWI: TWI module lock-up." found at
        // Product Anomaly Notification document found at
        // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
        _p_twi->EVENTS_ERROR = 0;
        _p_twi->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
        _p_twi->POWER        = 0;
        nrf_delay_us(5);
        _p_twi->POWER        = 1;
        _p_twi->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;

        clear();

        _p_twi->SHORTS = 0;
        return false;
      }

      rxBuffer.store_char(_p_twi->RXD);

      if (--data_length == 1)
      {
          _p_twi->SHORTS = TWI_SHORTS_BB_STOP_Enabled << TWI_SHORTS_BB_STOP_Pos;
      }

      if (data_length == 0)
      {
          break;
      }

      // Recover the peripheral as indicated by PAN 56: "TWI: TWI module lock-up." found at
      // Product Anomaly Notification document found at
      // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
      nrf_delay_us(20);
      _p_twi->TASKS_RESUME = 1;
  }
  /** @snippet [TWI HW master read] */

  /* Wait until stop sequence is sent */
  while(_p_twi->EVENTS_STOPPED == 0)
  {
      // Do nothing.
  }
  _p_twi->EVENTS_STOPPED = 0;

  _p_twi->SHORTS = 0;
  return rxBuffer.available();
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
  return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address) {
  // save address of target and clear buffer
  txAddress = address;
  txBuffer.clear();

  _p_twi->ADDRESS = address;

  transmissionBegun = true;
}

// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool issue_stop_condition)
{
  uint32_t timeout = MAX_TIMEOUT_LOOPS;   /* max loops to wait for EVENTS_TXDSENT event*/

  if (txBuffer.available() == 0)
  {
      /* Return false for requesting data of size 0 */
      return false;
  }

  _p_twi->ADDRESS       = txAddress;
  _p_twi->TXD           = txBuffer.read_char();
  _p_twi->TASKS_STARTTX = 1;

  /** @snippet [TWI HW master write] */
  while (true)
  {
      while(_p_twi->EVENTS_TXDSENT == 0 && _p_twi->EVENTS_ERROR == 0 && (--timeout))
      {
          // Do nothing.
      }

      if (timeout == 0 || _p_twi->EVENTS_ERROR != 0)
      {
        // Recover the peripheral as indicated by PAN 56: "TWI: TWI module lock-up." found at
        // Product Anomaly Notification document found at
        // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
        _p_twi->EVENTS_ERROR = 0;
        _p_twi->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
        _p_twi->POWER        = 0;
        nrf_delay_us(5);
        _p_twi->POWER        = 1;
        _p_twi->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;

        begin();

        return false;
      }
      _p_twi->EVENTS_TXDSENT = 0;
      if (txBuffer.available() == 0)
      {
          break;
      }

      _p_twi->TXD = txBuffer.read_char();
  }
  /** @snippet [TWI HW master write] */

  if (issue_stop_condition)
  {
      _p_twi->EVENTS_STOPPED = 0;
      _p_twi->TASKS_STOP     = 1;
      /* Wait until stop sequence is sent */
      while(_p_twi->EVENTS_STOPPED == 0)
      {
          // Do nothing.
      }
      _p_twi->EVENTS_STOPPED = 0;
  }
  return true;

  // if (_p_twi->EVENTS_ERROR)
  // {
  //   _p_twi->EVENTS_ERROR = 0x0UL;

  //   uint32_t error = _p_twi->ERRORSRC;

  //   _p_twi->ERRORSRC = error;

  //   if (error == TWI_ERRORSRC_ANACK_Msk)
  //   {
  //     return 2;
  //   }
  //   else if (error == TWI_ERRORSRC_DNACK_Msk)
  //   {
  //     return 3;
  //   }
  //   else
  //   {
  //     return 4;
  //   }
  // }

  // return 0;
}

uint8_t TwoWire::endTransmission()
{
  return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData)
{
  // No writing, without begun transmission or a full buffer
  if ( !transmissionBegun || txBuffer.isFull() )
  {
    return 0 ;
  }

  txBuffer.store_char( ucData ) ;

  return 1 ;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  //Try to store all data
  for(size_t i = 0; i < quantity; ++i)
  {
    //Return the number of data stored, when the buffer is full (if write return 0)
    if(!write(data[i]))
      return i;
  }

  //All data stored
  return quantity;
}

int TwoWire::available(void)
{
  return rxBuffer.available();
}

int TwoWire::read(void)
{
  return rxBuffer.read_char();
}

int TwoWire::peek(void)
{
  return rxBuffer.peek();
}

void TwoWire::flush(void)
{
  // Do nothing, use endTransmission(..) to force
  // data transfer.
}

void TwoWire::onService(void)
{
}

bool TwoWire::clear() {
  bool bus_clear;

  // Save and disable TWI hardware so software can take control over the pins.
  uint32_t twi_state = _p_twi->ENABLE;
  _p_twi->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

  uint32_t clk_pin_config = NRF_GPIO->PIN_CNF[_uc_pinSCL];
  NRF_GPIO->PIN_CNF[_uc_pinSCL] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  uint32_t data_pin_config = NRF_GPIO->PIN_CNF[_uc_pinSDA];
  NRF_GPIO->PIN_CNF[_uc_pinSDA] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                                | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  NRF_GPIO->OUTSET = (1UL << _uc_pinSDA);
  NRF_GPIO->OUTSET = (1UL << _uc_pinSCL);
  nrf_delay_us(4);

  if ( ((NRF_GPIO->IN >> _uc_pinSDA) & 1UL) && ((NRF_GPIO->IN >> _uc_pinSCL) & 1UL) )
  {
      bus_clear = true;
  }
  else
  {
      uint_fast8_t i;
      bus_clear = false;

      // Clock max 18 pulses worst case scenario(9 for master to send the rest of command and 9
      // for slave to respond) to SCL line and wait for SDA come high.
      for (i=18; i--;)
      {
          NRF_GPIO->OUTCLR = (1UL << _uc_pinSCL);
          nrf_delay_us(4);
          NRF_GPIO->OUTSET = (1UL << _uc_pinSCL);
          nrf_delay_us(4);

          if (((NRF_GPIO->IN >> _uc_pinSDA) & 1UL) == 1)
          {
              bus_clear = true;
              break;
          }
      }
  }

  NRF_GPIO->PIN_CNF[_uc_pinSCL] = clk_pin_config;
  NRF_GPIO->PIN_CNF[_uc_pinSDA] = data_pin_config;

  _p_twi->ENABLE = twi_state;

  return bus_clear;
}

#if WIRE_INTERFACES_COUNT > 0
TwoWire Wire(NRF_TWI1, SPI1_TWI1_IRQn, PIN_WIRE_SDA, PIN_WIRE_SCL);

extern "C"
{
  void SPI1_TWI1_IRQHandler(void)
  {
    Wire.onService();
  }
}
#endif

#endif
