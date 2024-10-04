#include "led_driver.h"
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <string.h>
#include <stdbool.h>

/*==========*
   Typedefs
 *==========*/
typedef void (*led_function_t)(void);

/*=======================
   File Static Variables  
  =======================*/
static const char *TAG = "LED_DRIVER";
static SemaphoreHandle_t led_strip_mutex = NULL;
static led_strip_handle_t led_strip;
static led_strip_spi_config_t spi_config = {
    .clk_src = SPI_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
    .flags.with_dma = true, // Using DMA can improve performance and help drive more LEDs
    .spi_bus = STRIP_SPI,   // SPI bus ID
};
static led_display_t display_config;
static led_function_t stored_led_function = NULL;
static uint8_t strip_brightness = 255;
static uint32_t leds = 0;
static uint32_t start_time;
static bool strip_power;
static led_display_t modes[MAX_LIGHT_MODES];
static uint32_t num_modes = 0;
static uint32_t mode_index = -1;


/*==============================
   Static Function Declarations  
  ==============================*/
static uint32_t get_millis();
static void solid_color();
static void color_shift();
static void breathe();
static void rainbow();


/*=============================
   Public Function Definitions
  =============================*/
void init_strip(uint16_t num_leds){
    

    if(led_strip!=NULL){
        led_strip_del(led_strip);
    }else{
        led_strip_mutex = xSemaphoreCreateMutex();
        strip_power = false;
    }

    while(xSemaphoreTake(led_strip_mutex, portMAX_DELAY) !=pdTRUE);

    leds = num_leds;

    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = PIN_NUM_STRIP, // The GPIO that connected to the LED strip's data line
        .max_leds = num_leds, // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of  your LED strip
        .led_model = LED_MODEL_WS2812, // LED strip model
        .flags.invert_out = false, // whether to invert the output signal (useful when your hardware has a level inverter)
    };

    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    led_strip_clear(led_strip);

    xSemaphoreGive(led_strip_mutex);
}


void set_strip(led_display_t* display){
    while(xSemaphoreTake(led_strip_mutex, portMAX_DELAY) !=pdTRUE);

    memcpy(&display_config, display, sizeof(led_display_t));
    switch (display->mode)
    {
    case SOLID_COLOR:
        stored_led_function = solid_color;
        break;
    case COLOR_SHIFT:
        stored_led_function = color_shift;
        break;
    case BREATHE:
        stored_led_function = breathe;
        break;
    case RAINBOW:
        stored_led_function = rainbow;
        break;
    default:
        //Error
        break;
    }

    start_time = get_millis();

    xSemaphoreGive(led_strip_mutex);
}


void set_strip_brightness(uint8_t brightness){
    strip_brightness = brightness;
}


uint8_t get_strip_brightness(){
    return strip_brightness;
}


void set_strip_power(bool power){
    strip_power = power;
}


bool get_strip_power(){
    return strip_power;
}


uint32_t strip_timer_handler(){
    if(stored_led_function == NULL){
        return 0xFFFFFFFF;
    }

    while(xSemaphoreTake(led_strip_mutex, portMAX_DELAY) !=pdTRUE);
    if(strip_power){
        stored_led_function();
    }else{
        led_strip_clear(led_strip);
    }
    xSemaphoreGive(led_strip_mutex);

    //Calculate time until function should be called again
    if(!strip_power || display_config.mode == SOLID_COLOR){
        return 10000; //If solid color or off don't update often
    }

    return (UPDATE_RATE_MS-((get_millis()-start_time)%UPDATE_RATE_MS));
}

void append_mode(led_display_t* display){
    if(num_modes==MAX_LIGHT_MODES){
        ESP_LOGI(TAG, "Unable to add mode as max modes have been reached");
        return;
    }

    memcpy(&(modes[num_modes]), display, sizeof(led_display_t));
    num_modes++;
}

void set_mode(uint32_t index, led_display_t* display){
    if(num_modes == index){
        append_mode(display);
        return;
    }

    if(index > num_modes){
        ESP_LOGI(TAG, "Unable to add mode at index, appending");
        append_mode(display);
        return;
    }

    memcpy(&(modes[index]), display, sizeof(led_display_t));
    num_modes++;
}

void delete_mode(uint32_t index){
    if(index>=num_modes){
        ESP_LOGI(TAG, "Index to delete doesn't exist");
        return;
    }

    num_modes--;

    for(int i = index; i<num_modes; i++){
        memcpy(&(modes[i]), &(modes[i+1]), sizeof(led_display_t));
    }
}


led_display_t* get_mode(uint32_t index){
    return &(modes[index]);
}


void set_mode_index(uint32_t index){
    mode_index = index;
    set_strip(&(modes[index]));
    strip_timer_handler();
}


uint32_t get_mode_index(){ return mode_index; }


uint32_t get_mode_amount(){ return num_modes; }


/*=============================
   Static Function Definitions  
  =============================*/

/**
 * @brief Get system time in ms
 * @returns System time in milliseconds
 */
static uint32_t get_millis(){
    return (uint32_t) (esp_timer_get_time() / 1000);
}

void solid_color(){
    led_color_t out_color = {
        .red = display_config.color1.red * (strip_brightness/255.0),
        .green = display_config.color1.green * (strip_brightness/255.0),
        .blue = display_config.color1.blue * (strip_brightness/255.0),
    };

    for(int i = 0; i<leds; i++){
        led_strip_set_pixel(led_strip, i, out_color.red, out_color.green, out_color.blue);
    }

    led_strip_refresh(led_strip);
}

void color_shift(){

}

void breathe(){

}

void rainbow(){

}