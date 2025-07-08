#ifndef __BENJI_GPU_TELEMETRY_H
#define __BENJI_GPU_TELEMETRY_H

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"

#include "telemetry_base.h"

#include "../gpu_vendors.h"

#ifndef BENJI_GPU_FIELDS
    #define BENJI_GPU_FIELDS(_field_getter_impl) \
        _field_getter_impl(gpu, name) \
        _field_getter_impl(gpu, vendor)
#endif

typedef enum _BENJI_GPU_MEMORY_TYPE {
    BENJI_GPU_WIN32_DEDICATED_VIDEO_MEMORY,
    BENJI_GPU_WIN32_DEDICATED_SYSTEM_MEMORY,
    BENJI_GPU_WIN32_SHARED_SYSTEM_MEMORY
} gpu_memory_type_t;

BENJI_CREATE_TELEMETRY_STRUCT(GPU, gpu,
    char* name;
    char* vendor;

    #ifdef _WIN32
        double dedicated_video_memory; // in GB
        double dedicated_system_memory; // in GB
        double shared_system_memory; // in GB
    #elif __linux__
        /* TODO: add linux stuff */
    #endif
)

BENJI_CREATE_TELEMETRY_BASE(gpu)

BENJI_GPU_FIELDS(BENJI_CREATE_TELEMETRY_GETTER_IMPL)

result_t* get_gpu_memory(gpu_memory_type_t memory_type);

#ifdef _WIN32
    result_t* _get_gpu_description();
#endif

#endif