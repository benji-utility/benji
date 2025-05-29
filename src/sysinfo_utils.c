#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "include/hardware.h"
#include "include/utils.h"
#include "include/result.h"

result_t* get_hardware_info(const char* hardware_group, char** header) {
    map_t* map_data;

    if (BENJI_STRING_EQUALS(hardware_group, "cpu_all")) {
        collect_map_data(
            cpu_info_t,
            get_cpu_info,
            cpu_info_to_map,
            map_data,
            free_cpu_info
        );

        *header = "cpu_info";
    }
    else if (BENJI_STRING_EQUALS(hardware_group, "gpu_all")) {
        collect_map_data(
            gpu_info_t,
            get_gpu_info,
            gpu_info_to_map,
            map_data,
            free_gpu_info
        );

        *header = "gpu_info";
    }
    else if (BENJI_STRING_EQUALS(hardware_group, "ram_all")) {
        collect_map_data(
            ram_info_t,
            get_ram_info,
            ram_info_to_map,
            map_data,
            free_ram_info
        );

        *header = "ram_info";
    }
    else if (BENJI_STRING_EQUALS(hardware_group, "device_context_all")) {
        collect_map_data(
            device_context_info_t,
            get_device_context_info,
            device_context_info_to_map,
            map_data,
            free_device_context_info
        );

        *header = "device_info";
    }
    else if (BENJI_STRING_EQUALS(hardware_group, "storage_all")) {
        collect_map_data(
            storage_info_t,
            get_storage_info,
            storage_info_to_map,
            map_data,
            free_storage_info
        );

        *header = "storage_info";
    }
    else {
        return result_error(-1, BENJI_ERROR_PACKET, "Invalid data group: '%s'", hardware_group);
    }

    return result_success(map_data);
}