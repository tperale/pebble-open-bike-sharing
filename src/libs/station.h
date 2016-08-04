#pragma once

#include <pebble.h>

typedef struct Station {
    void (*free)(struct Station*);
    struct Station* (*copy)(struct Station*);
    char* name;
    uint32_t empty_slots;
    uint32_t free_bike;
    uint32_t distance;
    int32_t angle;
} Station;

Station* Station_new (char*, uint32_t, uint32_t, uint32_t, int32_t);

bool Stations_are_equals(Station*, Station*);
