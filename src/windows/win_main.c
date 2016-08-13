#include "win_main.h"

static Window* window;

static StatusBarLayer* status_bar;

static TextLayer* s_text_layer_distance;
static char distance_buffer[16];

static char current_name_buffer[32];
static TextLayer* s_text_layer_current_destination;
static char next_name_buffer[32];
static TextLayer* s_text_layer_next_destination;

static void click_config () {
  window_single_click_subscribe(BUTTON_ID_UP, lambda(void, () {
    MEM_STATE("UP");
    if (!current_station) return;
    Station* tmp = Stations_previous(stations, current_station);
    MEM_STATE("UP");
    current_station->free(current_station);
    current_station = tmp->copy(tmp);
    MEM_STATE("UP");
    win_main_update();
  }));

  window_single_click_subscribe(BUTTON_ID_DOWN, lambda(void, () {
    MEM_STATE("DOWN");
    if (!current_station) return;
    Station* tmp = Stations_next(stations, current_station);
    current_station->free(current_station);
    current_station = tmp->copy(tmp);
    win_main_update();
  }));

  window_long_click_subscribe(BUTTON_ID_DOWN, 500, lambda(void, () {
    MEM_STATE("LONG DOWN");
    if (!current_station) return;
    send_request(GET_ADD_STATIONS);
  }), NULL);
}

static void window_load(Window *window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  status_bar = status_bar_layer_create();
  GRect frame = GRect(0, 0, bounds.size.w, STATUS_BAR_LAYER_HEIGHT);
  layer_set_frame(status_bar_layer_get_layer(status_bar), frame);
  layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));

  create_sensors(window_layer, bounds);

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
    destroy_sensors();
}

void win_main_update() {
  if (!stations) {
    WARN("Trying to update layer without any 'Stations'");
    return;
  } else if (!current_station && stations->current) {
    Station* tmp = stations->stations[0];
    current_station = tmp->copy(tmp);
  }

  /* Updating the name of the current station.
   */
  snprintf(current_name_buffer, 32, "• %s", current_station->name);
  text_layer_set_text(
      s_text_layer_current_destination,
      current_name_buffer
  );

  /* Update the info of the current station.
   *  - free_bike
   *  - empty_slots
   */
  update_station_info_with(
      current_station->free_bike,
      current_station->empty_slots
  );

  /* Layer to show the distance from your current position to the station.
   * TODO Put distance in another file.
   */
  if (current_station->distance / 1000 > 3) {
    // If the distance is longer than 3km
    // show the distance in km.
    snprintf(distance_buffer, 16, "%ld km", current_station->distance / 1000);
  } else {
    // Else show it in m.
    snprintf(distance_buffer, 16, "%ld m", current_station->distance);
  }
  text_layer_set_text(
      s_text_layer_distance,
      distance_buffer
  );

  /* Compass info, only need the angle of the station.
   */
  update_sensors_with(current_station->angle);

  /* Update the name of the next station.
   */
  Station* next_station = Stations_next(stations, current_station);
  snprintf(next_name_buffer, 32, "%s", next_station->name);
  text_layer_set_text(
      s_text_layer_next_destination,
      next_name_buffer
  );
}

void win_main_init (void) {
  font_init();
  window = window_create();

  #ifdef PBL_COLOR
  window_set_background_color(window, GColorChromeYellow);
  #endif
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });
  window_stack_push(window, true);
}

void win_main_deinit (void) {
  window_destroy_safe(window);
}
