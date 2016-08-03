#include "compass_info.h"

EasyData* compass_data = NULL;

static const GPathInfo INDICATION_ARROW = {
    4,
    (GPoint[]) {
        { -12, 22 },
        { 0, 10 },
        { 12, 22 },
        { 0, -28 }
    }
};

static CompassCalibrationWindow* calibration_window = NULL;

static Layer* s_direction_layer = NULL;
static GPath* s_arrow = NULL;
static GPoint center;

static Layer* window_layer = NULL;
static GRect bounds;

static void heading_data_handler (EasyData* data) {
  if (calibration_window) {
    compass_calibration_window_destroy(calibration_window);
    calibration_window = NULL;
  }

  int32_t current_angle = 0;
  if (Stations) {
    int32_t current_station_angle = DEG_TO_TRIGANGLE(Stations[current_index].angle);
    current_angle = (data->angle + current_station_angle);
    LOG("NEEDLE redraw : \n - angle %ld\n - station_angle %ld\n - result %ld\n - degree %ld",
        data->angle,
        current_station_angle,
        current_angle,
        TRIGANGLE_TO_DEG(current_angle)
    );
  } else {
    current_angle = data->angle;
    LOG("NEEDLE redraw : \n - angle %ld\n - degree %ld", current_angle,
        TRIGANGLE_TO_DEG(current_angle)
    );
  }

  gpath_rotate_to(s_arrow, current_angle);
  layer_mark_dirty(s_direction_layer);
}

static void draw_calibration (EasyData* data) {
  if (calibration_window) {
    if (window_stack_contains_window(compass_calibration_window_get_window(calibration_window))) {
      compass_calibration_window_apply_accel_data(calibration_window, data->accel);
      return;
    }
  } else {
    calibration_window = compass_calibration_window_create();
  }

  compass_calibration_window_apply_accel_data(calibration_window, data->accel);
  window_stack_push(compass_calibration_window_get_window(calibration_window), true);
}

/* @desc : Draw the needle from the base.
 *
 * @param {window_layer} : Layer to draw the needle on.
 * @param {bounds} : Bounds of the window to draw the needle on.
 */
static void draw_needle () {
    /* Needle to show the direction. */
    center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
    s_arrow = gpath_create(&INDICATION_ARROW);
    gpath_move_to(s_arrow, center);

    /* Layer for the needle update. */
    s_direction_layer = layer_create(bounds);
    layer_set_update_proc(s_direction_layer, lambda(void, (Layer* layer, GContext* ctx) {
      gpath_draw_filled(ctx, s_arrow);
    }));
    layer_add_child(window_layer, s_direction_layer);
}

void update_compass () {
  if (compass_data) {
    heading_data_handler(compass_data);
  }
}

void create_compass(Layer* win_layer, GRect b) {
    window_layer = win_layer;
    bounds = b;

    /* s_text_layer_calib_state = text_layer_create(GRect(bounds.size.w / 2, (5 * bounds.size.h) / 8, bounds.size.w / 2, bounds.size.h / 4)); */
    draw_needle();

    compass_data = setup_compass (
            heading_data_handler,
            draw_calibration,
            draw_calibration
    );
}

/* @desc : Code to destroy the compasas.
 */
void destroy_compass() {
    window_layer = NULL;

    compass_service_unsubscribe();

    gpath_destroy(s_arrow);
    s_arrow = NULL;

    layer_destroy(s_direction_layer);
    s_direction_layer = NULL;

    /* if (s_text_layer_calib_state != NULL) { */
        // s_text_layer_calib_state can be not initialized
        // if the compass is already calibrated.
        /* text_layer_destroy(s_text_layer_calib_state); */
    /* } */
}


