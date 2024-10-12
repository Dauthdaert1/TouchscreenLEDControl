# LCD Controller Component
## Overview
The purpose of this section is to show the UI on the circular touchscreen module and convert that into commands for the LED microcontroller. This includes the ability for users to turn the LEDs on/off, change the lighting mode (such as color), display effects(rainbow, breathing), and make defined modes.

## Features
* Touchscreen-based user interface with LVGL.
* Display Weather information using OpenWeather API.
* TODO: Pairing and Communication with LED strip.
* Supports colors and brightness control.

## Hardware
* **Microcontroller:** ESP32-WROOM-32D.
* **LCD Display:** 1.28inch 240x240 Round Display with GC9A01 Driver Chip.
* **Touch Controller:** CST816S Capacitive Touch Control Chip.
* **Communication:** 
    * Wifi to connect to APIs.
    * SPI to print to display.
    * I2C to read from touchscreen.

* **Power:** Uses development board's onboard Micro-USB port for power.

## Software Setup
**Framework:** ESP-IDF with LVGL\
**Dependencies:**
* LVGL
* Custom LCD Driver for Touch Display (included)

#### Documentation of Custom Drivers
[Driver README](components/display_drivers/README.md)

## Installation Instructions
**Clone the Repository**

First, clone the main repository with the codebases for both microcontrollers:
```console
git clone https://github.com/Dauthdaert1/TouchscreenLEDControl.git
```

**Set Up the Environment**

Ensure ESP-IDF is installed. This project was build with V5.3.1.

**Open Project in VSCode**

Open project in the ui_controller directory:

```console
├── ui_controller/
│   ├── src/           # Source code for UI controller
│   └── README.md      # Details specific to UI controller (this file)
└── README.md          # Main project overview 
```

**Update Template**

In the wifi manager component, copy [wifi_info_template.h](components/wifi_manager/wifi_info_template.h) and fill with your information.

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
- LVGL, licensed under the MIT License.