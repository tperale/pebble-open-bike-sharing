#pragma once

#include <pebble.h>
#include "station_info.h"
#include "sensors_info.h"
#include "../globals.h"
#include "../fonts.h"
#include "../libs/pebble-assist.h"
#include "../libs/functionnal.h"
#include "../libs/station.h"
#include "../libs/stations.h"
#include "../libs/log.h"

void win_main_init(void);
void win_main_update(void);
void win_main_deinit(void);
