#include "./globals.h"

Stations* stations = NULL;
Station* current_station = NULL;

void send_request (int value) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending request : %i", value);
  // Declare the dictionary's iterator
  DictionaryIterator* out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);

  if (result == APP_MSG_OK) {
    dict_write_int(out_iter, KEY_COMMUNICATION, &value, sizeof(int), false);

    // Unsubscribe the current timer to not interfer with the current
    // long "js" job.
    /* tick_timer_service_unsubscribe(); */

    result = app_message_outbox_send();
    if (result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }
}


