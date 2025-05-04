#ifndef __BENJI_STORAGE_INFO_H
#define __BENJI_STORAGE_INFO_H

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#ifdef _WIN32
    #include <winioctl.h>
#endif

#ifndef BENJI_MAX_STORAGE_DEVICES
    #define BENJI_MAX_STORAGE_DEVICES (32)
#endif

#ifndef BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE
    #define BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE (512)
#endif

typedef struct _BENJI_STORAGE_INFO {
    size_t device_count;

    // these will all be CSV strings
    char* models;
    char* serial_numbers;
    char* bus_types;
    char* sizes; // in GB
} storage_info_t;

result_t* get_storage_info();

result_t* get_storage_models(size_t* device_count); // this has the count only because it is the first one called
result_t* get_storage_serial_numbers();
result_t* get_storage_bus_types();
result_t* get_storage_sizes();

#ifdef _WIN32
    HANDLE open_storage_device_handle(size_t device_index);
    result_t* get_storage_device_descriptor(HANDLE handle, unsigned char** buffer);

    const char* get_bus_type(STORAGE_BUS_TYPE bus_type);
#endif

result_t* storage_info_to_map(storage_info_t storage_info);

#endif