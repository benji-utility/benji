#ifndef __BENJI_PROCESSES_TELEMETRY_H
#define __BENJI_PROCESSES_TELEMETRY_H

#include <stdint.h>

#ifndef BENJI_USE_SYSTEM_TELEMETRY_UTILS
    #define BENJI_USE_SYSTEM_TELEMETRY_UTILS
#endif

#include "../utils.h"
#include "../result.h"

#include "telemetry_base.h"

#ifndef BENJI_PROCESSES_FIELDS
    #define BENJI_PROCESSES_FIELDS(_field_getter_impl) \
        _field_getter_impl(processes, names) \
        _field_getter_impl(processes, pids) \
        _field_getter_impl(processes, parent_pids) \
        _field_getter_impl(processes, cpu_usages) \
        _field_getter_impl(processes, gpu_usages) \
        _field_getter_impl(processes, ram_usages) \
        _field_getter_impl(processes, disk_usages) \
        _field_getter_impl(processes, network_usages)
#endif

BENJI_CREATE_TELEMETRY_STRUCT(PROCESSES, processes,
    size_t processes_count;

    char* names;
    size_t* pids;
    size_t* parent_pids;
    double* cpu_usages; // percent between 0.0 and 1.0
    double* gpu_usages; // percent between 0.0 and 1.0
    double* ram_usages; // percent between 0.0 and 1.0
    double* disk_usages; // percent between 0.0 and 1.0
    double* network_usages; // percent between 0.0 and 1.0
)

BENJI_CREATE_TELEMETRY_BASE(processes)

BENJI_PROCESSES_FIELDS(BENJI_CREATE_TELEMETRY_GETTER_IMPL)

#endif