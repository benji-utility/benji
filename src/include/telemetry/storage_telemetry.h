#ifndef __BENJI_STORAGE_TELEMETRY_H
#define __BENJI_STORAGE_TELEMETRY_H

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#include "telemetry_base.h"

#ifdef _WIN32
    #include <winioctl.h>
#endif

#ifndef BENJI_MAX_STORAGE_DEVICES
    #define BENJI_MAX_STORAGE_DEVICES (32)
#endif

#ifndef BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE
    #define BENJI_DEFAULT_STORAGE_DEVICE_DESCRIPTOR_SIZE (512)
#endif

#ifndef BENJI_STORAGE_FIELDS
    #define BENJI_STORAGE_FIELDS(_field_getter_impl) \
        _field_getter_impl(storage_devices, model_info, size_t device_count, model_info_type_t model_info_type) \
        _field_getter_impl(storage_devices, sizes, size_t device_count)
#endif

BENJI_CREATE_TELEMETRY_STRUCT(STORAGE, storage,
    size_t device_count;

    // these will all be CSV strings
    char* models;
    char* serial_numbers;
    char* bus_types;
    char* sizes; // in GB
)

typedef enum _BENJI_STORAGE_DEVICE_MODEL_INFO_TYPE {
    BENJI_STORAGE_DEVICE_MODEL_NAME,
    BENJI_STORAGE_DEVICE_SERIAL_NUMBER,
    BENJI_STORAGE_DEVICE_BUS_TYPE
} model_info_type_t;

BENJI_CREATE_TELEMETRY_BASE(storage)

BENJI_STORAGE_FIELDS(BENJI_CREATE_TELEMETRY_GETTER_IMPL)

#ifdef _WIN32
    HANDLE _open_storage_device_handle(size_t device_index);
    result_t* _get_storage_device_descriptor(HANDLE handle, unsigned char** buffer);

    const char* _get_bus_type(STORAGE_BUS_TYPE bus_type);
#endif

size_t _count_storage_devices();

#endif