#pragma once

#include <pebble.h>
#include "./station.h"

//! Struct to contain every single "Station" and facilitate the interaction
//! them.
typedef struct Stations {
    //! Function to free the whole allocated content, including those
    //! in the "stations" array parameter.
    void (*free)(struct Stations*);
    //! Resize the array according to the length pass in argument.
    void (*resize)(struct Stations*, uint32_t);
    //! Add a "Station" to the array. If the array is out of space
    //! it will reallocate the space in a bigger array.
    //! The "Station" added must not already be in the array.
    void (*add)(struct Stations*, Station*);
    //! Put the "Station" in the right place in the array according
    //! to the distance of that "Station" or update one if it already
    //! exist in the array.
    void (*update)(struct Stations*, Station*);
    //! Insert a new "Station" into the array but keep it to his
    //! "current" length.
    void (*insert)(struct Stations*, Station*);
    //! Capacity of the array.
    uint32_t length;
    //! Number of station in the array.
    uint32_t current;
    //! Array of "Statiion" object sorted by distance.
    Station** stations;
} Stations;

Stations* Stations_new (uint32_t);

//! Get the next "Station" according to the current
//! distance of the current_station in argument.
//! Exemple :
//!   If the current_distance of the Station is 100
//!   And you have a list of distance {30, 56, 70, 116, 134},
//!   It will pick the station with the distance of 116.
Station* Stations_next (Stations*, Station*);

//! Get the previous "Station" according to the current
//! distance of the current_station in argument.
//! Exemple :
//!   If the current_distance of the Station is 100
//!   And you have a list of distance {30, 56, 70, 116, 134},
//!   It will pick the station with the distance of 116.
Station* Stations_previous (Stations*, Station*);
