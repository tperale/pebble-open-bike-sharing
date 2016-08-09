#include <pebble.h>
#include "./windows/win_main.h"
#include "./appinfo.h"
#include "./globals.h"
#include "./libs/pebble-assist.h"

static void second_handler (struct tm *tick_time, TimeUnits units_changed);

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void inbox_callback(DictionaryIterator *iterator, void *context) {
  switch (dict_find(iterator, KEY_TYPE)->value->uint32) {
    case RESPONSE_CLOSE_STATIONS: {
      if (!stations) {
        // New close stations.
        uint32_t number = dict_find(iterator, KEY_NUMBER_OF_STATIONS)->value->uint32;
        stations = Stations_new(number);
      } else {
        // If the stations array has already been allocated.
        // free_stations(Stations);
      }

      stations->add(stations, Station_new(
          dict_find(iterator, KEY_NAME)->value->cstring,
          dict_find(iterator, KEY_PARKINGS)->value->uint32,
          dict_find(iterator, KEY_FREE_BIKE)->value->uint32,
          dict_find(iterator, KEY_DISTANCE)->value->uint32,
          dict_find(iterator, KEY_ANGLE)->value->uint32
      ));
      break;
    }
    case RESPONSE_UPDATED_STATIONS:
    case RESPONSE_UPDATED_LOCATION: {
      DEBUG("Updating the location.");

      stations->update(stations, Station_new (
        dict_find(iterator, KEY_NAME)->value->cstring,
        dict_find(iterator, KEY_PARKINGS)->value->uint32,
        dict_find(iterator, KEY_FREE_BIKE)->value->uint32,
        dict_find(iterator, KEY_DISTANCE)->value->uint32,
        dict_find(iterator, KEY_ANGLE)->value->uint32
      ));

      // TODO No update if the current station is no longer in the array.

      break;
    }
    case RESPONSE_ADD_STATIONS: {
      DEBUG("Adding stations.");

      stations->add(stations, Station_new (
        dict_find(iterator, KEY_NAME)->value->cstring,
        dict_find(iterator, KEY_PARKINGS)->value->uint32,
        dict_find(iterator, KEY_FREE_BIKE)->value->uint32,
        dict_find(iterator, KEY_DISTANCE)->value->uint32,
        dict_find(iterator, KEY_ANGLE)->value->uint32
      ));
      break;
    }
    case RESPONSE_END: {
      win_main_update ();

      /* Reenable the tick timer to fetch new location. */
      tick_timer_service_subscribe(SECOND_UNIT, second_handler);
      break;
    }
  }
}

static void second_handler (struct tm *tick_time, TimeUnits units_changed) {
    if ((tick_time->tm_sec % 60) == 0) {
        send_request(GET_STATIONS_UPDATE);
    }
    if ((tick_time->tm_sec % 5) == 0) {
        send_request(GET_UPDATED_LOCATION);
    }
}

int main(void) {
  win_main_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed main window");

  app_message_register_inbox_received(inbox_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_open(1024, 1024);

  app_event_loop();
  win_main_deinit();
}
