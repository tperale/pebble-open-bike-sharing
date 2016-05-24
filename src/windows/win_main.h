#ifndef _WIN_MAIN_H
#define _WIN_MAIN_H

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

void win_main_init(void);
void win_main_update_with_index (uint32_t);
void win_main_update(void);
void win_main_deinit(void);

#endif
