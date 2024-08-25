file(GLOB_RECURSE PAGE_SRCS "pages/src/*.c")

idf_component_register(SRCS "main.c" "globals.c" ${PAGE_SRCS}
                       INCLUDE_DIRS "." "pages"
                       REQUIRES lvgl display_drivers led_drivers)
