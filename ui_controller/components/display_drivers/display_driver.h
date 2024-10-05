#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "lvgl.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/*===============*
   GPIO MAPPINGS
 *===============*/

//LCD display controller Pins
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5
#define PIN_NUM_DC   32
#define PIN_NUM_RST  33
#define PIN_NUM_BL   25

//Touch Controller Pins
#define PIN_TP_SDA   21
#define PIN_TP_SCL   22
#define PIN_TP_INT   26
#define PIN_TP_RST   27



/*========================*
   Configuration Settings
 *========================*/

/*SPI to use for LCD controller*/
#define DISPLAY_SPI     SPI3_HOST

/*I2C to use for touch controller*/
#define TOUCH_I2C       I2C_NUM_1

/*Display Width and Height in pixels*/
#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  240

/* Default: Frame buffers 1/10th the size of screen to reduce ram
 * Multiply by 2 for 16 bit color mapping */
#define BUFFER_SIZE 2*DISPLAY_WIDTH*DISPLAY_HEIGHT/10



/*===========*
   Functions
 *===========*/

/** 
 * @brief Initalizes GPIO and LVGL to work with display for display and touch controller.
 * @note lv_init() must be called before calling this
 * @note For GC9A01 LCD Controller and CST816S Touch Controller
*/
void init_display(SemaphoreHandle_t * mutex);

/**
 * @brief Control brightness of display with PWM wave
 * @param brightness Acceptable values are 0 - 100 inclusive
 */
void set_brightness(uint8_t brightness);

#endif