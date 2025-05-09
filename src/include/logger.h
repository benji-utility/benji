#ifndef __BENJI_LOGGER_H
#define __BENJI_LOGGER_H

#include <stdarg.h>

#include "utils.h"
#include "result.h"
#include "colors.h"

#define BENJI_LOG_DEBUG "[DEBUG]"
#define BENJI_LOG_INFO "[INFO]"
#define BENJI_LOG_WARNING "[WARNING]"
#define BENJI_LOG_ERROR "[ERROR]"

void log_debug(const char* string, ...);
void log_info(const char* info, ...);
void log_warning(result_error_payload_t error);
void log_warning_info(const char* info, ...);
void log_error(result_error_payload_t error);
void log_error_info(const char* info, ...);

#endif