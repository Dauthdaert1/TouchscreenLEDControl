#include <stdio.h>
#include <string.h>
#include "display_driver.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "lvgl.h"

/**
 * @brief Used for initalization commands
 */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //Bytes of data
} lcd_init_cmd_t;

/*=======================
   File Static Variables  
  =======================*/
static const char *TAG = "DISPLAY_DRIVER";
static spi_device_handle_t spi;
static i2c_master_bus_handle_t i2c;
static i2c_master_dev_handle_t i2c_dev;
static lv_display_t * display;
static uint8_t queue_cnt;
static SemaphoreHandle_t * lvgl_mutex;



/*==============================
   Static Function Declarations  
  ==============================*/
static void display_flush_cb(lv_display_t * display, const lv_area_t * area, unsigned char * px_map);
static void rgb_to_bgr565(void * buf, uint32_t buf_size_px);
static void my_input_read();
static uint32_t my_get_millis();
static void lvgl_timer_task(void *arg);
static void start_lvgl_timer();
static void lvgl_timer_init(void *pvParameter);
static void spi_send_cmd(uint8_t cmd);
static void spi_send_data(void * data, uint32_t size);
static void disp_spi_send_data(void * data, uint32_t size);
static void spi_send_data_async(void * data, uint32_t size);
static void spi_wait_queued();



/*=============================
   Public Function Definitions
  =============================*/
