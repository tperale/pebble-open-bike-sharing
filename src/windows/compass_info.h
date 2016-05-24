#pragma once

#include <pebble.h>

static const GPathInfo INDICATION_ARROW = {
    4,
    (GPoint[]) {
        { -12, 22 },
        { 0, 10 },
        { 12, 22 },
        { 0, -28 }
    }
};

void destroy_compass();
void create_compass(Layer*, GRect);
