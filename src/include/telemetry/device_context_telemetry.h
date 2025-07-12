#ifndef __BENJI_DEVICE_CONTEXT_TELEMETRY_H
#define __BENJI_DEVICE_CONTEXT_TELEMETRY_H

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#include "telemetry_base.h"

#ifndef BENJI_DEVICE_CONTEXT_FIELDS
    #define BENJI_DEVICE_CONTEXT_FIELDS(_field_getter_impl) \
        _field_getter_impl(device_context, device_name) \
        _field_getter_impl(device_context, operating_system_version_info, os_version_info_type_t version_info_type) \
        _field_getter_impl(device_context, hostname)
#endif

BENJI_CREATE_TELEMETRY_STRUCT(DEVICE_CONTEXT, device_context,
    char* device_name;
    char* operating_system_name;
    char* operating_system_version;
    char* hostname;
)

typedef enum _BENJI_OPERATING_SYSTEM_VERSION_INFO_TYPE {
    BENJI_OPERATING_SYSTEM_VERSION_NAME,
    BENJI_OPERATING_SYSTEM_VERSION_NUMBER
} os_version_info_type_t;

BENJI_CREATE_TELEMETRY_BASE(device_context)

BENJI_DEVICE_CONTEXT_FIELDS(BENJI_CREATE_TELEMETRY_GETTER_IMPL)

#ifdef _WIN32
    typedef LONG (WINAPI* rtl_get_version_t)(PRTL_OSVERSIONINFOW);

    char* _get_windows_name_from_version(unsigned long major_version, unsigned long minor_version, unsigned long build_number);
#endif

#endif