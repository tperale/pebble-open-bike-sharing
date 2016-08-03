#pragma once

#include "./libs/station.h"
#include "./libs/compass.h"

#define BUFFER_SIZE 32

extern s_station* Stations;
extern uint32_t station_number;

extern uint32_t current_index;

extern EasyData* compass_data;
