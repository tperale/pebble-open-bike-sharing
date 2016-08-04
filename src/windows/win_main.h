#pragma once

#include <pebble.h>
#include "../fonts.h"
#include "../libs/functionnal.h"
#include "../libs/station.h"
#include "../libs/stations.h"

void win_main_init(void);
void win_main_update_with_index (uint32_t);
void win_main_update(void);
void win_main_deinit(void);
