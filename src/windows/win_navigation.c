#include "win_navigation.h"
#include "../globals.h"
#include "../libs/pebble-assist.h"

static Window* window;

static GPath* s_arrow;
static GPoint center;

static Layer* s_direction_layer;

/* static Layer* s_text_layer; */

/* static char free_bike_number_buffer[32]; */

/* static void free_bike_update_proc(Layer *layer, GContext *ctx) { */
/*     GRect bounds = layer_get_frame(layer); */

/*     snprintf(free_bike_number_buffer, 32, "%ld free bike.", t_free_bike->value->int32); */
/*     graphics_draw_text(ctx, free_bike_number_buffer, fonts_get_system_font(FONT_KEY_FONT_FALLBACK), */
/*                        GRect(5, 5, bounds.size.w - 10, 100), GTextOverflowModeWordWrap, */
/*                        GTextAlignmentLeft, NULL); */
/* } */

static void compass_heading_handler(CompassHeadingData heading_data) {
  LOG("COMPASS redraw : %ld and angle %ld", heading_data.magnetic_heading, t_angle->value->int32);

  // rotate needle accordingly
  gpath_rotate_to(s_arrow, heading_data.magnetic_heading + (TRIG_MAX_ANGLE * (t_angle->value->int32 / 360)));

  // trigger layer for refresh
  layer_mark_dirty(s_direction_layer);
}

static void direction_update_proc(Layer *layer, GContext *ctx) {
    gpath_draw_filled(ctx, s_arrow);
    gpath_draw_outline(ctx, s_arrow);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    /* s_text_layer = layer_create(bounds); */
    /* layer_set_update_proc(s_text_layer, free_bike_update_proc); */
    /* layer_add_child(window_layer, s_text_layer); */

    center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
    gpath_move_to(s_arrow, center);

    /* Layer for */
    s_direction_layer = layer_create(bounds);
    layer_set_update_proc(s_direction_layer, direction_update_proc);
    layer_add_child(window_layer, s_direction_layer);
}


static void window_appear(Window *window) {
  compass_service_set_heading_filter(DEG_TO_TRIGANGLE(2));
  compass_service_subscribe(&compass_heading_handler);
}

static void window_disappear(Window *window) {
  compass_service_unsubscribe();
}

static void window_unload(Window *window) {
}

void win_navigation_show () {
    window_stack_push(window, true);
}

void win_navigation_update () {
    if (window_stack_get_top_window() != window) {
        return;
    }


    /* layer_mark_dirty(s_direction_layer); */
    /* layer_mark_dirty(s_text_layer); */
}

void win_navigation_init (void) {
  s_arrow = gpath_create(&INDICATION_ARROW);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .appear = window_appear,
    .disappear = window_disappear,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

void win_navigation_deinit (void) {
  window_destroy(window);
}


