#include "include/hardware/cpu_info.h"

result_t* get_cpu_info() {
    cpu_info_t* info = malloc(sizeof(cpu_info_t));

    if (!info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    result_t* cpu_name_result = get_cpu_name();
    return_if_error(cpu_name_result);
    info->name = strdup((char*) result_unwrap_value(cpu_name_result));
    strtrim(info->name);

    result_t* cpu_vendor_result = get_cpu_vendor();
    return_if_error(cpu_vendor_result);
    info->vendor = strdup((char*) result_unwrap_value(cpu_vendor_result));
    strtrim(info->vendor);

    result_t* cpu_arch_result = get_cpu_arch();
    return_if_error(cpu_arch_result);
    info->arch = strdup((char*) result_unwrap_value(cpu_arch_result));
    strtrim(info->arch);

    result_t* cpu_clock_speed_result = get_cpu_clock_speed();
    return_if_error(cpu_clock_speed_result);
    info->clock_speed = *(double*) result_unwrap_value(cpu_clock_speed_result);

    result_t* cpu_core_count_result = get_cpu_core_count();
    return_if_error(cpu_core_count_result);
    info->core_count = (size_t) (uintptr_t) result_unwrap_value(cpu_core_count_result);

    result_t* cpu_logical_processors_count_result = get_cpu_logical_processors_count();
    return_if_error(cpu_logical_processors_count_result);
    info->logical_processors_count = (size_t) (uintptr_t) result_unwrap_value(cpu_logical_processors_count_result);

    return result_success(info);
}

result_t* get_cpu_name() {
    #if defined(_WIN32)
        int cpuid_info[BENJI_CPUID_BUFFER_LENGTH];

        char* cpu_name = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!cpu_name) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        cpu_name[0] = '\0';

        for (int i = 0; i < BENJI_CPUID_CPU_NAME_SECTIONS_COUNT; ++i) {
            __cpuid(cpuid_info, BENJI_CPUID_CPU_NAME_START + i);
            memcpy(cpu_name + (i * 16), cpuid_info, sizeof(cpuid_info));
        }

        return result_success(cpu_name);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_vendor() {
    #if defined(_WIN32)
        int cpu_info[BENJI_CPUID_BUFFER_LENGTH];

        char* cpu_vendor = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!cpu_vendor) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        cpu_vendor[0] = '\0';

        __cpuid(cpu_info, 0);

        *((int*) cpu_vendor) = cpu_info[1];
        *((int*) (cpu_vendor + 4)) = cpu_info[3];
        *((int*) (cpu_vendor + 8)) = cpu_info[2];

        cpu_vendor[strlen(cpu_vendor) - 2] = '\0'; // this is kinda cursed, but it works sooooo

        return result_success(cpu_vendor);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_arch() {
    #if defined(_WIN32)
        SYSTEM_INFO system_info;

        GetSystemInfo(&system_info);

        char* arch;

        // TODO: use all the possible CPU architectures found in winnt.h
        switch (system_info.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_INTEL: arch = "x86"; break;
            case PROCESSOR_ARCHITECTURE_AMD64: arch = "x64"; break;
            case PROCESSOR_ARCHITECTURE_ARM: arch = "ARM"; break;
            case PROCESSOR_ARCHITECTURE_ARM64: arch = "ARM64"; break;
            case PROCESSOR_ARCHITECTURE_IA64: arch = "IA-64"; break;

            case PROCESSOR_ARCHITECTURE_UNKNOWN: // make this one fall through because i dont wanna deal with edge cases
            default: arch = "???"; break;
        }

        return result_success(arch);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_clock_speed() {
    #if defined(_WIN32)
        HKEY hkey;

        HRESULT hresult = RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            TEXT(BENJI_CPU_CLOCK_SPEED_REGISTRY_PATH),
            0, KEY_READ, &hkey
        );

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegOpenKeyEx() failed");
        }

        uint32_t speed = 0;
        unsigned long int data_type, data_size = sizeof(speed);

        hresult = RegQueryValueEx(
            hkey, TEXT(BENJI_CPU_CLOCK_SPEED_REGISTRY_KEY), NULL, &data_type, (LPBYTE) &speed, &data_size
        );

        if (SUCCEEDED(hresult) && data_type == REG_DWORD) {
            hresult = RegCloseKey(hkey);

            if (FAILED(hresult)) {
                return result_error(hresult, BENJI_ERROR_PACKET, "RegCloseKey() failed");
            }

            void* speed_ghz = malloc(sizeof(double));

            if (!speed_ghz) {
                return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
            }

            *(double*) speed_ghz = speed / 1000.0;

            return result_success(speed_ghz);
        }
        else {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegQueryValueEx() failed");
        }
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_core_count() {
    #if defined(_WIN32)
        return get_cpu_processor_info(count_cpu_cores);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_logical_processors_count() {
    #if defined(_WIN32)
        return get_cpu_processor_info(count_cpu_logical_processors);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

#ifdef _WIN32
    result_t* get_cpu_processor_info(processor_info_callback_t callback) {
        unsigned long int length = 0;

        GetLogicalProcessorInformation(NULL, &length);

        SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = malloc(length);

        if (!buffer) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        if (!GetLogicalProcessorInformation(buffer, &length)) {
            free(buffer);

            return result_error(-1, BENJI_ERROR_PACKET, "GetLogicalProcessorInformation() failed");
        }

        uint32_t result = 0;

        size_t count = length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

        for (size_t i = 0; i < count; i++) {
            if (buffer[i].Relationship == RelationProcessorCore) {
                result += callback(&buffer[i]);
            }
        }

        free(buffer);

        return result_success((void*) (uintptr_t) result);
    }

    uint32_t count_cpu_cores(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info) {
        return 1;
    }

    uint32_t count_cpu_logical_processors(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info) {
        return __popcnt(info->ProcessorMask);
    }
#endif

result_t* cpu_info_to_map(cpu_info_t cpu_info) {
    map_t* cpu_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    buffer[0] = '\0';

    map_insert(cpu_info_map, "name", cpu_info.name);
    map_insert(cpu_info_map, "vendor", cpu_info.vendor);
    map_insert(cpu_info_map, "arch", cpu_info.arch);

    sprintf(buffer, "%0.3f", cpu_info.clock_speed);
    map_insert(cpu_info_map, "clock_speed", buffer);

    sprintf(buffer, "%lli", cpu_info.core_count);
    map_insert(cpu_info_map, "core_count", buffer);

    sprintf(buffer, "%lli", cpu_info.logical_processors_count);
    map_insert(cpu_info_map, "logical_processors_count", buffer);

    free(buffer);

    return result_success(cpu_info_map);
}