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

#ifdef __cplusplus
extern "C" {
#endif

extern void init(void);

__STATIC_INLINE NRF_GPIO_Type * nrf_gpio_pin_port_decode(uint32_t * p_pin)
{
#if (GPIO_COUNT == 2)
        //if (*p_pin < P0_PIN_NUM)
    if (*p_pin < 32)
    {
        return NRF_P0;
    }
    else
    {
        *p_pin = *p_pin & (32 - 1);
        return NRF_P1;
    }
#else
    // The oldest definition case
    return NRF_GPIO;
#endif
}

#ifdef __cplusplus
}
#endif
