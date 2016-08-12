#pragma once

#include <pebble.h>
#include "./functionnal.h"
#include "./log.h"

typedef void(*CompassDrawFunction)(CompassHeadingData heading);
typedef void(*ShowCalibrationWin)();
typedef void(*AngleModificationCallback)(int32_t angle);

typedef struct EasyData {
  void (*heading_changed_handler)(struct EasyData*);
  void (*calibration_handler)(struct EasyData*);
  void (*calibrating_handler)(struct EasyData*);
  CompassHeadingData heading;
  int32_t angle;
  AccelData accel;
} EasyData;

EasyData* setup_sensors_handling(
    void (*heading_changed_func)(EasyData*),
    void (*calibration_func)(EasyData*),
    void (*calibrating_func)(EasyData*)
);

void stop_sensors_handling ();
