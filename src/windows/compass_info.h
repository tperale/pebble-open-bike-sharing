#pragma once

#include <pebble.h>

#include "./win_compass_calibration.h"
#include "../globals.h"
#include "../libs/compass.h"
#include "../libs/functionnal.h"
#include "../libs/pebble-assist.h"

/* @desc : Used to handle the new data from the compass but also
 *      to update the needle direction when you change the current
 *      station displayed.
 */
void update_compass();

void create_compass(Layer*, GRect);
void destroy_compass();
