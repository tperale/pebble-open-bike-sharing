#include "win_navigation.h"
#include "../globals.h"
#include "../libs/pebble-assist.h"

static Window* window;

static GPath* s_arrow;
static GPoint center;

static GBitmap* s_bicycle_bitmap;
static BitmapLayer* s_bicycle_bitmap_layer;
static GBitmap* s_parking_bitmap;
static BitmapLayer* s_parking_bitmap_layer;

static Layer* s_direction_layer;
static TextLayer* s_text_layer_free_bike;
static char text_layer_buffer[8];

static TextLayer* s_text_layer_parking;
static char parkings_slot_buffer[8];

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
  // rotate needle accordingly
  double tmp = ((double) TRIG_MAX_ANGLE) * ((double) Stations[0].angle / 360);
  LOG("COMPASS redraw : %ld and angle %ld (%lf)", heading_data.magnetic_heading, Stations[0].angle, tmp);
  gpath_rotate_to(s_arrow, (heading_data.magnetic_heading + (int) tmp));

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

    /* NUMBER OF FREE BIKE IN THE STATION */
    /* ICON : */
    s_bicycle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BICYCLE_BITMAP);
    s_bicycle_bitmap_layer = bitmap_layer_create(GRect(0, 0, bounds.size.w / 4, bounds.size.h / 7));
    bitmap_layer_set_bitmap(s_bicycle_bitmap_layer, s_bicycle_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_bicycle_bitmap_layer));

    /* TEXT : */
    s_text_layer_free_bike = text_layer_create(GRect(bounds.size.w / 4, 0, bounds.size.w / 2, bounds.size.h / 7));
    text_layer_set_text_alignment(s_text_layer_free_bike, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_free_bike, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_free_bike));

    /* NUMBER OF PARKING SLOTS */
    /* ICON : */
    s_parking_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARKING_BITMAP);
    s_bicycle_bitmap_layer = bitmap_layer_create(GRect(bounds.size.w / 2, 0, (3 * bounds.size.w) / 4, bounds.size.h / 7));
    bitmap_layer_set_bitmap(s_parking_bitmap_layer, s_parking_bitmap);
    /* layer_add_child(window_layer, bitmap_layer_get_layer(s_parking_bitmap_layer)); */
    /* TEXT : */
    s_text_layer_parking = text_layer_create(GRect((3 * bounds.size.w) / 4, 0, bounds.size.w, bounds.size.h / 7));
    text_layer_set_text_alignment(s_text_layer_parking, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_parking, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_parking));
}


static void window_appear(Window *window) {
    compass_service_set_heading_filter(DEG_TO_TRIGANGLE(2));
    compass_service_subscribe(&compass_heading_handler);
}

static void window_disappear(Window *window) {
    compass_service_unsubscribe();
}

static void window_unload(Window *window) {
    layer_destroy(s_direction_layer);

    bitmap_layer_destroy(s_parking_bitmap_layer);
    bitmap_layer_destroy(s_bicycle_bitmap_layer);

    gbitmap_destroy(s_bicycle_bitmap);
    gbitmap_destroy(s_parking_bitmap);
}

void win_navigation_show () {
    window_stack_push(window, true);
}

void win_navigation_update () {
    if (window_stack_get_top_window() != window) {
        return;
    }

    snprintf(text_layer_buffer, 8, "%ld", Stations[0].free_bike);
    text_layer_set_text(s_text_layer_free_bike, text_layer_buffer);

    snprintf(parkings_slot_buffer, 8, "%ld", Stations[0].empty_slots);
    text_layer_set_text(s_text_layer_parking, parkings_slot_buffer);
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
  /* window_set_background_color(window, GColorClear); */
}

void win_navigation_deinit (void) {
  window_destroy(window);
  gpath_destroy(s_arrow);
}
