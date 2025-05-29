#include "include/hardware/cpu_info.h"

result_t* get_cpu_info() {
    cpu_info_t* info = malloc(sizeof(cpu_info_t));

    if (!info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    result_t* cpu_name_result = get_cpu_name();
    return_if_error_with_free_info(cpu_name_result, free_cpu_info, info);
    info->name = strdup((char*) result_unwrap_value(cpu_name_result));
    strtrim(info->name);

    result_t* cpu_vendor_result = get_cpu_vendor();
    return_if_error_with_free_info(cpu_vendor_result, free_cpu_info, info);
    info->vendor = strdup((char*) result_unwrap_value(cpu_vendor_result));
    strtrim(info->vendor);

    result_t* cpu_arch_result = get_cpu_arch();
    return_if_error_with_free_info(cpu_arch_result, free_cpu_info, info);
    info->arch = strdup((char*) result_unwrap_value(cpu_arch_result));
    strtrim(info->arch);

    result_t* cpu_clock_speed_result = get_cpu_clock_speed();
    return_if_error_with_free_info(cpu_clock_speed_result, free_cpu_info, info);
    info->clock_speed = *(double*) result_unwrap_value(cpu_clock_speed_result);

    result_t* cpu_core_count_result = get_cpu_core_count();
    return_if_error_with_free_info(cpu_core_count_result, free_cpu_info, info);
    info->core_count = (size_t) (uintptr_t) result_unwrap_value(cpu_core_count_result);

    result_t* cpu_logical_processors_count_result = get_cpu_logical_processors_count();
    return_if_error_with_free_info(cpu_logical_processors_count_result, free_cpu_info, info);
    info->logical_processors_count = (size_t) (uintptr_t) result_unwrap_value(cpu_logical_processors_count_result);

    return result_success(info);
}

result_t* get_cpu_name() {
    #if defined(_WIN32)
        int cpuid_info[BENJI_CPUID_BUFFER_LENGTH];

        char* name = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!name) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        memset(name, 0, BENJI_BASIC_STRING_LENGTH);

        for (int i = 0; i < BENJI_CPUID_CPU_NAME_SECTIONS_COUNT; ++i) {
            __cpuid(cpuid_info, BENJI_CPUID_CPU_NAME_START + i);
            memcpy(name + (i * 16), cpuid_info, sizeof(cpuid_info));
        }

        // TODO: maybe make this terminate after the last character?
        name[BENJI_BASIC_STRING_LENGTH - 1] = '\0';

        return result_success(name);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_vendor() {
    #if defined(_WIN32)
        int cpu_info[BENJI_CPUID_BUFFER_LENGTH];

        char* vendor = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!vendor) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        vendor[0] = '\0';

        __cpuid(cpu_info, 0);

        // TODO: make these not hardcoded constants
        memcpy(vendor, &cpu_info[1], 4);
        memcpy(vendor + 4, &cpu_info[3], 4);
        memcpy(vendor + 8, &cpu_info[2], 4);

        vendor[12] = '\0';

        return result_success(vendor);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_arch() {
    #if defined(_WIN32)
        SYSTEM_INFO system_info;

        GetSystemInfo(&system_info);

        char* arch;

        // copied from "winnt.h" for max support
        switch (system_info.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_INTEL: arch = "x86"; break;
            case PROCESSOR_ARCHITECTURE_MIPS: arch = "MIPS"; break;
            case PROCESSOR_ARCHITECTURE_ALPHA: arch = "Alpha"; break;
            case PROCESSOR_ARCHITECTURE_PPC: arch = "PowerPC"; break;
            case PROCESSOR_ARCHITECTURE_SHX: arch = "Hitachi SHx"; break;
            case PROCESSOR_ARCHITECTURE_ARM: arch = "ARM"; break;
            case PROCESSOR_ARCHITECTURE_IA64: arch = "IA-64"; break;
            case PROCESSOR_ARCHITECTURE_ALPHA64: arch = "Alpha64"; break;
            case PROCESSOR_ARCHITECTURE_MSIL: arch = "MSIL"; break;
            case PROCESSOR_ARCHITECTURE_AMD64: arch = "x64"; break;
            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64: arch = "x86 on x64"; break;
            case PROCESSOR_ARCHITECTURE_NEUTRAL: arch = "Neutral"; break;
            case PROCESSOR_ARCHITECTURE_ARM64: arch = "ARM64"; break;
            case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64: arch = "ARM32 on x64"; break;
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64: arch = "x86 on ARM64"; break;

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

            void* clock_speed = malloc(sizeof(double));

            if (!clock_speed) {
                return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
            }

            *(double*) clock_speed = speed / 1000.0;

            return result_success(clock_speed);
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
        return get_cpu_processor_info(count_cpu_cores_callback);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif
}

result_t* get_cpu_logical_processors_count() {
    #if defined(_WIN32)
        return get_cpu_processor_info(count_cpu_logical_processors_callback);
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

    uint32_t count_cpu_cores_callback(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info) {
        return 1;
    }

    uint32_t count_cpu_logical_processors_callback(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info) {
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
    map_insert(cpu_info_map, "clock_speed", strdup(buffer));

    sprintf(buffer, "%zu", cpu_info.core_count);
    map_insert(cpu_info_map, "core_count", strdup(buffer));

    sprintf(buffer, "%zu", cpu_info.logical_processors_count);
    map_insert(cpu_info_map, "logical_processors_count", strdup(buffer));

    free(buffer);

    return result_success(cpu_info_map);
}

void free_cpu_info(cpu_info_t* info) {
    if (!info) {
        return;
    }

    free(info->name);
    info->name = NULL;

    free(info->vendor);
    info->vendor = NULL;

    free(info->arch);
    info->arch = NULL;

    free(info);
    info = NULL;
}