void init_display(SemaphoreHandle_t * mutex){
    lvgl_mutex = mutex;
    esp_err_t ret;



    /*====================================
       Configure GPIO, SPI, I2C, and LEDC 
      ====================================*/

    // Configure SPI
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,              //Pin is there, but unneeded so ignored
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = BUFFER_SIZE,
    };

    ret = spi_bus_initialize(DISPLAY_SPI, &buscfg, 1);
    ESP_ERROR_CHECK(ret);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 80 * 1000 * 1000, // Clock out at 80 MHz
        .mode = 0,                          // SPI mode 0
        .spics_io_num = PIN_NUM_CS,         // CS pin
        .queue_size = 7,                    // Transaction queue size
        .pre_cb = NULL,                     
    };

    ret = spi_bus_add_device(DISPLAY_SPI, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    //Configure GPIO
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;       // Disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;             // Set as output mode
    io_conf.pin_bit_mask = (1ULL << PIN_NUM_DC | 1ULL << PIN_NUM_RST | 1ULL << PIN_TP_RST);  // Bit mask of the pins to set
    io_conf.pull_down_en = 0;                    // Disable pull-down
    io_conf.pull_up_en = 0;                      // Disable pull-up
    gpio_config(&io_conf);

    //Configure I2C
    i2c_master_bus_config_t conf = {
        .i2c_port = -1,
        .sda_io_num = PIN_TP_SDA,
        .scl_io_num = PIN_TP_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7, 
    };

    i2c_device_config_t dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x15,
        .scl_speed_hz = 400000,
    };

    esp_err_t err = i2c_new_master_bus(&conf, &i2c);
    ESP_ERROR_CHECK(err);

    err = i2c_master_bus_add_device(i2c, &dev_conf, &i2c_dev);
    ESP_ERROR_CHECK(err);

    //Configure LEDC
    ledc_timer_config_t tim_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ESP_ERROR_CHECK(ledc_timer_config(&tim_config));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE, // Low speed mode
        .channel        = LEDC_CHANNEL_0,      // Channel 0
        .timer_sel      = LEDC_TIMER_0,        // Use timer 0
        .intr_type      = LEDC_INTR_DISABLE,   // Disable interrupt
        .gpio_num       = PIN_NUM_BL,         // GPIO pin number
        .duty           = 0,                   // Initial duty cycle (0%)
        .hpoint         = 0                    // Hpoint value (0)
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    

    /*=======================
       Display Initalization
      =======================*/

    //Reset Display
    gpio_set_level(PIN_NUM_RST, 0);
    gpio_set_level(PIN_TP_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    gpio_set_level(PIN_TP_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //LCD Initalization Commands from: 
    //https://github.com/lvgl/lvgl_esp32_drivers/blob/9fed1cc47b5a45fec6bae08b55d2147d3b50260c/lvgl_tft/GC9A01.c
    lcd_init_cmd_t GC_init_cmds[]={
		{0xEF, {0}, 0},
		{0xEB, {0x14}, 1},

		{0xFE, {0}, 0},
		{0xEF, {0}, 0},

		{0xEB, {0x14}, 1},
		{0x84, {0x40}, 1},
		{0x85, {0xFF}, 1},
		{0x86, {0xFF}, 1},
		{0x87, {0xFF}, 1},
		{0x88, {0x0A}, 1},
		{0x89, {0x21}, 1},
		{0x8A, {0x00}, 1},
		{0x8B, {0x80}, 1},
		{0x8C, {0x01}, 1},
		{0x8D, {0x01}, 1},
		{0x8E, {0xFF}, 1},
		{0x8F, {0xFF}, 1},
		{0xB6, {0x00, 0x20}, 2},
		//call orientation
		{0x3A, {0x05}, 1},
		{0x90, {0x08, 0x08, 0X08, 0X08}, 4},
		{0xBD, {0x06}, 1},
		{0xBC, {0x00}, 1},
		{0xFF, {0x60, 0x01, 0x04}, 3},
		{0xC3, {0x13}, 1},
		{0xC4, {0x13}, 1},
		{0xC9, {0x22}, 1},
		{0xBE, {0x11}, 1},
		{0xE1, {0x10, 0x0E}, 2},
		{0xDF, {0x21, 0x0C, 0x02}, 3},
		{0xF0, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}, 6},
		{0xF1, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}, 6},
		{0xF2, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}, 6},
		{0xF3, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}, 6},
		{0xED, {0x1B, 0x0B}, 2},
		{0xAE, {0x77}, 1},
		{0xCD, {0x63}, 1},
		{0x70, {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0X08, 0x03}, 9},
		{0xE8, {0x34}, 1},
		{0x62, {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0X0F, 0x71, 0xEF, 0x70, 0x70}, 12},
		{0x63, {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0X13, 0x71, 0xF3, 0x70, 0x70}, 12},
		{0x64, {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07}, 7},
		{0x66, {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0X00, 0x00, 0x00}, 10},
		{0x67, {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0X10, 0x32, 0x98}, 10},
		{0x74, {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00}, 7},
		{0x98, {0x3E, 0x07}, 2},
		{0x35, {0}, 0},
		{0x21, {0}, 0},
		{0x11, {0}, 0x80},	//0x80 delay flag
		{0x29, {0}, 0x80},	//0x80 delay flag
        {0x36, {0x68}, 4},  // Set orientation (0x08, 0xC8, 0x68, 0xA8 for "PORTRAIT", "PORTRAIT_INVERTED", "LANDSCAPE", "LANDSCAPE_INVERTED")
		{0, {0}, 0xff},		//init end flag
	};

    //Send init commands
    int curr_cmd = 0;
    while(GC_init_cmds[curr_cmd].databytes!=0xff){
        spi_send_cmd(GC_init_cmds[curr_cmd].cmd);

        if (GC_init_cmds[curr_cmd].databytes > 0) {
            spi_send_data(&GC_init_cmds[curr_cmd].data, GC_init_cmds[curr_cmd].databytes);
        }

        //For commands that need delay
        if (GC_init_cmds[curr_cmd].databytes & 0x80) {
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}
        curr_cmd++;
    }



    /*================
       Configure LVGL
      ================*/

    //Lock Mutex
    while(xSemaphoreTake(*lvgl_mutex, portMAX_DELAY) !=pdTRUE);

    display = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    lv_display_set_flush_cb(display, display_flush_cb);

    void* dma_buffer1 = (void*) heap_caps_aligned_alloc(32, BUFFER_SIZE, MALLOC_CAP_DMA);
    void* dma_buffer2 = (void*) heap_caps_aligned_alloc(32, BUFFER_SIZE, MALLOC_CAP_DMA);

    lv_display_set_buffers(display, dma_buffer1, dma_buffer2, BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    //Set Timing callback
    lv_tick_set_cb(my_get_millis);

    //Register Input device
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_input_read);

    //Start LVGL on core 1
    xTaskCreatePinnedToCore(lvgl_timer_init, "lvgl_timer_init", 32000, NULL, configMAX_PRIORITIES - 1, NULL, 1);

    xSemaphoreGive(*lvgl_mutex);    
}

void set_brightness(uint8_t brightness){
    //Ensure brightness value is in range
    if(brightness>100){
        ESP_LOGE(TAG, "Invalid brightness value: %u", brightness);
    }

    // Set duty cycle (8192 is 13 bit resolution)
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8192*(brightness/100.0)));
    // Update duty cycle
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

}



/*=============================
   Static Function Definitions  
  =============================*/

