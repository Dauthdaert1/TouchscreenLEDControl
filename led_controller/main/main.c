#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_driver.h"
#include "sdkconfig.h"

//static const char *TAG = "LED_STRIP";

void app_main(void)
{
    //Start LED Strip with 60 leds
    init_strip(60);

    set_strip_brightness(5);

    led_color_t color1 = {
        .red = 255,
        .green = 0,
        .blue = 0,
    };

    led_display_t mode = {
        .mode = SOLID_COLOR,
        .color1 = color1,
    };

    led_color_t color2 = {
        .red = 255,
        .green = 0,
        .blue = 0,
    };

    led_display_t mode2 = {
        .mode = SOLID_COLOR,
        .color1 = color2,
    };

    append_mode(&mode);
    append_mode(&mode2);
    set_mode_index(1);

    set_strip_power(LED_STRIP_ON);

    while (true)
    {
        uint32_t wait = strip_timer_handler();
        vTaskDelay(wait / portTICK_PERIOD_MS);
    }
}