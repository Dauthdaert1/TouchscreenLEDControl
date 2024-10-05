# Display Drivers
## Overview

These drivers were programmed to port LVGL to the ESP32 platform with the following components:

* **LCD Display:** 1.28inch 240x240 Round Display with GC9A01 Driver Chip.
* **Touch Controller:** CST816S Capacitive Touch Control Chip.

It is not designed for any others and may cause bad behavior.

## Configure
To configure the PINs and other settings, can be changed in [display_driver.h](display_driver.h).

## How to Use
The following commands must be ran to use.

```c
//Must be called to initalize LVGL before anything else can run
lv_init();

//Must have a mutex stored somewhere and passed
//This ensures that only one process is doing LVGL stuff across different cores.
SemaphoreHandle_t lvgl_mutex = NULL; //Must keep in memory
lvgl_mutex = xSemaphoreCreateMutex();

init_display(&lvgl_mutex);
```

Once the code above is ran, LVGL should be configured to output to the display and take input from the touch screen.

This implementation only works with one screen. There are no plans to make it support multiple.

The display renders and updates on a process on the second core (core 1). I would recommend trying to do additional functionality on core 0, but it shouldn't break, just slow rendering.

User must check out the mutex to run any LVGL commands to ensure it waits until the end of the LVGL rendering cycle. Example blocking wait code for this is below.
```c
while(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) !=pdTRUE);
//Run LVGL Functions
xSemaphoreGive(lvgl_mutex);
```

There is also the static function that takes a value 0-100 and sets the brightness using PWM waves. The mutex does not need to be checked out for this.
```c
set_brightness(uint8_t brightness)
```

