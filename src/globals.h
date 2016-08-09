#pragma once

#include "./libs/station.h"
#include "./libs/stations.h"
#include "./appinfo.h"

#define BUFFER_SIZE 32

extern Stations* stations;
extern Station* current_station;

void send_request (int value);
