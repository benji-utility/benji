#ifndef __BENJI_STORAGE_INFO_H
#define __BENJI_STORAGE_INFO_H

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#include "hardware_base.h"

#ifdef _WIN32
    #include <winioctl.h>
#endif

#ifndef BENJI_MAX_STORAGE_DEVICES
    #define BENJI_MAX_STORAGE_DEVICES (32)
#endif

#ifndef BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE
    #define BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE (512)
#endif

BENJI_START_HARDWARE_STRUCT(STORAGE)
    size_t device_count;

    // these will all be CSV strings
    char* models;
    char* serial_numbers;
    char* bus_types;
    char* sizes; // in GB
BENJI_END_HARDWARE_STRUCT(storage)

typedef enum _BENJI_STORAGE_DEVICE_MODEL_INFO_TYPE {
    BENJI_STORAGE_DEVICE_MODEL_NAME,
    BENJI_STORAGE_DEVICE_SERIAL_NUMBER,
    BENJI_STORAGE_DEVICE_BUS_TYPE
} model_info_type_t;

BENJI_CREATE_HARDWARE_BASE(storage)

result_t* get_storage_devices_info(size_t device_count, model_info_type_t info_type);
result_t* get_storage_devices_size(size_t device_count);

#ifdef _WIN32
    HANDLE open_storage_device_handle(size_t device_index);
    result_t* get_storage_device_descriptor(HANDLE handle, unsigned char** buffer);

    const char* get_bus_type(STORAGE_BUS_TYPE bus_type);
#endif

size_t count_storage_devices();

#endif