/**
 * @brief Implementation of function to print the buffer in px_map to the display to be called by LVGL
 * @note Following implementation in: https://docs.lvgl.io/master/porting/display.html#flush-cb
 * @param display The display being outputted to
 * @param area The area of the display being written to
 * @param px_map The buffer to write to LCD
 */
static void display_flush_cb(lv_display_t * display, const lv_area_t * area, unsigned char * px_map){
    //Set columns
    spi_send_cmd(0x2A);
    uint8_t data_x[4] = {0, area->x1, 0, area->x2};     //Beware Little Endian
    spi_send_data(data_x, 4);

    //Set rows
    spi_send_cmd(0x2B);
    uint8_t data_y[4] = {0, area->y1, 0, area->y2};
    spi_send_data(data_y, 4);

    uint32_t size = lv_area_get_size(area);
    //Reconfigure buffer for correct printing
    rgb_to_bgr565(px_map, size);

    //Flush buffer into LCD
    spi_send_cmd(0x2C);
    spi_send_data_async((void*)px_map, size*2);
    

    //Let LVGL know flush is starting
    lv_display_flush_ready(display);
}

/**
 * @brief Convert to bgr565 and swap endianness for display
 * @note rrrrrggggggbbbbb -> gggrrrrrbbbbbggg
 * @param buf Buffer to modify
 * @param buf_size_px The buffer size in pixels
 */
static void rgb_to_bgr565(void * buf, uint32_t buf_size_px){
    uint32_t u32_cnt = buf_size_px / 2;
    uint32_t * buf32 = buf;

    while(u32_cnt >= 8) {
        buf32[0] = ((buf32[0] & 0xf800f800) >> 3) | ((buf32[0] & 0x07000700) >> 8) | ((buf32[0] & 0x00e000e0) << 8) | ((buf32[0] & 0x001f001f) << 3);
        buf32[1] = ((buf32[1] & 0xf800f800) >> 3) | ((buf32[1] & 0x07000700) >> 8) | ((buf32[1] & 0x00e000e0) << 8) | ((buf32[1] & 0x001f001f) << 3);
        buf32[2] = ((buf32[2] & 0xf800f800) >> 3) | ((buf32[2] & 0x07000700) >> 8) | ((buf32[2] & 0x00e000e0) << 8) | ((buf32[2] & 0x001f001f) << 3);
        buf32[3] = ((buf32[3] & 0xf800f800) >> 3) | ((buf32[3] & 0x07000700) >> 8) | ((buf32[3] & 0x00e000e0) << 8) | ((buf32[3] & 0x001f001f) << 3);
        buf32[4] = ((buf32[4] & 0xf800f800) >> 3) | ((buf32[4] & 0x07000700) >> 8) | ((buf32[4] & 0x00e000e0) << 8) | ((buf32[4] & 0x001f001f) << 3);
        buf32[5] = ((buf32[5] & 0xf800f800) >> 3) | ((buf32[5] & 0x07000700) >> 8) | ((buf32[5] & 0x00e000e0) << 8) | ((buf32[5] & 0x001f001f) << 3);
        buf32[6] = ((buf32[6] & 0xf800f800) >> 3) | ((buf32[6] & 0x07000700) >> 8) | ((buf32[6] & 0x00e000e0) << 8) | ((buf32[6] & 0x001f001f) << 3);
        buf32[7] = ((buf32[7] & 0xf800f800) >> 3) | ((buf32[7] & 0x07000700) >> 8) | ((buf32[7] & 0x00e000e0) << 8) | ((buf32[7] & 0x001f001f) << 3);
        buf32 += 8;
        u32_cnt -= 8;
    }

    while(u32_cnt) {
        *buf32 = ((*buf32 & 0xf800f800) >> 3) | ((*buf32 & 0x07000700) >> 8) | ((*buf32 & 0x00e000e0) << 8) | ((*buf32 & 0x001f001f) << 3);
        buf32++;
        u32_cnt--;
    }

    if(buf_size_px & 0x1) {
        //Buffer is always large enough that this is not an issue
        *buf32 = ((*buf32 & 0xf800f800) >> 3) | ((*buf32 & 0x07000700) >> 8) | ((*buf32 & 0x00e000e0) << 8) | ((*buf32 & 0x001f001f) << 3);
    }
}

/**
 * @brief Implementation of callback function to read input
 * @note https://docs.lvgl.io/master/porting/indev.html
 * @param indev Input device pointer
 * @param data Data to set from touch controller
 */
