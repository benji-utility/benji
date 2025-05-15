#ifndef __BENJI_CPU_INFO_H
#define __BENJI_CPU_INFO_H

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#if defined(_WIN32)
    #include <intrin.h>
    #include <unistd.h>
#endif

#ifndef BENJI_CPUID_BUFFER_LENGTH
    #define BENJI_CPUID_BUFFER_LENGTH (4)
#endif

#ifndef BENJI_CPUID_CPU_NAME_SECTIONS_COUNT
    #define BENJI_CPUID_CPU_NAME_SECTIONS_COUNT (3)
#endif

#ifndef BENJI_CPUID_CPU_NAME_START
    #define BENJI_CPUID_CPU_NAME_START (0x80000002)
#endif

#ifndef BENJI_CPU_CLOCK_SPEED_REGISTRY_PATH
    #define BENJI_CPU_CLOCK_SPEED_REGISTRY_PATH "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#endif

#ifndef BENJI_CPU_CLOCK_SPEED_REGISTRY_KEY
    #define BENJI_CPU_CLOCK_SPEED_REGISTRY_KEY "~MHz"
#endif

typedef struct _BENJI_CPU_INFO {
    char* name;
    char* vendor;
    char* arch;
    double clock_speed; // in GHz
    size_t core_count;
    size_t logical_processors_count;
    // double current_temp; // TBD until i figure out how
} cpu_info_t;

result_t* get_cpu_info();

result_t* get_cpu_name();
result_t* get_cpu_vendor();
result_t* get_cpu_arch();
result_t* get_cpu_clock_speed();
result_t* get_cpu_core_count();
result_t* get_cpu_logical_processors_count();

#ifdef _WIN32
    typedef uint32_t (*processor_info_callback_t)(SYSTEM_LOGICAL_PROCESSOR_INFORMATION*);

    result_t* get_cpu_processor_info(processor_info_callback_t callback);
    uint32_t count_cpu_cores(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info);
    uint32_t count_cpu_logical_processors(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info);
#endif

result_t* cpu_info_to_map(cpu_info_t cpu_info);

#endif