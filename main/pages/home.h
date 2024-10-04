#ifndef HOME_H
#define HOME_H

#include "lvgl.h"

typedef struct{
    lv_obj_t* clock;
    lv_obj_t* seconds;
    lv_obj_t* am_pm;
    lv_obj_t* temp;
    lv_obj_t* high;
    lv_obj_t* low;
    lv_obj_t* feels;
    lv_obj_t* humidity;
    lv_obj_t* dew_point;
    lv_obj_t* wind_speed;
    lv_obj_t* wind_direction;
    lv_obj_t* precipitation;
    lv_obj_t* scrolling_text;
}weather_labels_t;

lv_obj_t* create_home(void);

weather_labels_t* get_weather_labels();

#endif