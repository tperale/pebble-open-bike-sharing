#include "compass.h"

static EasyData* data;

static bool compass_needs_calibration (CompassHeadingData heading) {
  return heading.compass_status == CompassStatusDataInvalid;
}

static bool compass_is_calibrating (CompassHeadingData heading) {
  return heading.compass_status == CompassStatusCalibrating;
}

/* static void compass_heading_handler (EasyData* data) { */
static void compass_heading_handler (CompassHeadingData heading) {
  data->heading = heading;

  if (compass_needs_calibration(data->heading)) {
    DEBUG("Compass : Need calibration");
    data->angle = 0;
    data->calibration_handler(data);
  } else if (compass_is_calibrating(data->heading)) {
    DEBUG("Compass : Calibrating");
    data->angle = 0;
    data->calibrating_handler(data);
  } else {
    data->angle = data->heading.magnetic_heading;
    data->heading_changed_handler(data);
  }
}

/* static void accelerometer_data_handler (EasyData* data) { */
static void accelerometer_data_handler (AccelData* accel_data, uint32_t num_samples) {
  data->accel = *accel_data;

  if (compass_is_calibrating(data->heading)) {
    DEBUG("Accel : Calibrating");
    data->angle = 0;
    data->calibrating_handler(data);
  } else {
    // TODO inclination handler.
  }
}

EasyData* setup_compass (
        void (*heading_changed_func)(EasyData*),
        void (*calibration_func)(EasyData*),
        void (*calibrating_func)(EasyData*))
{
  EasyData* d = malloc(sizeof(EasyData));
  *d = (EasyData) {
    .heading_changed_handler = heading_changed_func,
    .calibration_handler = calibration_func,
    .calibrating_handler = calibrating_func,
    .angle = 0,
  };

  data = d;

  /* compass_service_subscribe(lambda (void, (CompassHeadingData heading) { */
  /*   data->heading = heading; */
  /*   compass_heading_handler(data); */
  /* })); */

  compass_service_subscribe(compass_heading_handler);

  accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  /* accel_data_service_subscribe(1, lambda(void, (AccelData* accel_data, uint32_t num_samples) { */
  /*   data->accel = *accel_data; */
  /*   accelerometer_data_handler(data); */
  /* })); */
  accel_data_service_subscribe(1, accelerometer_data_handler);

  return d;
}

void stop_compass_handling () {
  compass_service_unsubscribe();
}
