idf_component_register(SRCS "src/led_driver.c"
                       INCLUDE_DIRS "."
                       REQUIRES espressif__led_strip driver esp_timer)
