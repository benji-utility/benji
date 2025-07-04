#ifndef __BENJI_CPU_INFO_H
#define __BENJI_CPU_INFO_H

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"
#include "../map.h"
#include "../result.h"

#include "hardware_base.h"

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

#ifndef BENJI_CPU_FIELDS
    #define BENJI_CPU_FIELDS(_field_getter_impl) \
        _field_getter_impl(cpu, name) \
        _field_getter_impl(cpu, vendor) \
        _field_getter_impl(cpu, arch) \
        _field_getter_impl(cpu, clock_speed) \
        _field_getter_impl(cpu, core_count) \
        _field_getter_impl(cpu, logical_processors_count)
#endif

BENJI_START_HARDWARE_STRUCT(CPU)
    char* name;
    char* vendor;
    char* arch;
    double clock_speed; // in GHz
    size_t core_count;
    size_t logical_processors_count;
BENJI_END_HARDWARE_STRUCT(cpu)

BENJI_CREATE_HARDWARE_BASE(cpu)

BENJI_CPU_FIELDS(BENJI_CREATE_HARDWARE_GETTER_IMPL)

#ifdef _WIN32
    typedef uint32_t (*processor_info_callback_t)(SYSTEM_LOGICAL_PROCESSOR_INFORMATION*);

    result_t* get_cpu_processor_info(processor_info_callback_t callback);
    uint32_t count_cpu_cores_callback(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info);
    uint32_t count_cpu_logical_processors_callback(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info);
#endif

#endif