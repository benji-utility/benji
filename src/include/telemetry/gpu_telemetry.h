#ifndef __BENJI_GPU_INFO_H
#define __BENJI_GPU_INFO_H

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"

#include "hardware_base.h"

#include "../gpu_vendors.h"

#ifndef BENJI_GPU_FIELDS
    #define BENJI_GPU_FIELDS(_field_getter_impl) \
        _field_getter_impl(gpu, name) \
        _field_getter_impl(gpu, vendor)
#endif

BENJI_START_HARDWARE_STRUCT(GPU)
    char* name;
    char* vendor;
    double dedicated_video_memory; // in GB
    double dedicated_system_memory; // in GB
    double shared_system_memory; // in GB
BENJI_END_HARDWARE_STRUCT(gpu)

typedef enum _BENJI_GPU_MEMORY_TYPE {
    BENJI_GPU_DEDICATED_VIDEO_MEMORY,
    BENJI_GPU_DEDICATED_SYSTEM_MEMORY,
    BENJI_GPU_SHARED_SYSTEM_MEMORY
} gpu_memory_type_t;

BENJI_CREATE_HARDWARE_BASE(gpu)

BENJI_GPU_FIELDS(BENJI_CREATE_HARDWARE_GETTER_IMPL)

result_t* get_gpu_memory(gpu_memory_type_t memory_type);

#ifdef _WIN32
    result_t* get_gpu_description();
#endif

#endif