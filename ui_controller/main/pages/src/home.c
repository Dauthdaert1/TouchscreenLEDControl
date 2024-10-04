#include "lvgl.h"
#include "display_driver.h"
#include "mode.h"
#include "globals.h"
#include "screen_manager.h"
#include "home.h"

#include "esp_log.h"

static weather_labels_t weather_labels;

weather_labels_t* get_weather_labels(){
    return &weather_labels;
}

static void snap_event_handler(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t current_x = lv_obj_get_scroll_x(obj);

    if(current_x < 0){
        return;
    }

    int32_t end_x = 0;
    if(current_x%70 < 35){
        end_x = (current_x/70)*70; //Integer division on purpose
    }else{
        end_x = ((current_x/70)+1)*70;
    }

    if(current_x == end_x){
        return;
    }
    
    lv_obj_scroll_to(obj, end_x, 0, LV_ANIM_ON);
}

lv_obj_t* create_home(void){
    lv_obj_t* page = lv_obj_create(NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN);

    
    /* static lv_style_t btn_style_circle;
    lv_style_init(&btn_style_circle);
    lv_style_set_bg_color(&btn_style_circle, lv_color_hex(0xBBBBBB));
    lv_style_set_radius(&btn_style_circle, 200);

    static lv_style_t btn_style_square;
    lv_style_init(&btn_style_square);
    lv_style_set_radius(&btn_style_square, 0);
    lv_style_set_bg_color(&btn_style_square, lv_color_hex(0xBBBBBB)); */
   

    /* Clock Hours/Minutes */
    
    static lv_style_t clock_style;
    lv_style_init(&clock_style);
    lv_style_set_text_color(&clock_style, lv_color_hex(0xa5b3fa));
    lv_style_set_text_font(&clock_style, &montserrat_36_cut);

    lv_obj_t * label1 = lv_label_create(page);
    weather_labels.clock = label1;
    lv_label_set_text(label1, "12:34");
    lv_obj_add_style(label1, &clock_style, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, -10, 85);
   

    /* Clock Seconds and AM/PM */
    static lv_style_t seconds_style;
    lv_style_init(&seconds_style);
    lv_style_set_text_color(&seconds_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&seconds_style, &lv_font_montserrat_14);

    lv_obj_t * label2 = lv_label_create(page);
    weather_labels.seconds = label2;
    lv_label_set_text(label2, "56");
    lv_obj_add_style(label2, &seconds_style, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 50, 75);

    lv_obj_t * label3 = lv_label_create(page);
    weather_labels.am_pm = label3;
    lv_label_set_text(label3, "AM");
    lv_obj_add_style(label3, &seconds_style, 0);
    lv_obj_align(label3, LV_ALIGN_CENTER, 53, 90);

    /* Lines */
    static lv_point_precise_t line_points[] = { {0, 0}, {180, 0} };

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 1);
    lv_style_set_line_color(&style_line, lv_color_hex(0xFFFFFF));
    lv_style_set_line_rounded(&style_line, true);

    lv_obj_t * line1;
    line1 = lv_line_create(page);
    lv_line_set_points(line1, line_points, 2);
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_align(line1, LV_ALIGN_CENTER, 0, 62);

    lv_obj_t * line2;
    line2 = lv_line_create(page);
    lv_line_set_points(line2, line_points, 2);
    lv_obj_add_style(line2, &style_line, 0);
    lv_obj_align(line2, LV_ALIGN_CENTER, 0, -55);

    /* Human Readable Description */
    /* Box Underneath */
    lv_obj_t *box = lv_obj_create(page);
    lv_obj_set_size(box, 190, 20);
    lv_obj_align(box, LV_ALIGN_CENTER, 0, 47);
    lv_obj_set_style_bg_color(box, lv_color_hex(0x4a4a4a), 0);
    lv_obj_set_style_border_width(box, 0, 0);
    lv_obj_set_style_border_color(box, lv_color_hex(0x4a4a4a), 0);

    /* Scrolling Text */
    static lv_style_t description_style;
    lv_style_init(&description_style);
    lv_style_set_text_color(&description_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&description_style, &lv_font_montserrat_18);

    static lv_anim_t animation_template;
    lv_anim_init(&animation_template);
    lv_anim_set_delay(&animation_template, 1000);
    lv_anim_set_repeat_delay(&animation_template, 5000);
    lv_anim_set_repeat_count(&animation_template, LV_ANIM_REPEAT_INFINITE);

    lv_style_set_anim(&description_style, &animation_template);

    lv_obj_t * label4 = lv_label_create(page);
    weather_labels.scrolling_text = label4;
    lv_label_set_long_mode(label4, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(label4, 180);
    lv_label_set_text(label4, "Expect a day of partly cloudy with rain");
    lv_obj_add_style(label4, &description_style, 0);
    lv_obj_align(label4, LV_ALIGN_CENTER, 0, 45);

    /* Temperature */
    static lv_style_t temperature_style;
    lv_style_init(&temperature_style);
    lv_style_set_text_color(&temperature_style, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&temperature_style, &montserrat_48_cut);

    lv_obj_t * label5 = lv_label_create(page);
    weather_labels.temp = label5;
    lv_label_set_text(label5, "69");
    lv_obj_add_style(label5, &temperature_style, 0);
    lv_obj_align(label5, LV_ALIGN_CENTER, -40, -75);

    /* F degree Label */
    static lv_style_t degree_style;
    lv_style_init(&degree_style);
    lv_style_set_text_color(&degree_style, lv_color_hex(0xa5b3fa));
    lv_style_set_text_font(&degree_style, &lv_font_montserrat_18);

    lv_obj_t * label6 = lv_label_create(page);
    lv_label_set_text(label6, "F");
    lv_obj_add_style(label6, &degree_style, 0);
    lv_obj_align(label6, LV_ALIGN_CENTER, -85, -67);

    static lv_style_t degree2_style;
    lv_style_init(&degree2_style);
    lv_style_set_text_color(&degree2_style, lv_color_hex(0xa5b3fa));
    lv_style_set_text_font(&degree2_style, &montserrat_36_cut);

    lv_obj_t * label7 = lv_label_create(page);
    lv_label_set_text(label7, ".");
    lv_obj_add_style(label7, &degree2_style, 0);
    lv_obj_align(label7, LV_ALIGN_CENTER, -92, -80);

    /* TOP Weather Label */
    lv_obj_t * label8 = lv_label_create(page);
    lv_label_set_text(label8, "WEATHER");
    lv_obj_add_style(label8, &seconds_style, 0);
    lv_obj_align(label8, LV_ALIGN_CENTER, 0, -105);

    /* High Label */
    lv_obj_t * label9 = lv_label_create(page);
    lv_label_set_text(label9, "MAX:");
    lv_obj_add_style(label9, &degree_style, 0);
    lv_obj_align(label9, LV_ALIGN_CENTER, 20, -85);

    lv_obj_t * label10 = lv_label_create(page);
    weather_labels.high = label10;
    lv_label_set_text(label10, "80");
    lv_obj_add_style(label10, &degree_style, 0);
    lv_obj_align(label10, LV_ALIGN_CENTER, 60, -85);

    /* Low Label */
    lv_obj_t * label11 = lv_label_create(page);
    lv_label_set_text(label11, "MIN:");
    lv_obj_add_style(label11, &degree_style, 0);
    lv_obj_align(label11, LV_ALIGN_CENTER, 23, -65);

    lv_obj_t * label12 = lv_label_create(page);
    weather_labels.low = label12;
    lv_label_set_text(label12, "50");
    lv_obj_add_style(label12, &degree_style, 0);
    lv_obj_align(label12, LV_ALIGN_CENTER, 60, -65);

    /* Stat Boxes */
    static lv_style_t stat_style;
    lv_style_init(&stat_style);
    lv_style_set_text_color(&stat_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&stat_style, &lv_font_montserrat_18);

    static lv_style_t scrollbar_style;
    lv_style_init(&scrollbar_style);
    lv_style_set_bg_opa(&scrollbar_style, LV_OPA_TRANSP);  // Make scrollbar background transparent
    lv_style_set_border_width(&scrollbar_style, 0);         // Remove border width
    lv_style_set_pad_all(&scrollbar_style, 0); 

    lv_obj_t * cont_row = lv_obj_create(page);
    lv_obj_set_size(cont_row, 240, 70);
    lv_obj_align(cont_row, LV_ALIGN_CENTER, 0, -8);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_border_opa(cont_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(cont_row, LV_OPA_TRANSP, 0);
    lv_obj_set_scroll_dir(cont_row, LV_DIR_HOR);
    lv_obj_set_style_pad_top(cont_row, 3, 0);
    lv_obj_set_style_pad_left(cont_row, 20, 0);
    lv_obj_set_style_pad_right(cont_row, 20, 0);
    lv_obj_set_scrollbar_mode(cont_row, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont_row, snap_event_handler, LV_EVENT_SCROLL_END, NULL);

    //TODO: Try to make boxes into same type of shape instead of each it's own

    /* HUM Box */
    lv_obj_t *box1 = lv_obj_create(cont_row);
    lv_obj_set_size(box1, 60, 60);
    lv_obj_set_style_bg_color(box1, lv_color_hex(0x22304d), 0);
    lv_obj_set_style_border_opa(box1, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(box1, LV_OBJ_FLAG_SCROLLABLE);

    /* HUM Label */
    lv_obj_t * label13 = lv_label_create(box1);
    lv_label_set_text(label13, "HUM");
    lv_obj_add_style(label13, &seconds_style, 0);
    lv_obj_align(label13, LV_ALIGN_CENTER, -2, -15);

    /* HUM Stat */
    lv_obj_t * label14 = lv_label_create(box1);
    weather_labels.humidity = label14;
    lv_label_set_text(label14, "80%");
    lv_obj_add_style(label14, &stat_style, 0);
    lv_obj_align(label14, LV_ALIGN_CENTER, 0, 10);

    /* FEELS Box */
    lv_obj_t *box2 = lv_obj_create(cont_row);
    lv_obj_set_size(box2, 60, 60);
    lv_obj_set_style_bg_color(box2, lv_color_hex(0x22304d), 0);
    lv_obj_set_style_border_opa(box2, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(box2, LV_OBJ_FLAG_SCROLLABLE);

    /* FEELS Label */
    lv_obj_t * label15 = lv_label_create(box2);
    lv_label_set_text(label15, "FEELS");
    lv_obj_add_style(label15, &seconds_style, 0);
    lv_obj_align(label15, LV_ALIGN_CENTER, -2, -15);

    /* FEELS Stat */
    lv_obj_t * label16 = lv_label_create(box2);
    weather_labels.feels = label16;
    lv_label_set_text(label16, "72");
    lv_obj_add_style(label16, &stat_style, 0);
    lv_obj_align(label16, LV_ALIGN_CENTER, 0, 10);

    /* DEW Box */
    lv_obj_t *box3 = lv_obj_create(cont_row);
    lv_obj_set_size(box3, 60, 60);
    lv_obj_set_style_bg_color(box3, lv_color_hex(0x22304d), 0);
    lv_obj_set_style_border_opa(box3, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(box3, LV_OBJ_FLAG_SCROLLABLE);

    /* DEW Label */
    lv_obj_t * label17 = lv_label_create(box3);
    lv_label_set_text(label17, "DEW");
    lv_obj_add_style(label17, &seconds_style, 0);
    lv_obj_align(label17, LV_ALIGN_CENTER, -2, -15);

    /* DEW Stat */
    lv_obj_t * label18 = lv_label_create(box3);
    weather_labels.dew_point = label18;
    lv_label_set_text(label18, "65");
    lv_obj_add_style(label18, &stat_style, 0);
    lv_obj_align(label18, LV_ALIGN_CENTER, 0, 10);

    /* WIND Box */
    lv_obj_t *box4 = lv_obj_create(cont_row);
    lv_obj_set_size(box4, 60, 60);
    lv_obj_set_style_bg_color(box4, lv_color_hex(0x22304d), 0);
    lv_obj_set_style_border_opa(box4, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(box4, LV_OBJ_FLAG_SCROLLABLE);

    /* WIND Label */
    lv_obj_t * label19 = lv_label_create(box4);
    lv_label_set_text(label19, "WIND");
    lv_obj_add_style(label19, &seconds_style, 0);
    lv_obj_align(label19, LV_ALIGN_CENTER, -2, -15);

    /* WIND Stat */
    lv_obj_t * label20 = lv_label_create(box4);
    weather_labels.wind_speed = label20;
    lv_label_set_text(label20, "4m/s");
    lv_obj_add_style(label20, &stat_style, 0);
    lv_obj_align(label20, LV_ALIGN_CENTER, 0, 10);

    /* DIR Box */
    lv_obj_t *box5 = lv_obj_create(cont_row);
    lv_obj_set_size(box5, 60, 60);
    lv_obj_set_style_bg_color(box5, lv_color_hex(0x22304d), 0);
    lv_obj_set_style_border_opa(box5, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(box5, LV_OBJ_FLAG_SCROLLABLE);

    /* DIR Label */
    lv_obj_t * label21 = lv_label_create(box5);
    lv_label_set_text(label21, "DIR");
    lv_obj_add_style(label21, &seconds_style, 0);
    lv_obj_align(label21, LV_ALIGN_CENTER, -2, -15);

    /* DIR Stat */
    lv_obj_t * label22 = lv_label_create(box5);
    weather_labels.wind_direction = label22;
    lv_label_set_text(label22, "NE");
    lv_obj_add_style(label22, &stat_style, 0);
    lv_obj_align(label22, LV_ALIGN_CENTER, 0, 10);


    /* PRIC Box */
    lv_obj_t *box6 = lv_obj_create(cont_row);
    lv_obj_set_size(box6, 60, 60);
    lv_obj_set_style_bg_color(box6, lv_color_hex(0x22304d), 0);
    lv_obj_set_style_border_opa(box6, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(box6, LV_OBJ_FLAG_SCROLLABLE);

    /* PRIC Label */
    lv_obj_t * label23 = lv_label_create(box6);
    lv_label_set_text(label23, "PRIC");
    lv_obj_add_style(label23, &seconds_style, 0);
    lv_obj_align(label23, LV_ALIGN_CENTER, -2, -15);

    /* PRIC Stat */
    lv_obj_t * label24 = lv_label_create(box6);
    weather_labels.precipitation = label24;
    lv_label_set_text(label24, "20%");
    lv_obj_add_style(label24, &stat_style, 0);
    lv_obj_align(label24, LV_ALIGN_CENTER, 0, 10);

    return page;
}