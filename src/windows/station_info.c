#include "station_info.h"
#include "../globals.h"

static GBitmap* s_bicycle_bitmap;
static Layer* s_bicycle_bitmap_layer;
static GBitmap* s_parking_bitmap;
static Layer* s_parking_bitmap_layer;

static Layer* s_direction_layer = NULL;
static TextLayer* s_text_layer_free_bike = NULL;
static char free_bike_buffer[8] = {};

static TextLayer* s_text_layer_parking = NULL;
static char parkings_slots_buffer[8] = {};

static void update_parking_layer (Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_context_set_antialiased(ctx, true);
    /* graphics_draw_bitmap_in_rect(ctx, s_parking_bitmap, gbitmap_get_bounds(s_parking_bitmap)); */
    graphics_draw_bitmap_centered(ctx, s_parking_bitmap, GRect(bounds.size.w / 2, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
}

static void update_bicycle_layer (Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_context_set_antialiased(ctx, true);
    /* graphics_draw_bitmap_in_rect(ctx, s_bicycle_bitmap, gbitmap_get_bounds(s_bicycle_bitmap)); */
    graphics_draw_bitmap_centered(ctx, s_bicycle_bitmap, GRect(0, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
}

void update_station_info_with(uint32_t index) {
    uint32_t free_bike = Stations[index].free_bike;
    snprintf(free_bike_buffer, 8, "%ld", free_bike);
    if (free_bike) {
        text_layer_set_background_color(s_text_layer_free_bike, GColorWhite);
    } else {
        text_layer_set_background_color(s_text_layer_free_bike, GColorLightGray);
    }
    text_layer_set_text(
            s_text_layer_free_bike,
            free_bike_buffer);

    uint32_t empty_slots = Stations[index].empty_slots;
    snprintf(parkings_slots_buffer, 8, "%ld", empty_slots);
    if (empty_slots) {
        text_layer_set_background_color(s_text_layer_free_bike, COLOR_FALLBACK(GColorChromeYellow, GColorClear));
    } else {
        text_layer_set_background_color(s_text_layer_parking, COLOR_FALLBACK(GColorRed, GColorLightGray));
    }
    text_layer_set_text(
            s_text_layer_parking,
            parkings_slots_buffer);
}

void destroy_station_info() {
    gbitmap_destroy(s_bicycle_bitmap);
    /* bitmap_layer_destroy(s_bicycle_bitmap_layer); */
    layer_destroy(s_bicycle_bitmap_layer);
    gbitmap_destroy(s_parking_bitmap);
    /* bitmap_layer_destroy(s_parking_bitmap_layer); */
    layer_destroy(s_parking_bitmap_layer);
}

void create_station_info(Layer* window_layer, GRect bounds) {
    GFont s_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
    /* NUMBER OF FREE BIKE IN THE STATION */
    /* ICON : */
    /* s_bicycle_bitmap_layer = bitmap_layer_create(GRect(0, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8)); */
    s_bicycle_bitmap_layer = layer_create(bounds);
    layer_set_update_proc(s_bicycle_bitmap_layer, update_bicycle_layer);
    s_bicycle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BICYCLE_BITMAP);
    /* bitmap_layer_set_bitmap(s_bicycle_bitmap_layer, s_bicycle_bitmap); */
    /* layer_add_child(window_layer, bitmap_layer_get_layer(s_bicycle_bitmap_layer)); */
    layer_add_child(window_layer, s_bicycle_bitmap_layer);
    /* TEXT : */
    s_text_layer_free_bike = text_layer_create(GRect(bounds.size.w / 4, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_font(s_text_layer_free_bike, s_font);
    text_layer_set_text_alignment(s_text_layer_free_bike, GTextAlignmentCenter);
    text_layer_set_background_color(s_text_layer_free_bike, COLOR_FALLBACK(GColorChromeYellow, GColorClear));
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_free_bike));

    /* NUMBER OF PARKING SLOTS */
    /* ICON : */
    /* s_parking_bitmap_layer = bitmap_layer_create(GRect(bounds.size.w / 2, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8)); */
    s_parking_bitmap_layer = layer_create(bounds);
    layer_set_update_proc(s_parking_bitmap_layer, update_parking_layer);

    s_parking_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARKING_BITMAP);
    /* bitmap_layer_set_bitmap(s_parking_bitmap_layer, s_parking_bitmap); */
    /* layer_add_child(window_layer, bitmap_layer_get_layer(s_parking_bitmap_layer)); */
    layer_add_child(window_layer, s_parking_bitmap_layer);
    /* TEXT : */
    s_text_layer_parking = text_layer_create(GRect((3 * bounds.size.w) / 4, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_font(s_text_layer_parking, s_font);
    text_layer_set_text_alignment(s_text_layer_parking, GTextAlignmentCenter);
    text_layer_set_background_color(s_text_layer_parking, COLOR_FALLBACK(GColorChromeYellow, GColorClear));
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_parking));
}
