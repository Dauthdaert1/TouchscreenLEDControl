file(GLOB_RECURSE PAGE_SRCS "pages/src/*.c")
file(GLOB_RECURSE FONTS_SRCS "pages/fonts/*.c")

idf_component_register(SRCS "main.c" "globals.c" ${PAGE_SRCS} ${FONTS_SRCS}
                       INCLUDE_DIRS "." "pages"
                       REQUIRES lvgl display_drivers wifi_manager esp_now_driver)
