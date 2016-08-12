#pragma once

#include <pebble.h>

#define DISABLE_LOGGING false

#if DISABLE_LOGGING
#define LOG(...)
#define DEBUG(...)
#define INFO(...)
#define WARN(...)
#define ERROR(...)
#define MEM_STATE(label)
#else
#define LOG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define INFO(...) app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define WARN(...) app_log(APP_LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(...) app_log(APP_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define MEM_STATE(label) DEBUG("%s (%d bytes FREE and %d bytes USED)", label, heap_bytes_free(), heap_bytes_used())
#endif
