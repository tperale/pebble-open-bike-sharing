#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "pebble.h"

#define KEY_NAME 0
#define KEY_DISTANCE 1
#define KEY_ANGLE 2
#define KEY_FREE_BIKE 3
#define KEY_PARKINGS 4
#define KEY_NUMBER_OF_STATIONS 5

#define KEY_COMMUNICATION 99

#define GET_LOCATION 100
#define GET_STATIONS 101

#define BUFFER_SIZE 32

extern int32_t station_number;
extern Tuple* t_name;
extern Tuple* t_distance;
extern Tuple* t_angle;
extern int32_t t_free_bikes;
extern int32_t t_parkings;

#endif
