# Arduino Core for Nordic Semiconductor nRF5 based boards

[![Build Status](https://travis-ci.org/sandeepmistry/arduino-nRF5.svg?branch=master)](https://travis-ci.org/sandeepmistry/arduino-nRF5)

Program your [Nordic Semiconductor](https://www.nordicsemi.com) nRF51 or nRF52 board using the [Arduino](https://www.arduino.cc) IDE.

Does not require a custom bootloader on the device.

## Supported boards

### nRF52
 * [Plain nRF52 MCU](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF52832)
 * [Nordic Semiconductor nRF52 DK](https://www.nordicsemi.com/eng/Products/Bluetooth-Smart-Bluetooth-low-energy/nRF52-DK)
   * For boards prior to ```2016.9``` (see sticker), the lastest JLink bootloader is required to upload sketches. To upgrade, press the boot/reset button while powering on the board and copy over the latest [bootloader](https://www.nordicsemi.com/eng/nordic/Products/nRF52-DK/nRF5x-OB-JLink-IF/52275).
 * [Shenzhen Taida Century Technology nRF52 low cost development board](https://www.aliexpress.com/item/NRF52832-high-cost-development-board-gold-core-board/32725601299.html)
 * [RedBear Blend 2](https://github.com/redbear/nRF5x#blend-2)
 * [RedBear Nano 2](https://github.com/redbear/nRF5x#ble-nano-2)

### nRF51
 * [Plain nRF51 MCU](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF51822)
 * [BBC micro:bit](https://microbit.org)
 * [Bluz DK](http://bluz.io)
 * Nordic Semiconductor  [nRF51822 Development Kit](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF51822-Development-Kit) + [nRF51422 Development Kit](https://www.nordicsemi.com/eng/Products/ANT/nRF51422-Development-Kit)
  * PCA10000
  * PCA10001, PCA10002, PCA10003, PCA10004 via nRF6310(nRFgo)
 * [Nordic Semiconductor NRF51 Smart Beacon Kit](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF51822-Bluetooth-Smart-Beacon-Kit)
 * [Nordic Semiconductor NRF51 Dongle](http://www.nordicsemi.com/eng/Products/nRF51-Dongle)
 * [OSHChip](http://www.oshchip.org/)
 * [RedBearLab BLE Nano](http://redbearlab.com/blenano/)
 * [RedBearLab nRF51822](http://redbearlab.com/redbearlab-nrf51822/)
 * [Waveshare BLE400](http://www.waveshare.com/wiki/BLE400)
 * [ng-beacon](https://github.com/urish/ng-beacon)
 * [TinyBLE](https://www.seeedstudio.com/Seeed-Tiny-BLE-BLE-%2B-6DOF-Mbed-Platform-p-2268.html)

## Installing

### Board Manager

 1. [Download and install the Arduino IDE](https://www.arduino.cc/en/Main/Software) (At least v1.6.12)
 2. Start the Arduino IDE
 3. Go into Preferences
 4. Add ```https://sandeepmistry.github.io/arduino-nRF5/package_nRF5_boards_index.json``` as an "Additional Board Manager URL"
 5. Open the Boards Manager from the Tools -> Board menu and install "Nordic Semiconductor nRF5 Boards"
 6. Select your nRF5 board from the Tools -> Board menu

__NOTE:__ During installation it takes the Arduino IDE a few minutes to extract the tools after they have been downloaded, please be patient.

#### OS Specific Setup

##### OS X

No additional setup required.

##### Linux

For 64-bit Linux users,  ```libc6:i386```, ```libstdc++6:i386```, ```libncurses5:i386``` and ```libudev1:i386``` need to be installed :
  * ```sudo dpkg --add-architecture i386```
  * ```sudo apt-get update```
  * ```sudo apt-get install libc6:i386 libstdc++6:i386 libncurses5:i386 libudev1:i386```    

#####  Windows

###### Driver Setup for mbed devices
 Download [mbed Windows Serial driver](https://developer.mbed.org/handbook/Windows-serial-configuration#1-download-the-mbed-windows-serial-port)

###### Driver Setup for Segger J-Link

 1. Download [Zadig](http://zadig.akeo.ie)
 2. Plugin Segger J-Link or DK board
 3. Start ```Zadig```
 4. Select ```Options -> List All Devices```
 5. Plug and unplug your device to find what changes, and select the ```Interface 2``` from the device dropdown
 6. Click ```Replace Driver```

__NOTE__: To roll back to the original driver go to: Device Manager -> Right click on device -> Check box for "Delete the driver software for this device" and click Uninstall

### Selecting a SoftDevice
SoftDevices contain the BLE stack and housekeeping, and must be downloaded once before a sketch using BLE can be loaded.
The SD consumes ~5k of Ram + some extra based on actual BLE configuration.
* SoftDevice S110 v8.0.0 supports [Revision](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf51%2Fdita%2Fnrf51%2Fcompatibility_matrix%2FnRF51822_ic_revision_overview.html&cp=3_0_1) 2 and 3 of nRF51 in peripheral role. It is 96k in size.
* SoftDevice S130 v2.0.1 supports Revision 3 of nRF51 in peripheral and central role. It is 108k in size.
* SoftDevice S132 v2.0.1 supports nRF52 in peripheral and central role. It is 112k in size.

### Flashing a SoftDevice

 1. ```cd <SKETCHBOOK>```, where ```<SKETCHBOOK>``` is your Arduino Sketch folder:
  * OS X: ```~/Documents/Arduino```
  * Linux: ```~/Arduino```
  * Windows: ```~/Documents/Arduino```
 2. Create the following directories: ```tools/nRF5FlashSoftDevice/tool/```
 3. Download [nRF5FlashSoftDevice.jar](https://github.com/sandeepmistry/arduino-nRF5/releases/download/tools/nRF5FlashSoftDevice.jar) to ```<SKETCHBOOK>/tools/nRF5FlashSoftDevice/tool/```
 4. Restart the Arduino IDE
 5. Select your nRF board from the Tools -> Board menu
 6. Select a SoftDevice from the Tools -> "SoftDevice: " menu
 7. Select a Programmer (J-Link, ST-Link V2, or CMSIS-DAP) from the Tools -> "Programmer: " menu
 8. Select Tools -> nRF5 Flash SoftDevice
 9. Read license agreement
 10. Click "Accept" to accept license and continue, or "Decline" to decline and abort
 11. If accepted, SoftDevice binary will be flashed to the board

### From git (for core development)

 1. Follow steps from Board Manager section above
 2. ```cd <SKETCHBOOK>```, where ```<SKETCHBOOK>``` is your Arduino Sketch folder:
  * OS X: ```~/Documents/Arduino```
  * Linux: ```~/Arduino```
  * Windows: ```~/Documents/Arduino```
 3. Create a folder named ```hardware```, if it does not exist, and change directories to it
 4. Clone this repo: ```git clone https://github.com/sandeepmistry/arduino-nRF5.git sandeepmistry/nRF5```
 5. Restart the Arduino IDE

## BLE

This Arduino Core does **not** contain any Arduino style API's for BLE functionality. All the relevant Nordic SoftDevice (S110, S130, S132) header files are included build path when a SoftDevice is selected via the `Tools` menu.

### Recommend BLE Libraries

 * [BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral)
   * v0.3.0 and greater, available via the Arduino IDE's library manager.
   * Supports peripheral mode only.

## Low Frequency Clock Source (LFCLKSRC)

If the selected board has an external 32 kHz crystal connected, it will be used as the source for the low frequency clock. Otherwise the internal 32 kHz RC oscillator will be used. The low frequency clock is used by the `delay(ms)` and `millis()` Arduino API's.

The Generic nRF51 and nRF52 board options have an additional menu item under `Tools -> Low Frequency Clock` that allows you to select the low frequency clock source. However, Nordic does not recommend the Synthesized clock, which also has a significant power impact.

## Credits

This core is based on the [Arduino SAMD Core](https://github.com/arduino/ArduinoCore-samd) and licensed under the same [LGPL License](LICENSE)

The following tools are used:

 * [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded) as the compiler
 * A [forked](https://github.com/sandeepmistry/openocd-code-nrf5) version of [OpenOCD](http://openocd.org) to flash sketches
