#include "lvgl.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_driver.h"
#include "sdkconfig.h"
#include "display_driver.h"
#include "home.h"
#include "globals.h"
#include "screen_manager.h"
#include "brightness.h"

//static const char *TAG = "LED_STRIP";

void app_main(void)
{
    init_globals();
    if (lvgl_mutex == NULL) {
        printf("Mutex creation failed\n");
        return;
    }

    lv_init();
    init_display(&lvgl_mutex);

    //Set Start Screen
    while(xSemaphoreTake(lvgl_mutex, portMAX_DELAY) !=pdTRUE);
    push_screen(create_home());
    xSemaphoreGive(lvgl_mutex);

    set_brightness(100);

    //Start LED Strip with 60 leds
    init_strip(60);

    led_color_t color1 = {
        .red = 255,
        .green = 255,
        .blue = 255,
    };

    led_display_t mode = {
        .mode = SOLID_COLOR,
        .color1 = color1,
    };

    led_color_t color2 = {
        .red = 255,
        .green = 0,
        .blue = 255,
    };

    led_display_t mode2 = {
        .mode = SOLID_COLOR,
        .color1 = color2,
    };

    append_mode(&mode);
    append_mode(&mode2);
    set_mode_index(1);

    while (true)
    {
        uint32_t wait = strip_timer_handler();
        vTaskDelay(wait / portTICK_PERIOD_MS);
    }
}