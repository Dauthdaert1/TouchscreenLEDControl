#include "lvgl.h"
#include "display_driver.h"
#include "mode.h"
#include "globals.h"
#include "screen_manager.h"

#include "esp_log.h"

static void back_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        pop_screen();
    }
}

static void list_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        pop_screen();
    }
    
}

static void up_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    /*
    if(code == LV_EVENT_CLICKED) {
        uint32_t index = get_mode_index();
        uint32_t amt = get_mode_amount();
        if(index==0){
            index = amt-1;
        }else{
            index -= 1;
        }

        set_mode_index(index);
    }
    */
}

static void down_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    /*
    if(code == LV_EVENT_CLICKED) {
        uint32_t index = get_mode_index();
        uint32_t amt = get_mode_amount();

        if((index+1)==amt){
            index = 0;
        }else{
            index += 1;
        }

        set_mode_index(index);
    }
    */
}

lv_obj_t* create_mode(void){
    lv_obj_t* page = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x707070), LV_PART_MAIN);

    static lv_style_t btn_style_circle;
    lv_style_init(&btn_style_circle);
    lv_style_set_bg_color(&btn_style_circle, lv_color_hex(0xBBBBBB));
    lv_style_set_radius(&btn_style_circle, 200);

    static lv_style_t btn_style_square;
    lv_style_init(&btn_style_square);
    lv_style_set_radius(&btn_style_square, 0);
    lv_style_set_bg_color(&btn_style_square, lv_color_hex(0xBBBBBB));

    
    static lv_style_t text_style;
    lv_style_init(&text_style);
    lv_style_set_text_color(&text_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&text_style, &lv_font_montserrat_48);

    /* Back Button */ 
    lv_obj_t * btn1 = lv_button_create(page);
    lv_obj_add_style(btn1, &btn_style_circle, 0);
    lv_obj_set_style_bg_opa(btn1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(btn1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(btn1, LV_OPA_TRANSP, 0);
    lv_obj_set_size(btn1, 50, 50);
    lv_obj_add_event_cb(btn1, back_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -85, 0);

    lv_obj_t * label1 = lv_label_create(page);
    lv_label_set_text(label1, LV_SYMBOL_NEW_LINE);
    lv_obj_add_style(label1, &text_style, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, -85, 0);

    /* List Button */ 
    lv_obj_t * btn2 = lv_button_create(page);
    lv_obj_add_style(btn2, &btn_style_square, 0);
    lv_obj_set_style_bg_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_outline_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_opa(btn2, LV_OPA_TRANSP, 0);
    lv_obj_set_size(btn2, 50, 50);
    lv_obj_add_event_cb(btn2, list_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 85, 0);

    lv_obj_t * label2 = lv_label_create(page);
    lv_label_set_text(label2, LV_SYMBOL_LIST);
    lv_obj_add_style(label2, &text_style, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 85, 0);

    /* Up Button */ 
    lv_obj_t * btn3 = lv_button_create(page);
    lv_obj_add_style(btn3, &btn_style_circle, 0);
    lv_obj_set_size(btn3, 70, 70);
    lv_obj_add_event_cb(btn3, up_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn3, LV_ALIGN_CENTER, 0, -67);

    lv_obj_t * label3 = lv_label_create(page);
    lv_label_set_text(label3, LV_SYMBOL_UP);
    lv_obj_add_style(label3, &text_style, 0);
    lv_obj_align(label3, LV_ALIGN_CENTER, 0, -70);

    /* Down Button */ 
    lv_obj_t * btn4 = lv_button_create(page);
    lv_obj_add_style(btn4, &btn_style_circle, 0);
    lv_obj_set_size(btn4, 70, 70);
    lv_obj_add_event_cb(btn4, down_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn4, LV_ALIGN_CENTER, 0, 67);

    lv_obj_t * label4 = lv_label_create(page);
    lv_label_set_text(label4, LV_SYMBOL_DOWN);
    lv_obj_add_style(label4, &text_style, 0);
    lv_obj_align(label4, LV_ALIGN_CENTER, 0, 70);

    return page;
}