#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "led_strip.h"
#include <stdbool.h>

/*===============*
   GPIO MAPPINGS
 *===============*/
#define PIN_NUM_STRIP 14


/*========================*
   Configuration Settings
 *========================*/
#define STRIP_SPI       SPI2_HOST
#define UPDATE_RATE_MS  50
#define MAX_LIGHT_MODES 20

/*===========*
   Constants
 *===========*/
#define LED_STRIP_OFF   0
#define LED_STRIP_ON    1

/*==========*
   Typedefs
 *==========*/

/*Lighting Modes*/
typedef enum{
    SOLID_COLOR,
    COLOR_SHIFT,
    BREATHE,
    RAINBOW,
}led_light_mode_t;

/*Struct to Store RGB*/
typedef struct{
    uint32_t red;
    uint32_t green;
    uint32_t blue;
}led_color_t;

/*Struct defining how to display lights*/
typedef struct{
    led_light_mode_t mode;
    led_color_t color1;
    led_color_t color2;
    led_color_t color3;
    uint32_t var1;
    uint32_t var2;
    uint32_t var3;
}led_display_t;


/*===========*
   Functions
 *===========*/

/** 
 * @brief Initalizes LED strip
 * @param num_leds The number of LEDs to use
 * @note Is for WS2812 LED strips, must be modified for others
*/
void init_strip(uint16_t num_leds);

/**
 * @brief Sets output for LED strip for temporary display
 * @param display The led_display_t configuration to display
 */
void set_strip(led_display_t* display);

/**
 * @brief Sets brightness value 0-255
 * @param brightness
 */
void set_strip_brightness(uint8_t brightness);

/**
 * @brief Gets current brightness
 * @returns Brightness value 0-255
 */
uint8_t get_strip_brightness();

/**
 * @brief Sets power
 * @param power True(On), False(Off)
 */
void set_strip_power(bool power);

/**
 * @brief Gets power state
 * @returns bool On(true) or Off(false)
 */
bool get_strip_power();

/**
 * @brief Call periodically to update LEDs
 * @returns Time to wait for next call (in ms)
 */
uint32_t strip_timer_handler();

/**
 * @brief Call to set lighting mode after last index
 * @param display Pointer to display to copy to index
 */
void append_mode(led_display_t* display);

/**
 * @brief Call to set lighting mode at specific index
 * @param index Index of mode
 * @param display Pointer to display to copy to index
 */
void set_mode(uint32_t index, led_display_t* display);

/**
 * @brief Deletes a mode and shifts modes after to positions
 * @param index Index to delete
 */
void delete_mode(uint32_t index);

/**
 * @brief Get pointer to current display mode
 * @param index Index of mode
 * @returns Pointer to current display mode
 */
led_display_t* get_mode(uint32_t index);

/**
 * @brief Set index of mode to display
 * @param index Index of mode
 */
void set_mode_index(uint32_t index);

/**
 * @brief Get index of currently displaying mode
 * @returns Current index of mode (-1 if none)
 */
uint32_t get_mode_index();

/**
 * @brief Get amount of different modes
 * @returns Amount of different modes
 */
uint32_t get_mode_amount();

#endif