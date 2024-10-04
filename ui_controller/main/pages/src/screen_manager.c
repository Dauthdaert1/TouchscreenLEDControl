#include "screen_manager.h"
#include "esp_log.h"

static const char *TAG = "SCREEN_MANAGER";

lv_obj_t* screen_stack[MAX_SCREENS];
int stack_top = -1;

void pop_screen(){
    if(stack_top < 1){
        ESP_LOGI(TAG, "Can't pop empty or bottom screen");
        return;
    }

    lv_screen_load(screen_stack[stack_top-1]);

    lv_obj_delete(screen_stack[stack_top]);

    screen_stack[stack_top] = NULL;

    stack_top -= 1;

}

void push_screen(lv_obj_t* scr){
    if(stack_top+1 == MAX_SCREENS){
        ESP_LOGI(TAG, "Can't add more screens");
        return;
    }

    screen_stack[stack_top+1] = scr;

    lv_screen_load(scr);

    stack_top += 1;
}

void clear_screens(){
    for(int i = 0; i < stack_top; i++){
        lv_obj_delete(screen_stack[i]);
        screen_stack[i] = NULL;
    }

    stack_top = -1;
}