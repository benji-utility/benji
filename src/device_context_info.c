#include "include/hardware/device_context_info.h"

result_t* get_device_context_info() {
    device_context_info_t* info = malloc(sizeof(device_context_info_t));

    result_t* device_name_result = get_device_context_device_name();
    return_if_error(device_name_result);
    info->device_name = strdup((char*) result_unwrap_value(device_name_result));
    strtrim(info->device_name);

    return result_success(info);
}

result_t* get_device_context_device_name() {
    #if defined(_WIN32)
        char device_name[BENJI_BASIC_STRING_LENGTH];
        unsigned long device_name_length = sizeof(device_name);

        if (GetComputerNameA(device_name, &device_name_length)) {
            return result_success(device_name);
        }
        else {
            return result_error(
                GetLastError(),
                "GetComputerNameA() failed",
                BENJI_ERROR_PACKET
            );
        }
    #elif defined(__linux__)
        // TODO: add linux stuff
    #endif
}

result_t* get_device_context_operating_system();
result_t* get_device_context_operating_system_version();
result_t* get_device_context_hostname();

result_t* device_context_info_to_map(device_context_info_t device_context_info) {
    map_t* device_context_info_map = map_init();

    map_insert(device_context_info_map, "device_name", device_context_info.device_name);
    map_insert(device_context_info_map, "operating_system", device_context_info.operating_system);
    map_insert(device_context_info_map, "operating_system_version", device_context_info.operating_system_version);
    map_insert(device_context_info_map, "operating_system_version", device_context_info.hostname);

    return result_success(device_context_info_map);
}