idf_component_register(SRCS "esp_now_driver.c"
                    INCLUDE_DIRS "include"
                    REQUIRES freertos esp_wifi nvs_flash)
