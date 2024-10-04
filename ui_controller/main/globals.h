#ifndef GLOBALS_H
#define GLOBALS_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Declare mutex handles as extern
extern SemaphoreHandle_t lvgl_mutex;

void init_globals(void);

#endif