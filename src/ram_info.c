#include "include/hardware/ram_info.h"

result_t* get_ram_info() {
    ram_info_t* info = malloc(sizeof(ram_info_t));

    if (!info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    result_t* ram_total_memory_result = get_ram_total_memory();
    return_if_error(ram_total_memory_result);
    info->total_memory = *(double*) result_unwrap_value(ram_total_memory_result);

    result_t* ram_memory_load_result = get_ram_memory_load();
    return_if_error(ram_memory_load_result);
    info->memory_load = *(double*) result_unwrap_value(ram_memory_load_result);

    result_t* ram_free_memory_result = get_ram_free_memory();
    return_if_error(ram_free_memory_result);
    info->free_memory = *(double*) result_unwrap_value(ram_free_memory_result);

    result_t* ram_speed_result = get_ram_speed();
    return_if_error(ram_speed_result);
    info->speed = (uint16_t) (uintptr_t) result_unwrap_value(ram_speed_result);

    return result_success(info);
}

result_t* get_ram_total_memory() {
    #if defined(_WIN32)
        result_t* status_result = get_memory_status();
        return_if_error(status_result);

        MEMORYSTATUSEX status = *(MEMORYSTATUSEX*) result_unwrap_value(status_result);

        void* memory = malloc(sizeof(double));

        if (!memory) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        *(double*) memory = bytes_to_gigabytes(status.ullTotalPhys);

        return result_success(memory);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_ram_memory_load() {
    #if defined(_WIN32)
        result_t* status_result = get_memory_status();
        return_if_error(status_result);

        MEMORYSTATUSEX status = *(MEMORYSTATUSEX*) result_unwrap_value(status_result);

        result_t* total_memory_result = get_ram_total_memory();

        if (total_memory_result->is_error) {
            return result_error(
                total_memory_result->payload.error.code,
                BENJI_ERROR_PACKET,
                total_memory_result->payload.error.message
            );
        }

        double total_memory = *(double*) result_unwrap_value(total_memory_result);

        double percent = status.dwMemoryLoad / 100.0; // dwMemoryLoad returns a value between [0, 100]

        void* memory = malloc(sizeof(double));

        if (!memory) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        // total memory is already in GB, so no need to convert
        *(double*) memory = total_memory * percent;

        return result_success(memory);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_ram_free_memory() {
    #if defined(_WIN32)
        result_t* status_result = get_memory_status();
        return_if_error(status_result);

        MEMORYSTATUSEX status = *(MEMORYSTATUSEX*) result_unwrap_value(status_result);

        void* memory = malloc(sizeof(double));

        if (!memory) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        *(double*) memory = bytes_to_gigabytes(status.ullAvailPhys);

        return result_success(memory);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_ram_speed() {
    #if defined(_WIN32)
        unsigned long size = GetSystemFirmwareTable(BENJI_SYSTEM_FIRMWARE_TABLE, 0, NULL, 0);

        if (size == 0) {
            return result_error(-1, BENJI_ERROR_PACKET, "Failed to get SMBIOS table size");
        }

        RAW_SMBIOS_DATA* buffer = malloc(size);

        if (!buffer) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        if (!GetSystemFirmwareTable(BENJI_SYSTEM_FIRMWARE_TABLE, 0, buffer, size)) {
            free(buffer);

            return result_error(-1, BENJI_ERROR_PACKET, "Failed to get SMBIOS table");
        }

        uint8_t* data = buffer->data;

        uint8_t* end = data + buffer->length;
        
        uint16_t speed = 0;

        bool found_memory_device = false;

        while (data < end) {
            SMBIOS_MEMORY_DEVICE* memory = (SMBIOS_MEMORY_DEVICE*) data;

            if (memory->type == BENJI_SMBIOS_MEMORY_DEVICE_TYPE) {  // Memory Device
                found_memory_device = true;

                if (memory->speed > 0) {
                    speed = memory->speed;
                }
            }

            data += memory->length;

            while (data < end && (*data != 0 || *(data + 1) != 0)) {
                data++;
            }

            data += 2;
        }

        free(buffer);

        if (!found_memory_device) {
            return result_error(-1, BENJI_ERROR_PACKET, "SMBIOS Type 17 (Memory Device) not found");
        } else if (speed == 0) {
            return result_error(-1, BENJI_ERROR_PACKET, "RAM speed reported as 0 (unknown/unsupported by BIOS)");
        }

        return result_success((void*) (uintptr_t) speed);
    #endif
}

#ifdef _WIN32
    result_t* get_memory_status() {
        MEMORYSTATUSEX* status = malloc(sizeof(MEMORYSTATUSEX));

        if (!status) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        status->dwLength = sizeof(MEMORYSTATUSEX);

        // for whatever stupid reason, it returns zero upon erroring
        if (!GlobalMemoryStatusEx(status)) {
            free(status);

            return result_error(GetLastError(), BENJI_ERROR_PACKET, "GlobalMemoryStatusEx() failed");
        }

        return result_success((void*) status);
    }
#endif

result_t* ram_info_to_map(ram_info_t ram_info) {
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

    sprintf(buffer, "%0.3f", ram_info.free_memory);
    map_insert(ram_info_map, "free_memory", strdup(buffer));

    sprintf(buffer, "%i", ram_info.speed);
    map_insert(ram_info_map, "speed", strdup(buffer));

    free(buffer);

    return result_success(ram_info_map);
}

void free_ram_info(ram_info_t* info) {
    free(info);
}