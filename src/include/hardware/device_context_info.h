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

result_t* get_device_context_info();

result_t* get_device_context_device_name();
result_t* get_device_context_operating_system();
result_t* get_device_context_operating_system_version();
result_t* get_device_context_hostname();

result_t* device_context_info_to_map(device_context_info_t device_context_info);

#endif