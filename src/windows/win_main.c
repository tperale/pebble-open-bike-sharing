#include "win_main.h"
#include "../globals.h"
#include "../libs/pebble-assist.h"

static Window* window;

static Layer* s_direction_layer;
static GPath* s_arrow;
static GPoint center;

static GBitmap* s_bicycle_bitmap;
static BitmapLayer* s_bicycle_bitmap_layer;
static GBitmap* s_parking_bitmap;
static BitmapLayer* s_parking_bitmap_layer;

static Layer* s_direction_layer;
static TextLayer* s_text_layer_free_bike;
static char free_bike_buffer[8];

static TextLayer* s_text_layer_parking;
static char parkings_slots_buffer[8];

static char current_name_buffer[32];
static TextLayer* s_text_layer_current_destination;
static TextLayer* s_text_layer_next_destination;

uint32_t current_index = 0;

void update_with_index(uint32_t index);

static void direction_handler (CompassHeadingData heading_data) {
    LOG("COMPASS redraw : %ld", heading_data.magnetic_heading);
    gpath_rotate_to(s_arrow, heading_data.magnetic_heading);

    /* if(heading_data.compass_status == CompassStatusDataInvalid) { */
    /* } else if (heading_data.compass_status == CompassStatusCalibrating) { */
    /* } */
    layer_mark_dirty(s_direction_layer);
}
static void direction_update_proc(Layer* layer, GContext* ctx) {
    gpath_draw_filled(ctx, s_arrow);
    gpath_draw_outline(ctx, s_arrow);
}

static void load_next () {
    update_with_index(current_index + 1);
}
static void load_previous () {
    update_with_index(current_index - 1);
}
static void click_config () {
    window_single_click_subscribe(BUTTON_ID_UP, load_previous);
    window_single_click_subscribe(BUTTON_ID_DOWN, load_next);
}

static void window_load(Window *window) {
    Layer* window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    /* Layer for the needle update. */
    s_direction_layer = layer_create(bounds);
    layer_set_update_proc(s_direction_layer, direction_update_proc);
    layer_add_child(window_layer, s_direction_layer);
    /* Needle to show the direction. */
    center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
    s_arrow = gpath_create(&INDICATION_ARROW);
    gpath_move_to(s_arrow, center);

    s_direction_layer = layer_create(bounds);
    layer_set_update_proc(s_direction_layer, direction_update_proc);
    layer_add_child(window_layer, s_direction_layer);

    /* Setting up the layer to write the current destination. */
    s_text_layer_current_destination = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_current_destination, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_current_destination, GColorBlack);
    text_layer_set_text_color(s_text_layer_current_destination, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_current_destination));

    /* Setting up the layer to write the next destination. */
    s_text_layer_next_destination = text_layer_create(GRect(0, (7 * bounds.size.h) / 8, bounds.size.w, bounds.size.h));
    text_layer_set_text_alignment(s_text_layer_next_destination, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_next_destination, GColorBlack);
    text_layer_set_text_color(s_text_layer_next_destination, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_next_destination));

    /* NUMBER OF FREE BIKE IN THE STATION */
    /* ICON : */
    s_bicycle_bitmap_layer = bitmap_layer_create(GRect(0, bounds.size.h / 8, bounds.size.w / 4, bounds.size.h / 8));
    s_bicycle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BICYCLE_BITMAP);
    bitmap_layer_set_bitmap(s_bicycle_bitmap_layer, s_bicycle_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_bicycle_bitmap_layer));
    /* TEXT : */
    s_text_layer_free_bike = text_layer_create(GRect(bounds.size.w / 4, bounds.size.h / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_free_bike, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_free_bike, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_free_bike));

    /* NUMBER OF PARKING SLOTS */
    /* ICON : */
    s_parking_bitmap_layer = bitmap_layer_create(GRect(bounds.size.w / 2, bounds.size.h / 8, bounds.size.w / 4, bounds.size.h / 8));
    s_parking_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARKING_BITMAP);
    bitmap_layer_set_bitmap(s_parking_bitmap_layer, s_parking_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_parking_bitmap_layer));
    /* TEXT : */
    s_text_layer_parking = text_layer_create(GRect((3 * bounds.size.w) / 4, bounds.size.h / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_parking, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_parking, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_parking));

    window_set_click_config_provider(window, click_config);
}

static void window_unload(Window *window) {
  gbitmap_destroy(s_bicycle_bitmap);
  bitmap_layer_destroy(s_bicycle_bitmap_layer);
  gbitmap_destroy(s_parking_bitmap);
  bitmap_layer_destroy(s_parking_bitmap_layer);
}

void update_with_index(uint32_t index) {
    if (window_stack_get_top_window() != window) {
        current_index = index;
        return;
    }

    current_index = (index % station_number);

    DEBUG("New current index %ld", current_index);

    if (Stations) {
        snprintf(current_name_buffer, 32, "• %s", Stations[index].name);
        text_layer_set_text(
                s_text_layer_current_destination,
                current_name_buffer);

        snprintf(free_bike_buffer, 8, "%ld", Stations[index].free_bike);
        text_layer_set_text(
                s_text_layer_free_bike,
                free_bike_buffer);

        snprintf(parkings_slots_buffer, 8, "%ld", Stations[index].empty_slots);
        text_layer_set_text(
                s_text_layer_parking,
                parkings_slots_buffer);

        text_layer_set_text(
                s_text_layer_next_destination,
                Stations[(index + 1) % station_number].name);
    } else {
        WARN("Trying to update layer without any 'Stations'");
    }
}

void win_main_init (void) {
  compass_service_set_heading_filter(DEG_TO_TRIGANGLE(2));
  compass_service_subscribe(&direction_handler);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

void win_main_update (void) {
  if (window_stack_get_top_window() != window) {
      return;
  }

  update_with_index(0);
}

void win_main_deinit (void) {
  window_destroy_safe(window);
}
