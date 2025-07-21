#include "include/telemetry/ram_telemetry.h"

result_t* get_ram_info() {
    ram_info_t* ram_info = malloc(sizeof(ram_info_t));

    if (!ram_info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    get_telemetry_info_double(
        ram_info,
        ram_info->total_memory,
        get_ram_total_memory,
        free_ram_info
    );

    get_telemetry_info_double(
        ram_info,
        ram_info->memory_load,
        get_ram_memory_load,
        free_ram_info
    );

    get_telemetry_info_double(
        ram_info,
        ram_info->available_memory,
        get_ram_available_memory,
        free_ram_info
    );

    get_telemetry_info_integer(
        ram_info,
        ram_info->speed,
        get_ram_speed,
        free_ram_info
    );

    return result_success(ram_info);
}

result_t* get_ram_total_memory() {
    #if defined(_WIN32)
        return _get_ram_total_memory_windows();
    #elif defined(__linux__)
        return _get_ram_total_memory_linux();
    #endif
}

result_t* get_ram_memory_load() {
    #if defined(_WIN32)
        return _get_ram_memory_load_windows();
    #elif defined(__linux__)
        return _get_ram_memory_load_linux();
    #endif
}

result_t* get_ram_available_memory() {
    #if defined(_WIN32)
        return _get_ram_available_memory_windows();
    #elif defined(__linux__)
        return _get_ram_available_memory_linux();
    #endif
}

result_t* get_ram_speed() {
    #if defined(_WIN32)
        return _get_ram_speed_windows();
    #elif defined(__linux__)
        return _get_ram_speed_linux();
    #endif
}

#if defined(_WIN32)
    result_t* _get_ram_total_memory_windows() {
        result_t* status_result = _get_memory_status();
        return_if_error(status_result);

        MEMORYSTATUSEX memory_status = *(MEMORYSTATUSEX*) result_unwrap_value(status_result);

        void* total_memory = malloc(sizeof(double));

        if (!total_memory) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        *(double*) total_memory = bytes_to_gigabytes(memory_status.ullTotalPhys);

        return result_success(total_memory);
    }

    result_t* _get_ram_memory_load_windows() {
        result_t* status_result = _get_memory_status();
        return_if_error(status_result);

        MEMORYSTATUSEX memory_status = *(MEMORYSTATUSEX*) result_unwrap_value(status_result);

        result_t* total_memory_result = get_ram_total_memory();

        if (total_memory_result->is_error) {
            return result_error(
                total_memory_result->payload.error.code,
                BENJI_ERROR_PACKET,
                total_memory_result->payload.error.message
            );
        }

        double total_memory = *(double*) result_unwrap_value(total_memory_result);

        double memory_usage_percent = memory_status.dwMemoryLoad / 100.0; // "dwMemoryLoad" returns a value between [0, 100]

        void* used_memory = malloc(sizeof(double));

        if (!used_memory) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        // total memory is already in GB, so no need to convert
        *(double*) used_memory = total_memory * memory_usage_percent;

        return result_success(used_memory);
    }

    result_t* _get_ram_available_memory_windows() {
        result_t* status_result = _get_memory_status();
        return_if_error(status_result);

        MEMORYSTATUSEX status = *(MEMORYSTATUSEX*) result_unwrap_value(status_result);

        void* available_memory = malloc(sizeof(double));

        if (!available_memory) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        *(double*) available_memory = bytes_to_gigabytes(status.ullAvailPhys);

        return result_success(available_memory);
    }

    result_t* _get_ram_speed_windows() {
        unsigned long system_firmware_table_size = GetSystemFirmwareTable(BENJI_SYSTEM_FIRMWARE_TABLE, 0, NULL, 0);

        if (system_firmware_table_size == 0) {
            return result_error(-1, BENJI_ERROR_PACKET, "Failed to get SMBIOS table size");
        }

        RAW_SMBIOS_DATA* smbios_data_buffer = malloc(system_firmware_table_size);

        if (!smbios_data_buffer) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        if (!GetSystemFirmwareTable(BENJI_SYSTEM_FIRMWARE_TABLE, 0, smbios_data_buffer, system_firmware_table_size)) {
            free(smbios_data_buffer);

            return result_error(-1, BENJI_ERROR_PACKET, "Failed to get SMBIOS table");
        }

        uint8_t* smbios_data = smbios_data_buffer->data;

        uint8_t* smbios_data_end = smbios_data + smbios_data_buffer->length;

        uint16_t memory_speed = 0;

        bool found_memory_device = false;

        while (smbios_data < smbios_data_end) {
            SMBIOS_MEMORY_DEVICE* memory_device = (SMBIOS_MEMORY_DEVICE*) smbios_data;

            if (memory_device->type == BENJI_SMBIOS_MEMORY_DEVICE_TYPE) {
                found_memory_device = true;

                if (memory_device->speed > 0) {
                    memory_speed = memory_device->speed;
                }
            }

            smbios_data += memory_device->length;

            while (smbios_data < smbios_data_end && (*smbios_data != 0 || *(smbios_data + 1) != 0)) {
                smbios_data++;
            }

            smbios_data += 2;
        }

        free(smbios_data_buffer);

        if (!found_memory_device) {
            return result_error(-1, BENJI_ERROR_PACKET, "SMBIOS Type 17 (Memory Device) not found");
        } else if (memory_speed == 0) {
            return result_error(-1, BENJI_ERROR_PACKET, "RAM speed reported as 0MHz (unknown/unsupported by BIOS)");
        }

        return result_success((void*) (uintptr_t) memory_speed);
    }

    result_t* _get_memory_status() {
        MEMORYSTATUSEX* memory_status = malloc(sizeof(MEMORYSTATUSEX));

        if (!memory_status) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        memory_status->dwLength = sizeof(MEMORYSTATUSEX);

        // for whatever stupid reason, "GlobalMemoryStatusEx()" returns zero upon erroring
        if (!GlobalMemoryStatusEx(memory_status)) {
            free(memory_status);

            return result_error(GetLastError(), BENJI_ERROR_PACKET, "GlobalMemoryStatusEx() failed");
        }

        return result_success((void*) memory_status);
    }
#elif defined(__linux__)
    result_t* _get_ram_total_memory_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_ram_memory_load_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_ram_available_memory_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_ram_speed_linux() {
        // TODO: add linux stuff
    }
#endif

result_t* ram_info_to_map(const ram_info_t ram_info) {
    map_t* ram_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    buffer[0] = '\0';

    sprintf(buffer, "%0.3f", ram_info.total_memory);
    map_insert(ram_info_map, "total_memory", strdup(buffer));

    sprintf(buffer, "%0.3f", ram_info.memory_load);
    map_insert(ram_info_map, "memory_load", strdup(buffer));

    sprintf(buffer, "%0.3f", ram_info.available_memory);
    map_insert(ram_info_map, "available_memory", strdup(buffer));

    sprintf(buffer, "%i", ram_info.speed);
    map_insert(ram_info_map, "speed", strdup(buffer));

    free(buffer);

    return result_success(ram_info_map);
}

void free_ram_info(ram_info_t* ram_info) {
    free(ram_info);
    ram_info = NULL;
}