### SoftwareSerial

This library is based off SoftwareSerial from @arduino-org's arduino-core-nrf52 release [1.0.1](https://github.com/arduino-org/arduino-core-nrf52/releases/tag/1.0.1)

#### Modifications
* merged SoftwareSerial.cpp into SoftwareSerial.h to allow ```_SS_MAX_RX_BUFF``` to be user-defined
* added ```_SS_TX_ONLY``` user define to allow this to be used as a transmit only library (no interrupts used)

### License

I do not claim copyright on the code, license taken from SoftwareSerial.h header.

```
SoftwareSerial.h - library for Arduino Primo
Copyright (c) 2016 Arduino. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
```