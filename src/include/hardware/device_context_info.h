#ifndef __BENJI_DEVICE_CONTEXT_INFO_H
#define __BENJI_DEVICE_CONTEXT_INFO_H

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

typedef struct _BENJI_DEVICE_CONTEXT_INFO {
    char* device_name;
    char* operating_system;
    char* operating_system_version;
    char* hostname;
} device_context_info_t;

typedef enum _BENJI_OPERATING_SYSTEM_VERSION_INFO_TYPE {
    BENJI_OPERATING_SYSTEM_VERSION_NAME,
    BENJI_OPERATING_SYSTEM_VERSION_NUMBER
} os_version_info_type_t;

result_t* get_device_context_info();

result_t* get_device_context_device_name();
result_t* get_device_context_operating_system_info(os_version_info_type_t version_info_type);
result_t* get_device_context_hostname();

#ifdef _WIN32
    typedef LONG (WINAPI* rtl_get_version_t)(PRTL_OSVERSIONINFOW);

    char* get_windows_name_from_version(unsigned long major_version, unsigned long minor_version, unsigned long build_number);
#endif

result_t* device_context_info_to_map(device_context_info_t device_context_info);

void free_device_context_info(device_context_info_t* info);

#endif