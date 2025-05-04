#ifndef __BENJI_RESULT_H
#define __BENJI_RESULT_H

#include <stdlib.h>
#include <stdbool.h>

#ifndef BENJI_ERROR_PACKET
    #define BENJI_ERROR_PACKET (struct _BENJI_ERROR_PACKET) { .file_name = __FILE__, .function_name = __func__, .lineno = __LINE__ }
#endif

#ifndef return_if_error
    #define return_if_error(result) if (result->is_error) { \
        result_error_payload_t error = result_unwrap_error(result); \
        \
        return result_error( \
            error.code, \
            error.message, \
            error.location \
        ); \
    }
#endif

#ifndef return_if_error_with_warning
    #define return_if_error_with_warning(result) if (result->is_error) { \
        result_error_payload_t error = result_unwrap_error(result); \
        \
        log_warning(error); \
        \
        return result_error( \
            error.code, \
            error.message, \
            error.location \
        ); \
    }
#endif

typedef struct _BENJI_ERROR_PACKET {
    const char* file_name;
    const char* function_name;
    unsigned long lineno;
} error_packet_t;

typedef struct _BENJI_RESULT_ERROR_PAYLOAD {
    int code;
    const char* message;
    error_packet_t location;
} result_error_payload_t;

typedef struct _BENJI_RESULT {
    bool is_error;

    union _RESULT_PAYLOAD {
        void* value;
        result_error_payload_t error;
    } payload;
} result_t;

result_t* result_init();

result_t* result_success(void* value);
result_t* result_error(int error_code, const char* message, error_packet_t location);

void* result_unwrap_value(result_t* result);
result_error_payload_t result_unwrap_error(result_t* result);

void result_free(result_t* result);

#endif