#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "pebble.h"

#define KEY_NAME 0
#define KEY_DISTANCE 1
#define KEY_ANGLE 2
#define KEY_FREE_BIKE 3

#define KEY_COMMUNICATION 99

#define GET_LOCATION 100
#define GET_STATIONS 101

#define BUFFER_SIZE 32

extern Tuple* t_name;
extern Tuple* t_distance;
extern Tuple* t_angle;
extern Tuple* t_free_bike;

#endif