static void my_input_read(lv_indev_t * indev, lv_indev_data_t * data){
    uint8_t r_buf = 0x01;
    uint8_t result[6] = {};
    esp_err_t err = i2c_master_transmit_receive(i2c_dev, &r_buf, 1, result, 6, 1000);
    ESP_ERROR_CHECK(err);

    uint16_t x = result[3];
    uint16_t y = result[5];

    if(result[1]) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

/**
 * @brief Wrapper for callback to get system time in LVGL
 * @returns System time in milliseconds
 */
static uint32_t my_get_millis(){
    return (uint32_t) (esp_timer_get_time() / 1000);
}

/**
 * @brief Wrapper for function to periodically interrupt with mutex locking
 */
static void lvgl_timer_task(void *arg)
{
    while(xSemaphoreTake(*lvgl_mutex, portMAX_DELAY) !=pdTRUE);
    lv_timer_handler();  // Run the LVGL task handler
    xSemaphoreGive(*lvgl_mutex);
}

/**
 * @brief Starts timer to periodically call function
 */
static void start_lvgl_timer()
{
    const esp_timer_create_args_t lvgl_timer_args = {
        .callback = &lvgl_timer_task,
        .arg = NULL,
        .name = "lvgl_timer",
        .dispatch_method = ESP_TIMER_TASK,  // Run in a high-priority task
        .skip_unhandled_events = false,
    };

    esp_timer_handle_t lvgl_timer;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_timer_args, &lvgl_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_timer, 5 * 1000));  // 5 milliseconds
}

/**
 * @brief Task to call on second core and deletes itself after setting timer
 */
void lvgl_timer_init(void *pvParameter)
{
    start_lvgl_timer();
    vTaskDelete(NULL);  // Delete this task once the timer is configured
}

/**
 * @brief Sets DC signal to command mode and sends the command over SPI
 * @param cmd Command (1 byte) to send
 */
static void spi_send_cmd(uint8_t cmd){
    spi_wait_queued();
    gpio_set_level(PIN_NUM_DC, 0);
    disp_spi_send_data(&cmd, 1);
}

/**
 * @brief Sets DC signal to data mode and sends the data over SPI
 * @param data Pointer to data to send
 * @param size Amount of data to send
 */
static void spi_send_data(void * data, uint32_t size){
    spi_wait_queued();
    gpio_set_level(PIN_NUM_DC, 1);
    disp_spi_send_data(data, size);
}

/**
 * @brief Sends specified amount of bytes over SPI to LCD
 * @param data Pointer to data to send
 * @param size Amount of data to send
 */
static void disp_spi_send_data(void * data, uint32_t size){
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       // Zero out the transaction
    t.length = size * 8;            // Data length in bits
    t.tx_buffer = data;             // The data to be sent
    t.user = (void*)1;              // D/C needs to be set to 1

    // Send the data using DMA
    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
}

/**
 * @brief Sends data asynchronously with DMA for parallel flushing and rendering
 * @param data Pointer to data to send
 * @param size Amount of data to send
 */
static void spi_send_data_async(void * data, uint32_t size){
    spi_wait_queued();

    /*
    I don't know why this is, but if padded_length is not accounted for,
    when rendering a buffer with an odd amount of pixels inside, the 7th pixel 
    from the end of the buffer can misrender, and does about 50% of the time
    for no appearent reason. I don't know why the 7th pixel misrenders but somehow
    making the tx_buffer slightly longer on odd pixel counts fixes this ¯\_(ツ)_/¯
    */
    size_t padded_length = size + (size % 4);

    gpio_set_level(PIN_NUM_DC, 1);
    spi_transaction_t *t = (spi_transaction_t*) malloc(sizeof(spi_transaction_t));
    memset(t, 0, sizeof(spi_transaction_t));        // Zero out the transaction
    t->length = padded_length * 8;                           // Data length in bits
    t->tx_buffer = data;                            // The data to be sent
    t->user = (void*)1;                             // D/C needs to be set to 1

    // Send the data using DMA
    ESP_ERROR_CHECK(spi_device_queue_trans(spi, t, portMAX_DELAY));
    queue_cnt++;
}

/**
 * @brief Wait for previously queued data and free memory if when done if exists
 * @note Always call to ensure rendering isn't interrupted
 */
static void spi_wait_queued(){
    for (; queue_cnt > 0; queue_cnt--) {
        spi_transaction_t *trans_result;
        esp_err_t ret = spi_device_get_trans_result(spi, &trans_result, portMAX_DELAY);
        if (ret == ESP_OK) {
            free(trans_result);
        }
    }
}