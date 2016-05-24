#ifndef _WIN_MAIN_H
#define _WIN_MAIN_H

#include <pebble.h>

void win_main_init(void);
void win_main_update_with_index (uint32_t);
void win_main_update(void);
void win_main_deinit(void);

#endif
