#include <pebble.h>
#include "./windows/win_main.h"
#include "globals.h"

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
  /* Get the name of the hackerspace. */
  t_name = dict_find(iterator, KEY_NAME);
  t_distance = dict_find(iterator, KEY_DISTANCE);
  t_angle = dict_find(iterator, KEY_ANGLE);
  t_free_bike = dict_find(iterator, KEY_FREE_BIKE);

  win_main_update ();
}



int main(void) {
  win_main_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed main window");

  app_message_register_inbox_received(inbox_connected_person_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  app_event_loop();

  win_main_deinit();
}
