#include "win_main.h"
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
static char free_bike_buffer[8];

static TextLayer* s_text_layer_parking;
static char parkings_slots_buffer[8];

static TextLayer* s_text_layer_current_destination;
static TextLayer* s_text_layer_next_destination;

uint32_t current_index = 0;

void update_with_index(uint32_t index);

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

    /* Setting up the layer to write the current destination. */
    s_text_layer_current_destination = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_current_destination, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_current_destination, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_current_destination));

    /* Setting up the layer to write the next destination. */
    s_text_layer_next_destination = text_layer_create(GRect(0, (7 * bounds.size.h) / 8, bounds.size.w, bounds.size.h));
    text_layer_set_text_alignment(s_text_layer_next_destination, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_next_destination, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_next_destination));

    center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
    gpath_move_to(s_arrow, center);

    /* NUMBER OF FREE BIKE IN THE STATION */
    /* ICON : */
    /* s_bicycle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BICYCLE_BITMAP); */
    /* s_bicycle_bitmap_layer = bitmap_layer_create(GRect(bounds.size.h / 8, 0, bounds.size.w / 4, ( 2 * bounds.size.h) / 8)); */
    /* bitmap_layer_set_bitmap(s_bicycle_bitmap_layer, s_bicycle_bitmap); */
    /* layer_add_child(window_layer, bitmap_layer_get_layer(s_bicycle_bitmap_layer)); */
    /* TEXT : */
    s_text_layer_free_bike = text_layer_create(GRect(bounds.size.w / 4, bounds.size.h / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_free_bike, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_free_bike, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_free_bike));

    /* NUMBER OF PARKING SLOTS */
    /* ICON : */
    /* s_parking_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARKING_BITMAP); */
    /* s_bicycle_bitmap_layer = bitmap_layer_create(GRect(bounds.size.w / 2, 0, (3 * bounds.size.w) / 4, bounds.size.h / 7)); */
    /* bitmap_layer_set_bitmap(s_parking_bitmap_layer, s_parking_bitmap); */
    /* layer_add_child(window_layer, bitmap_layer_get_layer(s_parking_bitmap_layer)); */
    /* TEXT : */
    s_text_layer_parking = text_layer_create(GRect((3 * bounds.size.w) / 4, bounds.size.h / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_parking, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer_parking, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_parking));

    window_set_click_config_provider(window, click_config);
}

static void window_unload(Window *window) {
  // Destroy GBitmap
}

void update_with_index(uint32_t index) {
    if (window_stack_get_top_window() != window) {
        current_index = index;
        return;
    }

    current_index = (index % station_number);

    DEBUG("New current index %ld", current_index);

    if (Stations) {
        text_layer_set_text(
                s_text_layer_current_destination,
                Stations[index].name);

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

  /* snprintf(station_name_buffer, 32, "%s", Stations[0].name); */
  /* space_info_title[0] = station_name_buffer; */

  /* snprintf(station_number_buffer, 32, "%lu", Stations[0].distance); */
  /* space_info_subtitle[0] = station_number_buffer; */

  /* layer_mark_dirty(menu_layer_get_layer(s_menu_layer)); */
  /* menu_layer_reload_data(s_menu_layer); */
}

void win_main_deinit (void) {
  window_destroy_safe(window);
}
