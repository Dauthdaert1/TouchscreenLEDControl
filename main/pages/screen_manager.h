#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "lvgl.h"

#define MAX_SCREENS 20

/**
 * @brief Pops top screen from stack
 * @note Doesn't pop if bottom screen
 */
void pop_screen();

/**
 * @brief Pushes screen to stack
 */
void push_screen(lv_obj_t* scr);

/**
 * @brief Clears the stack
 * @note Must set active screen before calling because memory of everything will be freed
 */
void clear_screens();


#endif