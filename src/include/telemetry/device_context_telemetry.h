#ifndef __BENJI_DEVICE_CONTEXT_INFO_H
#define __BENJI_DEVICE_CONTEXT_INFO_H

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#include "hardware_base.h"

#ifndef BENJI_DEVICE_CONTEXT_FIELDS
    #define BENJI_DEVICE_CONTEXT_FIELDS(_field_getter_impl) \
        _field_getter_impl(device_context, device_name) \
        _field_getter_impl(device_context, hostname)
#endif

BENJI_START_HARDWARE_STRUCT(DEVICE_CONTEXT)
    char* device_name;
    char* operating_system;
    char* operating_system_version;
    char* hostname;
BENJI_END_HARDWARE_STRUCT(device_context)

typedef enum _BENJI_OPERATING_SYSTEM_VERSION_INFO_TYPE {
    BENJI_OPERATING_SYSTEM_VERSION_NAME,
    BENJI_OPERATING_SYSTEM_VERSION_NUMBER
} os_version_info_type_t;

BENJI_CREATE_HARDWARE_BASE(device_context)

BENJI_DEVICE_CONTEXT_FIELDS(BENJI_CREATE_HARDWARE_GETTER_IMPL)

result_t* get_device_context_operating_system_info(os_version_info_type_t version_info_type);

#ifdef _WIN32
    typedef LONG (WINAPI* rtl_get_version_t)(PRTL_OSVERSIONINFOW);

    char* get_windows_name_from_version(unsigned long major_version, unsigned long minor_version, unsigned long build_number);
#endif

#endif