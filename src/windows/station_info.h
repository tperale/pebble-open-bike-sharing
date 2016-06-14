#pragma once

#include <pebble.h>
#include "../libs/graphics.h"

void update_station_info_with(uint32_t);
void destroy_station_info();
void create_station_info(Layer*, GRect);
