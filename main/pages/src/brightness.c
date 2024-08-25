#include "lvgl.h"
#include "display_driver.h"
#include "brightness.h"
#include "globals.h"
#include "led_driver.h"
#include "screen_manager.h"
#include "mode.h"

#include "esp_log.h"

static void power_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * btn = lv_event_get_target(e);
        if(lv_obj_get_state(btn) & LV_STATE_CHECKED){
            set_strip_power(true);
            lv_obj_clear_flag(e->user_data, LV_OBJ_FLAG_HIDDEN);
        }else{
            set_strip_power(false);
            lv_obj_add_flag(e->user_data, LV_OBJ_FLAG_HIDDEN);
        }
        strip_timer_handler();
    }
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);
    uint8_t brightness = lv_arc_get_value(arc);
    set_strip_brightness(brightness);
    strip_timer_handler();
}

static void settings_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        push_screen(create_mode());
    }
    
}

lv_obj_t* create_brightness(void){
    lv_obj_t* page = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x707070), LV_PART_MAIN);

    static lv_style_t style_arc;
    lv_style_init(&style_arc);
    lv_style_set_arc_width(&style_arc, 25);  // Set the arc width (10 pixels in this example)

    lv_obj_t * arc = lv_arc_create(page);
    lv_obj_add_style(arc, &style_arc, LV_PART_MAIN);
    lv_obj_add_style(arc, &style_arc, LV_PART_INDICATOR);
    lv_obj_set_size(arc, 200, 200);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_range(arc, 0, 255);
    lv_arc_set_value(arc, 10);
    lv_arc_set_range(arc, 2, 255);
    lv_obj_center(arc);
    lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    if(!get_strip_power()){
        lv_obj_add_flag(arc, LV_OBJ_FLAG_HIDDEN);
    }
    

    static lv_style_t btn_style_off;
    lv_style_init(&btn_style_off);
    lv_style_set_bg_color(&btn_style_off, lv_color_hex(0xBBBBBB));
    lv_style_set_radius(&btn_style_off, 200);

    static lv_style_t btn_style_on;
    lv_style_init(&btn_style_on);
    lv_style_set_bg_color(&btn_style_on, lv_color_hex(0x00c5ff));

    lv_obj_t * btn1 = lv_button_create(page);
    lv_obj_add_style(btn1, &btn_style_off, 0);
    lv_obj_add_style(btn1, &btn_style_on, LV_STATE_CHECKED);
    lv_obj_set_size(btn1, 90, 90);
    lv_obj_add_event_cb(btn1, power_handler, LV_EVENT_ALL, arc);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_center(btn1);

    
    static lv_style_t text_style;
    lv_style_init(&text_style);
    lv_style_set_text_color(&text_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&text_style, &lv_font_montserrat_48);
    

    lv_obj_t * label1 = lv_label_create(btn1);
    lv_label_set_text(label1, LV_SYMBOL_POWER);
    lv_obj_add_style(label1, &text_style, 0);
    lv_obj_center(label1);

    lv_obj_t * btn2 = lv_button_create(page);
    lv_obj_add_style(btn2, &btn_style_off, 0);
    lv_obj_set_style_bg_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_size(btn2, 50, 50);
    //lv_obj_add_flag(btn2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(btn2, settings_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 85);

    lv_obj_t * label2 = lv_label_create(page);
    lv_label_set_text(label2, LV_SYMBOL_SETTINGS);
    lv_obj_add_style(label2, &text_style, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 85);

    /*Manually update the label for the first time*/
    lv_obj_send_event(arc, LV_EVENT_VALUE_CHANGED, NULL);
    

    return page;
}