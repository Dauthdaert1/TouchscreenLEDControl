idf_component_register(SRCS "wifi_manager.c" "weather_info.c"
                    INCLUDE_DIRS "include" "."
                    REQUIRES driver esp_wifi esp_http_client nvs_flash lwip esp-tls json freertos)
