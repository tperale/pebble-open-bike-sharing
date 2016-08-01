#pragma once

#include <pebble.h>
#include "../fonts.h"
#include "../libs/graphics.h"

void update_station_info_with(uint32_t);
void station_info_deinit();
void station_info_init(Layer*, GRect);
