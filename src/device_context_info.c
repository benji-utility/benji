#include "include/hardware/device_context_info.h"

result_t* get_device_context_info() {
    device_context_info_t* info = malloc(sizeof(device_context_info_t));

    result_t* device_name_result = get_device_context_device_name();
    return_if_error(device_name_result);
    info->device_name = strdup((char*) result_unwrap_value(device_name_result));
    strtrim(info->device_name);

    result_t* operating_system_result = get_device_context_operating_system_info(BENJI_OPERATING_SYSTEM_VERSION_NAME);
    return_if_error(operating_system_result);
    info->operating_system = strdup((char*) result_unwrap_value(operating_system_result));
    strtrim(info->operating_system);

    result_t* operating_system_version_result = get_device_context_operating_system_info(BENJI_OPERATING_SYSTEM_VERSION_NUMBER);
    return_if_error(operating_system_version_result);
    info->operating_system_version = strdup((char*) result_unwrap_value(operating_system_version_result));
    strtrim(info->operating_system_version);

    info->hostname = "";

    return result_success(info);
}

result_t* get_device_context_device_name() {
    #if defined(_WIN32)
        unsigned long device_name_size = 0;

        GetComputerNameEx(ComputerNameDnsHostname, NULL, &device_name_size);
        if (GetLastError() != ERROR_MORE_DATA) {
            device_name_size = BENJI_BASIC_STRING_LENGTH; // default to something definitely long enough
        }

        wchar_t* device_name = malloc(BENJI_CAPACITY(device_name_size, wchar_t));

        if (!device_name) {
            return result_error(-1, "malloc() failed", BENJI_ERROR_PACKET);
        }

        // get the name assigned through Windows, not what comes from the BIOS
        if (GetComputerNameEx(ComputerNameDnsHostname, device_name, &device_name_size)) {
            return result_success(wcharp_to_charp(device_name));
        }
        else {
            return result_error(
                GetLastError(),
                "GetComputerNameEx() failed",
                BENJI_ERROR_PACKET
            );
        }
    #elif defined(__linux__)
        // TODO: add linux stuff
    #endif
}

result_t* get_device_context_operating_system_info(enum BENJI_OPERATING_SYSTEM_VERSION_INFO_TYPE version_info_type) {
    #if defined(_WIN32)
        char* operating_system = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        HMODULE hmodule = GetModuleHandle(TEXT("ntdll.dll"));

        if (hmodule) {
            rtl_get_version_t rtl_get_version = (rtl_get_version_t) GetProcAddress(hmodule, "RtlGetVersion");

            if (rtl_get_version != NULL) {
                RTL_OSVERSIONINFOW rtl_os_version_info = {0};
                rtl_os_version_info.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

                if (rtl_get_version(&rtl_os_version_info) == 0) {
                    switch (version_info_type) {
                        case BENJI_OPERATING_SYSTEM_VERSION_NAME: {
                            operating_system = get_windows_name_from_version(
                                rtl_os_version_info.dwMajorVersion,
                                rtl_os_version_info.dwMinorVersion,
                                rtl_os_version_info.dwBuildNumber
                            );

                            break;
                        }

                        case BENJI_OPERATING_SYSTEM_VERSION_NUMBER: {
                            sprintf(
                                operating_system,
                                "%lu.%lu (Build %lu)",
                                rtl_os_version_info.dwMajorVersion,
                                rtl_os_version_info.dwMinorVersion,
                                rtl_os_version_info.dwBuildNumber
                            );

                            break;
                        }
                    }
                }
                else {
                    return result_error(-1, "Failed to get OS version info", BENJI_ERROR_PACKET);
                }
            }
            else {
                return result_error(-1, "GetProcAddress() failed", BENJI_ERROR_PACKET);
            }
        }
        else {
            return result_error(GetLastError(), "GetModuleHandle() failed", BENJI_ERROR_PACKET);
        }

        return result_success(operating_system);
    #elif defined(__linux__)
        // TODO: add linux stuff
    #endif
}

result_t* get_device_context_operating_system_version();

result_t* get_device_context_hostname();

#ifdef _WIN32
    char* get_windows_name_from_version(unsigned long major_version, unsigned long minor_version, unsigned long build_number) {
        if (major_version == 10 && minor_version == 0) {
            if (build_number >= 22000) {
                return "Windows 11";
            }

            return "Windows 10";
        }
        else if (major_version == 6 && minor_version == 3) {
            return "Windows 8.1";
        }
        else if (major_version == 6 && minor_version == 2) {
            return "Windows 8";
        }
        else if (major_version == 6 && minor_version == 1) {
            return "Windows 7";
        }
        else if (major_version == 6 && minor_version == 0) {
            return "Windows Vista";
        }
        else if (major_version == 5 && minor_version == 1) {
            return "Windows XP";
        }
        else {
            return "???";
        }
    }
#endif

result_t* device_context_info_to_map(device_context_info_t device_context_info) {
    map_t* device_context_info_map = map_init();

    map_insert(device_context_info_map, "device_name", device_context_info.device_name);
    map_insert(device_context_info_map, "operating_system", device_context_info.operating_system);
    map_insert(device_context_info_map, "operating_system_version", device_context_info.operating_system_version);
    map_insert(device_context_info_map, "hostname", device_context_info.hostname);

    return result_success(device_context_info_map);
}