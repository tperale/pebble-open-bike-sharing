#pragma once

#include "pebble.h"

static const GPathInfo INDICATION_ARROW = {
    3,
    (GPoint []) {
        { -8, 20 },
        { 8, 20 },
        { 0, -80 }
    }
};

void win_navigation_init(void);
void win_navigation_show(void);
void win_navigation_update();
void win_navigation_deinit(void);
