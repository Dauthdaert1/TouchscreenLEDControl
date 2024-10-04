#include <stdio.h>
#include "weather_info.h"
#include "wifi_info.h"
#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_tls.h"
#include <cJSON.h>
#include <math.h>

#define HTTP_RESPONSE_BUFFER_SIZE 1024

static const char *TAG = "OpenWeather";

char *response_data = NULL;
size_t response_len = 0;
bool all_chunks_received = false;

static weather_data_t weather_data;

weather_data_t* get_weather_data(){
    return &weather_data;
}

static float kelvin_to_fahrenheit(float num){
    return (num-273.15)*(9.0/5) + 32;
}

void update_data(const char *json_string)
{
   
    cJSON *root = cJSON_Parse(json_string);
    cJSON *obj = cJSON_GetObjectItemCaseSensitive(root, "main");
    float temp = cJSON_GetObjectItemCaseSensitive(obj, "temp")->valuedouble;
    weather_data.temp = kelvin_to_fahrenheit(temp);
    weather_data.feels = kelvin_to_fahrenheit(cJSON_GetObjectItemCaseSensitive(obj, "feels_like")->valuedouble);
    weather_data.humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity")->valueint;
    weather_data.high = kelvin_to_fahrenheit(cJSON_GetObjectItemCaseSensitive(obj, "temp_max")->valuedouble);
    weather_data.low = kelvin_to_fahrenheit(cJSON_GetObjectItemCaseSensitive(obj, "temp_min")->valuedouble);
    
    float temp_c = temp - 273.15;
    float gamma = log(weather_data.humidity/100.0) + 17.625*temp_c / (243.04+temp_c);
    weather_data.dew_point = 243.04*gamma / (17.625 - gamma);

    cJSON *obj2 = cJSON_GetObjectItemCaseSensitive(root, "wind");
    weather_data.wind_speed = cJSON_GetObjectItemCaseSensitive(obj2, "speed")->valuedouble;
    weather_data.wind_direction = cJSON_GetObjectItemCaseSensitive(obj2, "deg")->valuedouble;

    
    
    cJSON_Delete(root);
    free(response_data);
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            // Resize the buffer to fit the new chunk of data
            response_data = realloc(response_data, response_len + evt->data_len);
            memcpy(response_data + response_len, evt->data, evt->data_len);
            response_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            all_chunks_received = true;
            ESP_LOGI("OpenWeatherAPI", "Received data: %s", response_data);
            update_data(response_data);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void fetch_weather_data(void) {
    char url[256];
    //snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s", "43.0389", "-87.906471", API_KEY);
    snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s,%s&APPID=%s", "Milwaukee", "US", API_KEY);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 200)
        {
            ESP_LOGI(TAG, "Message sent Successfully");
        }
        else
        {
            ESP_LOGI(TAG, "Message sent Failed");
        }
    }
    else
    {
        ESP_LOGI(TAG, "Message sent Failed");
    }
    esp_http_client_cleanup(client);
    //return err;
    ESP_LOGI(TAG, "Weather called");
}