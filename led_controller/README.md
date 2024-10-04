# LED Controller Component
## Overview
The purpose of this section is to drive the LED strip from the microcontroller. This includes the ability to turn the LEDs on/off, change the lighting mode (such as color), and display effects(rainbow, breathing).

## Features
* Supports colors and brightness control.
* Have different lighting effects.
* TODO: Pairing and Communication with LCD touchscreen.

## Hardware
* **Microcontroller:** ESP32-WROOM-32D.
* **LED Strip:** WS2812B Individually Addressable RGB LED Strip.
* **Communication:** 
    * Wifi to connect to APIs.
    * SPI controller behavior with DMA to print to LED strip.

* **Power:** Uses external power supply.\
**NOTE: DO NOT TRY TO POWER OFF DEVBOARD USB, DRAWS TOO MANY AMPS!**

## Software Setup
**Framework:** ESP-IDF\
**Dependencies:**
* espressif__led_strip

## Installation Instructions
**Clone the Repository**

First, clone the main repository with the codebases for both microcontrollers:
```console
git clone https://github.com/Dauthdaert1/TouchscreenLEDControl.git
```

**Set Up the Environment**

Ensure ESP-IDF is installed. This project was build with V5.2.2.

**Open Project in VSCode**

Open project in the led_controller directory:

```console
├── led_controller/
│   ├── src/           # Source code for LED controller
│   └── README.md      # Details specific to LED controller (this file)
└── README.md          # Main project overview
```

**Build the Project**
Either use the ESP-IDF menu on the left, or type into console:
```console
idf.py build
```

**Flash to Microcontroller**
Run the command:
```console
idf.py flash
```

## License

This project is licensed under the MIT License. See the [LICENSE](../LICENSE) file for details.

This project includes the following third-party libraries:
- espressif__led_strip, licensed under the Apache License.