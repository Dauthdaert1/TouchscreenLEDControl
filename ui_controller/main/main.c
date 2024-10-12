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
#include "wifi_manager.h"
#include "weather_info.h"

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

    wifi_manager_init();
    wifi_manager_connect();
    espnow_init();

    //esp_now_drivers_init();

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    while (true)
    {
        
        fetch_weather_data();
        weather_data_t* weather_data = get_weather_data();
        weather_labels_t* weather_labels = get_weather_labels();
        lv_label_set_text_fmt(weather_labels->temp, "%d", (int)weather_data->temp);
        lv_label_set_text_fmt(weather_labels->feels, "%d", (int)weather_data->feels);
        lv_label_set_text_fmt(weather_labels->high, "%d", (int)weather_data->high);
        lv_label_set_text_fmt(weather_labels->low, "%d", (int)weather_data->low);
        lv_label_set_text_fmt(weather_labels->humidity, "%d%%", weather_data->humidity);
        lv_label_set_text_fmt(weather_labels->dew_point, "%d", (int)weather_data->dew_point);
        lv_label_set_text_fmt(weather_labels->wind_speed, "%.1fm/s", weather_data->wind_speed);
        lv_label_set_text_fmt(weather_labels->wind_direction, "%d", weather_data->wind_direction);
        vTaskDelay(600000 / portTICK_PERIOD_MS);
        
       vTaskDelay(600000 / portTICK_PERIOD_MS);
    }
}