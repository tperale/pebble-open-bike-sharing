#include "win_main.h"

#include "station_info.h"
#include "compass_info.h"

#include "../globals.h"
#include "../libs/pebble-assist.h"

static Window* window;

static StatusBarLayer* status_bar;

static TextLayer* s_text_layer_distance;
static char distance_buffer[16];

static char current_name_buffer[32];
static TextLayer* s_text_layer_current_destination;
static TextLayer* s_text_layer_next_destination;

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

    status_bar = status_bar_layer_create();
    GRect frame = GRect(0, 0, bounds.size.w, STATUS_BAR_LAYER_HEIGHT);
    layer_set_frame(status_bar_layer_get_layer(status_bar), frame);
    layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));

    create_compass(window_layer, bounds);

    /* Setting up the layer to write the current destination. */
    s_text_layer_current_destination = text_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, bounds.size.w, bounds.size.h / 8));
    text_layer_set_text_alignment(s_text_layer_current_destination , PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
    text_layer_set_background_color(s_text_layer_current_destination, GColorBlack);
    text_layer_set_text_color(s_text_layer_current_destination, GColorWhite);
    #ifdef PBL_ROUND
    text_layer_set_font(s_text_layer_current_destination, GOTHIC_14);
    text_layer_enable_screen_text_flow_and_paging(s_text_layer_current_destination, 2);
    #endif
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_current_destination));

    station_info_init(window_layer, bounds);

    /* DISTANCE LAYER */
    s_text_layer_distance = text_layer_create(PBL_IF_RECT_ELSE(
                GRect(0, (6 * bounds.size.h) / 8, bounds.size.w / 2, bounds.size.h / 4),
                GRect(bounds.size.w / 4, (5 * bounds.size.h) / 8, bounds.size.w / 2, bounds.size.h / 8)
    ));
    #ifndef PBL_ROUND
    text_layer_set_font(s_text_layer_distance, ROBOTO_21);
    #endif
    text_layer_set_text_alignment(s_text_layer_distance, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
    text_layer_set_background_color(s_text_layer_distance, GColorClear);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_distance));

    /* Setting up the layer to write the next destination. */
    s_text_layer_next_destination = text_layer_create(PBL_IF_RECT_ELSE(
                GRect(0, (7 * bounds.size.h) / 8, bounds.size.w, bounds.size.h / 8),
                GRect(0, (6 * bounds.size.h) / 8, bounds.size.w, bounds.size.h / 4)
    ));
    text_layer_set_text_alignment(s_text_layer_next_destination, PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentCenter));
    text_layer_set_background_color(s_text_layer_next_destination, GColorBlack);
    text_layer_set_text_color(s_text_layer_next_destination, GColorWhite);
    #ifdef PBL_ROUND
    GFont s_font_next_destination = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    text_layer_set_font(s_text_layer_next_destination, s_font_next_destination);
    text_layer_enable_screen_text_flow_and_paging(s_text_layer_next_destination, 5);
    #endif
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer_next_destination));

    window_set_click_config_provider(window, click_config);
}

static void window_unload(Window *window) {
    station_info_deinit();
    destroy_compass();
}

void update_with_index(uint32_t index) {
    if (window_stack_get_top_window() != window) {
        current_index = index;
        return;
    }

    current_index = ((station_number + index) % station_number);
    // Adding the number of station because "index" is unsigned
    // and cannot be equal to -1.

    DEBUG("New current index %ld", current_index);

    if (!Stations) {
        WARN("Trying to update layer without any 'Stations'");
        return;
    }

    snprintf(current_name_buffer, 32, "• %s", Stations[current_index].name);
    text_layer_set_text(
            s_text_layer_current_destination,
            current_name_buffer);

    update_station_info_with(current_index);

    text_layer_set_text(
            s_text_layer_next_destination,
            Stations[(current_index + 1) % station_number].name);

    if (Stations[current_index].distance / 1000 > 3) {
        // If the distance is longer than 3km
        // show the distance in km.
        snprintf(distance_buffer, 16, "%ld km", Stations[current_index].distance / 1000);
    } else {
        // Else show it in m.
        snprintf(distance_buffer, 16, "%ld m", Stations[current_index].distance);
    }
    text_layer_set_text(
            s_text_layer_distance,
            distance_buffer);
}

void win_main_init (void) {
    font_init();
    window = window_create();
    window_set_background_color(window, COLOR_FALLBACK(GColorChromeYellow, GColorClear));
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

    update_with_index(current_index);
}

void win_main_deinit (void) {
    window_destroy_safe(window);
}
