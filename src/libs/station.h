#pragma once

#include <pebble.h>

typedef struct {
    char name[32];
    uint32_t empty_slots;
    uint32_t free_bike;
    uint32_t distance;
    int32_t angle;
} s_station;

void free_stations(s_station*);

void add_station_from_dict(s_station*, uint32_t index, DictionaryIterator*);

s_station* create_stations_array(uint32_t);
