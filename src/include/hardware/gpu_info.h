#ifndef __BENJI_GPU_INFO_H
#define __BENJI_GPU_INFO_H

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "../utils.h"

#include "../gpu_vendors.h"

typedef struct _BENJI_GPU_INFO {
    char* name;
    char* vendor;
    double dedicated_video_memory; // in GB
    double dedicated_system_memory; // in GB
    double shared_system_memory; // in GB
} gpu_info_t;

typedef enum _BENJI_GPU_MEMORY_TYPE {
    BENJI_GPU_DEDICATED_VIDEO_MEMORY,
    BENJI_GPU_DEDICATED_SYSTEM_MEMORY,
    BENJI_GPU_SHARED_SYSTEM_MEMORY
} gpu_memory_type_t;

result_t* get_gpu_info();

result_t* get_gpu_name();
result_t* get_gpu_vendor();
result_t* get_gpu_memory(gpu_memory_type_t memory_type);

#ifdef _WIN32
    result_t* get_gpu_description();
#endif

result_t* gpu_info_to_map(gpu_info_t gpu_info);

#endif