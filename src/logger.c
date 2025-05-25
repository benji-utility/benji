#include "include/logger.h"

void log_message(log_level_t log_level, const char* string, ...) {
    va_list arguments;

    va_start(arguments, string);

    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    vsprintf(output, string, arguments);

    const char* log_prefix = get_prefix_from_log_level(log_level);

    // this is dumb but i dont really have a choice
    strprepend(output, " ");
    strprepend(output, log_prefix);

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);

    va_end(arguments);
}

void log_error_payload(log_level_t log_level, result_error_payload_t payload) {
    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    const char* log_prefix = get_prefix_from_log_level(log_level);

    sprintf(
        output,
        "%s %s:%i under %s() -> %s (error code %i)",
        log_prefix,
        payload.location.file_name,
        payload.location.lineno,
        payload.location.function_name,
        payload.message,
        payload.code
    );

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);
}

const char* get_prefix_from_log_level(log_level_t log_level) {
    switch (log_level) {
        case BENJI_LOG_LEVEL_DEBUG: return BENJI_LOG_PREFIX_DEBUG;
        case BENJI_LOG_LEVEL_INFO: return BENJI_LOG_PREFIX_INFO;
        case BENJI_LOG_LEVEL_WARNING: return BENJI_LOG_PREFIX_WARNING;
        case BENJI_LOG_LEVEL_ERROR: return BENJI_LOG_PREFIX_ERROR;
    }
}