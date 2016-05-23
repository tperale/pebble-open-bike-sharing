#include "station.h"
#include "./pebble-assist.h"
#include "../appinfo.h"
#include <string.h>

void free_stations(s_station* stations_array) {
    WARN("Freeing the 'stations_array'.");
    free(stations_array);
}

void add_station_from_dict(s_station* stations_array, uint32_t index, DictionaryIterator* iterator) {
    s_station station;
    Tuple* t_name = dict_find(iterator, KEY_NAME);
    if (t_name) {
        strcpy(station.name, (char*) t_name->value->cstring);
        DEBUG("Station name : %s", station.name);
    } else {
        ERROR("Error including a new station, no 'name' key.");
        return;
    }

    Tuple* t_free_bike = dict_find(iterator, KEY_FREE_BIKE);
    if (t_free_bike) {
    station.free_bike = (uint32_t) t_free_bike->value->uint32;
    } else {
        ERROR("Error including a new station, no 'free bike' key.");
        return;
    }

    Tuple* t_empty_slots = dict_find(iterator, KEY_PARKINGS);
    if (t_empty_slots) {
        station.empty_slots = (uint32_t) t_empty_slots->value->uint32;
    } else {
        ERROR("Error including a new station, no 'empty slots' key.");
        return;
    }

    Tuple* t_distance = dict_find(iterator, KEY_DISTANCE);
    if (t_distance) {
        station.distance = (uint32_t) t_distance->value->uint32;
    } else {
        ERROR("Error including a new station, no 'distance' key.");
        return;
    }

    Tuple* t_angle = dict_find(iterator, KEY_ANGLE);
    if (t_angle) {
        station.angle = (int32_t) t_angle->value->int32;
    } else {
        ERROR("Error including a new station, no 'angle' key.");
        return;
    }

    stations_array[index] = station;
}

s_station* create_stations_array(uint32_t number_of_stations) {
    WARN("Allocating a new array %ld", number_of_stations);
    return (s_station*) malloc(number_of_stations * sizeof(s_station));
}
