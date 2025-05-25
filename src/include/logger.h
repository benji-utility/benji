#ifndef __BENJI_LOGGER_H
#define __BENJI_LOGGER_H

#include <stdarg.h>

#include "utils.h"
#include "result.h"

#ifndef BENJI_LOG_PREFIX_DEBUG
    #define BENJI_LOG_PREFIX_DEBUG "[DEBUG]"
#endif

#ifndef BENJI_LOG_PREFIX_INFO
    #define BENJI_LOG_PREFIX_INFO "[INFO]"
#endif

#ifndef BENJI_LOG_PREFIX_WARNING
    #define BENJI_LOG_PREFIX_WARNING "[WARNING]"
#endif

#ifndef BENJI_LOG_PREFIX_ERROR
    #define BENJI_LOG_PREFIX_ERROR "[ERROR]"
#endif

typedef enum _BENJI_LOG_LEVEL {
    BENJI_LOG_LEVEL_DEBUG,
    BENJI_LOG_LEVEL_INFO,
    BENJI_LOG_LEVEL_WARNING,
    BENJI_LOG_LEVEL_ERROR
} log_level_t;

void log_message(log_level_t log_level, const char* string, ...);
void log_error_payload(log_level_t log_level, result_error_payload_t payload);

const char* get_prefix_from_log_level(log_level_t log_level);

#endif