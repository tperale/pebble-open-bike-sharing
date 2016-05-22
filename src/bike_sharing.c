#include <pebble.h>
#include "./windows/win_main.h"
#include "./appinfo.h"
#include "./globals.h"

static void send_request (int value) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending request : %i", value);
    // Declare the dictionary's iterator
    DictionaryIterator* out_iter;

    // Prepare the outbox buffer for this message
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    if (result == APP_MSG_OK) {
        dict_write_int(out_iter, KEY_COMMUNICATION, &value, sizeof(int), false);

        result = app_message_outbox_send();
        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
        }
    } else {
        // The outbox cannot be used right now
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
    }
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void inbox_connected_person_callback(DictionaryIterator *iterator, void *context) {
  uint32_t station_number = dict_find(iterator, KEY_NUMBER_OF_STATIONS)->value->uint32;
  int32_t index = dict_find(iterator, KEY_INDEX)->value->int32;

  if (!Stations) {
      Stations = create_stations_array(station_number);
  } else {
      // If the stations array has already been allocated.
      // free_stations(Stations);
  }

  add_station_from_dict(Stations, index, iterator);

  win_main_update ();
}

static void second_handler (struct tm *tick_time, TimeUnits units_changed) {
    // send_request(GET_LOCATION);
    // send_request(GET_STATIONS);
}

int main(void) {
  win_main_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed main window");

  app_message_register_inbox_received(inbox_connected_person_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_open(1024, 1024);

  tick_timer_service_subscribe(SECOND_UNIT, second_handler);

  app_event_loop();
  win_main_deinit();
}
