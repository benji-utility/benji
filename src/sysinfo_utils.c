#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "include/hardware.h"
#include "include/utils.h"
#include "include/result.h"

result_t* get_hardware_info(const char* hardware_group, char** header) {
    map_t* map_data;

    if (strcmp(hardware_group, "cpu_all") == 0) {
        collect_map_data(cpu_info_t, get_cpu_info, cpu_info_to_map, map_data);

        *header = "cpu_info";
    }
    else if (strcmp(hardware_group, "gpu_all") == 0) {
        collect_map_data(gpu_info_t, get_gpu_info, gpu_info_to_map, map_data);

        *header = "gpu_info";
    }
    else if (strcmp(hardware_group, "ram_all") == 0) {
        collect_map_data(ram_info_t, get_ram_info, ram_info_to_map, map_data);

        *header = "ram_info";
    }

    return result_success(map_data);
}