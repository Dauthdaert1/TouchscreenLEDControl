# LED Drivers

This is designed to encapsulate the espressif LED driver so that simpler methods that handle setting all the lights can be used, as well as adding additional features such as animations and brightness.

## Functional Description

This component allows for an LED strip to contain different lighting modes that can be switched between. Below is example code to start a basic program.

```c
//Start LED Strip with 60 leds
init_strip(60);

//Sets brightness to 5 (Scale 0-255)
set_strip_brightness(5);

//Create Lighting Mode
led_color_t color1 = {
    .red = 255,
    .green = 0,
    .blue = 0,
};

led_display_t mode = {
    .mode = SOLID_COLOR,
    .color1 = color1,
};

//Register Lighting Mode
append_mode(&mode);

set_mode_index(0);

set_strip_power(LED_STRIP_ON);

while (true)
{
    uint32_t wait = strip_timer_handler();
    vTaskDelay(wait / portTICK_PERIOD_MS);
}
```

In [led_driver.h](led_driver.h) are defines where GPIO pin, STRIP_SPI, Max Refresh Rate, and the Maximum number of modes can be changed.

Lighting modes are copied to a file static variable, so there is no need to continue storing the mode or colors after registering it.

Currently the strip_timer_handler() function returns the time to wait and you must wait that long in the main loop before trying to render it again. This may change to a task based approach later if needed, but I don't need to do that for my project which would just add complexity.

### Lighting Modes
The lighting modes are stored in the led_light_mode_t enum, and are defined as 
* SOLID_COLOR
* COLOR_SHIFT
* BREATHE
* RAINBOW

#### SOLID_COLOR
Only uses mode and color1 in led_display_t, just displays color1 on all LEDs. Default 10 second wait in update time as there is nothing to update.
```c
led_display_t mode = {
    .mode = SOLID_COLOR,    //Mode
    .color1 = color1,       //Color to all LEDs
};
```

#### COLOR_SHIFT
Unimplemented
```c
led_display_t mode = {
    .mode = COLOR_SHIFT,    //Mode
};
```

#### BREATHE
Unimplemented
```c
led_display_t mode = {
    .mode = BREATHE,        //Mode
};
```

#### RAINBOW
Unimplemented
```c
led_display_t mode = {
    .mode = RAINBOW,        //Mode
};
```