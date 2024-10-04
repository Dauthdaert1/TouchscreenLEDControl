#ifndef WIFI_MANAGER
#define WIFI_MANAGER

#include <stdbool.h>


void wifi_manager_init();
void wifi_manager_connect();
void wifi_manager_disconnect();
bool wifi_manager_is_connected();

#endif
