#include "station_info.h"
#include "../globals.h"

static GBitmap* s_bicycle_bitmap;
static BitmapLayer* s_bicycle_bitmap_layer;

static GBitmap* s_parking_bitmap;
static BitmapLayer* s_parking_bitmap_layer;

static TextLayer* s_text_layer_free_bike = NULL;
static char free_bike_buffer[8] = {};

static TextLayer* s_text_layer_parking = NULL;
static char parkings_slots_buffer[8] = {};

void update_station_info_with(uint32_t index) {
    uint32_t free_bike = Stations[index].free_bike;
    snprintf(free_bike_buffer, 8, "%ld", free_bike);
    text_layer_set_background_color(
            s_text_layer_free_bike,
            free_bike ? COLOR_FALLBACK(GColorChromeYellow, GColorClear) : COLOR_FALLBACK(GColorRed, GColorLightGray)
    );
    text_layer_set_text(
            s_text_layer_free_bike,
            free_bike_buffer
    );

    uint32_t empty_slots = Stations[index].empty_slots;
    snprintf(parkings_slots_buffer, 8, "%ld", empty_slots);
    text_layer_set_background_color(
            s_text_layer_parking,
            empty_slots ? COLOR_FALLBACK(GColorChromeYellow, GColorClear) : COLOR_FALLBACK(GColorRed, GColorLightGray)
    );
    text_layer_set_text(
            s_text_layer_parking,
            parkings_slots_buffer
    );
}

void station_info_deinit () {
    gbitmap_destroy(s_bicycle_bitmap);
    bitmap_layer_destroy(s_bicycle_bitmap_layer);
    text_layer_destroy(s_text_layer_free_bike);

    gbitmap_destroy(s_parking_bitmap);
    bitmap_layer_destroy(s_parking_bitmap_layer);
    text_layer_destroy(s_text_layer_parking);
}

void station_info_init (Layer* window_layer, GRect bounds) {
    /* NUMBER OF FREE BIKE IN THE STATION */
    /* ICON : */
    s_bicycle_bitmap_layer = bitmap_layer_create(GRect(0, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
    s_bicycle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BICYCLE_BITMAP);
    bitmap_layer_set_bitmap(s_bicycle_bitmap_layer, s_bicycle_bitmap);
    bitmap_layer_set_alignment(s_bicycle_bitmap_layer, GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_bicycle_bitmap_layer));
    /* TEXT : */
    s_text_layer_free_bike = text_layer_create(GRect(bounds.size.w / 4, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_font(s_text_layer_free_bike, GOTHIC_18);
    text_layer_set_text_alignment(s_text_layer_free_bike, GTextAlignmentCenter);
    text_layer_set_background_color(s_text_layer_free_bike, COLOR_FALLBACK(GColorChromeYellow, GColorClear));
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_free_bike));

    /* NUMBER OF PARKING SLOTS */
    /* ICON : */
    s_parking_bitmap_layer = bitmap_layer_create(GRect(bounds.size.w / 2, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
    s_parking_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PARKING_BITMAP);
    bitmap_layer_set_bitmap(s_parking_bitmap_layer, s_parking_bitmap);
    bitmap_layer_set_alignment(s_parking_bitmap_layer, GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_parking_bitmap_layer));
    /* TEXT : */
    s_text_layer_parking = text_layer_create(GRect((3 * bounds.size.w) / 4, (2 * bounds.size.h) / 8, bounds.size.w / 4, bounds.size.h / 8));
    text_layer_set_font(s_text_layer_parking, GOTHIC_18);
    text_layer_set_text_alignment(s_text_layer_parking, GTextAlignmentCenter);
    text_layer_set_background_color(s_text_layer_parking, COLOR_FALLBACK(GColorChromeYellow, GColorClear));
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_parking));
}
