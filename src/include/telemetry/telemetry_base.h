#ifndef __BENJI_TELEMETRY_BASE_H
#define __BENJI_TELEMETRY_BASE_H

#include <stdint.h>

#include "../result.h"

#ifndef BENJI_CREATE_TELEMETRY_STRUCT
    #define BENJI_CREATE_TELEMETRY_STRUCT(_telemetry_group_upper, _telemetry_group_lower, ...) \
        typedef struct _BENJI_##_telemetry_group_upper##_INFO { \
            __VA_ARGS__ \
        } _telemetry_group_lower##_info_t;
#endif

#ifndef BENJI_CREATE_TELEMETRY_BASE
    #define BENJI_CREATE_TELEMETRY_BASE(_telemetry_group) \
        result_t* get_##_telemetry_group##_info(); \
        result_t* _telemetry_group##_info_to_map(const _telemetry_group##_info_t info); \
        void free_##_telemetry_group##_info(_telemetry_group##_info_t* info);
#endif

#ifndef BENJI_CREATE_TELEMETRY_GETTER_IMPL
    #define BENJI_CREATE_TELEMETRY_GETTER_IMPL(_telemetry_group, _field, ...) \
        result_t* get_##_telemetry_group##_##_field(__VA_ARGS__); \
        result_t* _get_##_telemetry_group##_##_field##_windows(__VA_ARGS__); \
        result_t* _get_##_telemetry_group##_##_field##_linux(__VA_ARGS__);
#endif

#ifndef get_telemetry_info_string
    #define get_telemetry_info_string(_info_parent, _info, _get_info, _free_info, ...) do { \
        result_t* result = _get_info(__VA_ARGS__); \
        return_if_error_with_free_info(result, _free_info, _info_parent); \
        _info = strdup((char*) result_unwrap_value(result)); \
        strtrim(_info); \
    } while (false);
#endif

#ifndef get_telemetry_info_integer
    #define get_telemetry_info_integer(_info_parent, _info, _get_info, _free_info, ...) do { \
        result_t* result = _get_info(__VA_ARGS__); \
        return_if_error_with_free_info(result, _free_info, _info_parent); \
        _info = (size_t) (uintptr_t) result_unwrap_value(result); \
    } while (false);
#endif

#ifndef get_telemetry_info_double
    #define get_telemetry_info_double(_info_parent, _info, _get_info, _free_info, ...) do { \
        result_t* result = _get_info(__VA_ARGS__); \
        return_if_error_with_free_info(result, _free_info, _info_parent); \
        _info = *(double*) result_unwrap_value(result); \
    } while (false);
#endif

static struct _BENJI_TELEMETRY_CONFIG {
    size_t max_processes;
} telemetry_config;

#endif