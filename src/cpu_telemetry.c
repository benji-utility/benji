#include "include/telemetry/cpu_telemetry.h"

result_t* get_cpu_info() {
    cpu_info_t* cpu_info = malloc(sizeof(cpu_info_t));

    if (!cpu_info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    get_telemetry_info_string(
        cpu_info,
        cpu_info->name,
        get_cpu_name,
        free_cpu_info
    );

    get_telemetry_info_string(
        cpu_info,
        cpu_info->vendor,
        get_cpu_vendor,
        free_cpu_info
    );

    get_telemetry_info_string(cpu_info,
        cpu_info->architecture,
        get_cpu_architecture,
        free_cpu_info
    );

    get_telemetry_info_double(
        cpu_info,
        cpu_info->clock_speed,
        get_cpu_clock_speed,
        free_cpu_info
    );

    get_telemetry_info_integer(
        cpu_info,
        cpu_info->core_count,
        get_cpu_core_count,
        free_cpu_info
    );

    get_telemetry_info_integer(
        cpu_info,
        cpu_info->logical_processors_count,
        get_cpu_logical_processors_count,
        free_cpu_info
    );

    return result_success(cpu_info);
}

result_t* get_cpu_name() {
    #if defined(_WIN32)
        return _get_cpu_name_windows();
    #elif defined(__linux__)
        return _get_cpu_name_linux();
    #endif
}

result_t* get_cpu_vendor() {
    #if defined(_WIN32)
        return _get_cpu_vendor_windows();
    #elif defined(__linux__)
        return _get_cpu_vendor_linux();
    #endif
}

result_t* get_cpu_architecture() {
   #if defined(_WIN32)
        return _get_cpu_architecture_windows();
    #elif defined(__linux__)
        return _get_cpu_architecture_linux();
    #endif
}

result_t* get_cpu_clock_speed() {
    #if defined(_WIN32)
        return _get_cpu_clock_speed_windows();
    #elif defined(__linux__)
        return _get_cpu_clock_speed_linux();
    #endif
}

result_t* get_cpu_core_count() {
    #if defined(_WIN32)
        return _get_cpu_core_count_windows();
    #elif defined(__linux__)
        return _get_cpu_core_count_linux();
    #endif
}

result_t* get_cpu_logical_processors_count() {
    #if defined(_WIN32)
        return _get_cpu_logical_processors_count_windows();
    #elif defined(__linux__)
        return _get_cpu_logical_processors_count_linux();
    #endif
}

#if defined(_WIN32)
    result_t* _get_cpu_name_windows() {
        int cpuid_info[BENJI_CPUID_BUFFER_LENGTH];

        char* cpu_name = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!cpu_name) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        memset(cpu_name, 0, BENJI_BASIC_STRING_LENGTH);

        for (int i = 0; i < BENJI_CPUID_CPU_NAME_SECTIONS_COUNT; ++i) {
            __cpuid(cpuid_info, BENJI_CPUID_CPU_NAME_START + i);
            memcpy(cpu_name + (i * 16), cpuid_info, sizeof(cpuid_info));
        }

        // TODO: maybe make this terminate after the last character?
        cpu_name[BENJI_BASIC_STRING_LENGTH - 1] = '\0';

        return result_success(cpu_name);
    }

    result_t* _get_cpu_vendor_windows() {
        int cpu_info[BENJI_CPUID_BUFFER_LENGTH];

        char* cpu_vendor = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!cpu_vendor) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        cpu_vendor[0] = '\0';

        __cpuid(cpu_info, 0);

        // TODO: make these not hardcoded constants
        memcpy(cpu_vendor, &cpu_info[1], 4);
        memcpy(cpu_vendor + 4, &cpu_info[3], 4);
        memcpy(cpu_vendor + 8, &cpu_info[2], 4);

        cpu_vendor[12] = '\0';

        return result_success(cpu_vendor);
    }

    result_t* _get_cpu_architecture_windows() {
        SYSTEM_INFO system_info;

        GetSystemInfo(&system_info);

        char* cpu_architecture;

        // copied from "winnt.h" for max support
        switch (system_info.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_INTEL: cpu_architecture = "x86"; break;
            case PROCESSOR_ARCHITECTURE_MIPS: cpu_architecture = "MIPS"; break;
            case PROCESSOR_ARCHITECTURE_ALPHA: cpu_architecture = "Alpha"; break;
            case PROCESSOR_ARCHITECTURE_PPC: cpu_architecture = "PowerPC"; break;
            case PROCESSOR_ARCHITECTURE_SHX: cpu_architecture = "Hitachi SHx"; break;
            case PROCESSOR_ARCHITECTURE_ARM: cpu_architecture = "ARM"; break;
            case PROCESSOR_ARCHITECTURE_IA64: cpu_architecture = "IA-64"; break;
            case PROCESSOR_ARCHITECTURE_ALPHA64: cpu_architecture = "Alpha64"; break;
            case PROCESSOR_ARCHITECTURE_MSIL: cpu_architecture = "MSIL"; break;
            case PROCESSOR_ARCHITECTURE_AMD64: cpu_architecture = "x64"; break;
            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64: cpu_architecture = "x86 on x64"; break;
            case PROCESSOR_ARCHITECTURE_NEUTRAL: cpu_architecture = "Neutral"; break;
            case PROCESSOR_ARCHITECTURE_ARM64: cpu_architecture = "ARM64"; break;
            case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64: cpu_architecture = "ARM32 on x64"; break;
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64: cpu_architecture = "x86 on ARM64"; break;

            case PROCESSOR_ARCHITECTURE_UNKNOWN: // make this one fall through because i dont wanna deal with edge cases
            default: cpu_architecture = "???"; break;
        }

        return result_success(cpu_architecture);
    }

    result_t* _get_cpu_clock_speed_windows() {
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
    }

    result_t* _get_cpu_core_count_windows() {
        return _get_cpu_processor_info(_count_cpu_cores_callback);
    }

    result_t* _get_cpu_logical_processors_count_windows() {
        return _get_cpu_processor_info(_count_cpu_logical_processors_callback);
    }

    result_t* _get_cpu_processor_info(processor_info_callback_t callback) {
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

    uint32_t _count_cpu_cores_callback(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info) {
        return 1;
    }

    uint32_t _count_cpu_logical_processors_callback(SYSTEM_LOGICAL_PROCESSOR_INFORMATION* info) {
        return __popcnt(info->ProcessorMask);
    }
#elif defined(__linux__)
    result_t* _get_cpu_name_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_cpu_vendor_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_cpu_architecture_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_cpu_clock_speed_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_cpu_core_count_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_cpu_logical_processors_count_linux() {
        // TODO: add linux stuff
    }
#endif

result_t* cpu_info_to_map(const cpu_info_t cpu_info) {
    map_t* cpu_info_map = map_init();

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    buffer[0] = '\0';

    map_insert(cpu_info_map, "name", cpu_info.name);
    map_insert(cpu_info_map, "vendor", cpu_info.vendor);
    map_insert(cpu_info_map, "arch", cpu_info.architecture);

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

    free(info->architecture);
    info->architecture = NULL;

    free(info);
    info = NULL;
}