#include "globals.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

SemaphoreHandle_t lvgl_mutex = NULL;

void init_globals(void){
    lvgl_mutex = xSemaphoreCreateMutex();
}