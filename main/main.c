#include "lvgl.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
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

    while (true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}