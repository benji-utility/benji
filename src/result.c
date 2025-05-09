#include "include/result.h"

result_t* result_init() {
    result_t* result = malloc(sizeof(result_t));

    result->is_error = false;

    return result;
}

result_t* result_success(void* value) {
    result_t* result = result_init();

    result->is_error = false;
    result->payload.value = value;

    return result;
}

result_t* result_error(int error_code, error_packet_t location, const char* message, ...) {
    result_t* result = result_init();

    result->is_error = true;

    char* error_message = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!error_message) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    error_message[0] = '\0';

    va_list arguments;

    va_start(arguments, message);

    vsprintf(error_message, message, arguments);

    result->payload.error.code = error_code;
    result->payload.error.message = error_message;
    result->payload.error.location = location;

    va_end(arguments);

    return result;
}

void* result_unwrap_value(result_t* result) {
    void* value = result->payload.value;

    result_free(result);

    return value;
}

result_error_payload_t result_unwrap_error(result_t* result) {
    result_error_payload_t error = result->payload.error;

    result_free(result);

    return error;
}

void result_free(result_t* result) {
    free(result);
}