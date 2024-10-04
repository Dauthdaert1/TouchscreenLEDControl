# Touchscreen LED Control
## Author: Dauthdaert1
## High Level Description
This is a project to control an LED strip using a touch LCD display. The LCD display has it's own microcontroller plugged in via USB so that it can be placed easily where the user wants it. The LED strip is controlled by it's own microcontroller as well. In the future, they will communicate with each other via bluetooth, but this is currently not implemented as I just split the project.

I have plans to make the system controllable by a mobile app but that's a while off.

---
## Hardware Configurations
LCD display module hardware:
* ESP32-WROOM-32D
* Waveshare 1.28inch Round LCD Display Module with Touch Panel (https://a.co/d/ahuuevx)
* 2x Pullup Resisor for I2C (4.7kΩ to 10kΩ)

![LCD Module Schematic PNG](assets/LCD_Module.png?raw=true "LCD Module")

LED driver hardware:
* ESP32-WROOM-32D
* WS2812B 5V LED strip
* SN74AHCT125N Logic Level Shifter (For 3.3V microcontroller to drive 5V LED strip)
* 5V Power Supply (Amperage Depends on Amount of LEDs to drive, ideally ~60mA per LED)
* 1000 µF Electrolytic Capacitor
* 220Ω Resistor (For Current Limiting)

![LED Module Schematic PNG](assets/LED_Module.png?raw=true "LED Module")

**Note:** *Schematics are my pin configuration, ESP lets you use almost any pins so they just need to be reconfigured in .h files to different pins if wired differently*

## Software Configurations
### IDE:
The environment this is built for is ESP-IDF in VSCode. It includes setup sdkconfig files for this project. If changing environment, be careful to replicate configurations or project may not work properly.

### Dependencies:
* LVGL
* espressif__led_strip

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

This project includes the following third-party libraries:
- LVGL, licensed under the MIT License.
- espressif__led_strip, licensed under the Apache License 2.0.