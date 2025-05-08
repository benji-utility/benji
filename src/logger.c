#include "include/logger.h"

void log_debug(const char* string, ...) {
    va_list arguments;

    va_start(arguments, string);

    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    vsprintf(output, string, arguments);

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);

    va_end(arguments);
}
void log_info(const char* info, ...) {
    va_list arguments;

    va_start(arguments, info);

    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    fprintf(stdout, ANSI_CYAN);
    vsprintf(output, info, arguments);
    fprintf(stdout, ANSI_COLOR_RESET);

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);

    va_end(arguments);
}

void log_warning(result_error_payload_t error) {
    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    sprintf(
        output,
        "[WARNING] %s:%i under %s() -> %s (%i)\n",
        error.location.file_name,
        error.location.lineno,
        error.location.function_name,
        error.message,
        error.code
    );

    fprintf(stderr, output);

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);
}

void log_warning_info(const char* info, ...) {
    va_list arguments;

    va_start(arguments, info);

    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    vsprintf(output, info, arguments);

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);

    va_end(arguments);
}

void log_error(result_error_payload_t error) {
    char* output = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!output) {
        return;
    }

    output[0] = '\0';

    sprintf(
        output,
        "[FATAL] %s:%i under %s() -> %s (%i)\n" ANSI_COLOR_RESET,
        error.location.file_name,
        error.location.lineno,
        error.location.function_name,
        error.message,
        error.code
    );

    fprintf(stderr, output);

    #if defined(_WIN32)
        strtrim(output);
        OutputDebugStringA(output);
    #elif defined (__linux__)
        /* TODO: add linux stuff */
    #endif

    free(output);
}