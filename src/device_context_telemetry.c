#include "include/telemetry/device_context_telemetry.h"

result_t* get_device_context_info() {
    device_context_info_t* device_context_info = malloc(sizeof(device_context_info_t));

    if (!device_context_info) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    get_telemetry_info_string(
        device_context_info,
        device_context_info->device_name,
        get_device_context_device_name,
        free_device_context_info
    );

    get_telemetry_info_string(
        device_context_info,
        device_context_info->operating_system_name,
        get_device_context_operating_system_version_info,
        free_device_context_info,
        BENJI_OPERATING_SYSTEM_VERSION_NAME
    );

    get_telemetry_info_string(
        device_context_info,
        device_context_info->operating_system_version,
        get_device_context_operating_system_version_info,
        free_device_context_info,
        BENJI_OPERATING_SYSTEM_VERSION_NUMBER
    );

    get_telemetry_info_string(
        device_context_info,
        device_context_info->hostname,
        get_device_context_hostname,
        free_device_context_info
    );

    return result_success(device_context_info);
}

result_t* get_device_context_device_name() {
    #if defined(_WIN32)
        return _get_device_context_device_name_windows();
    #elif defined(__linux__)
        return _get_device_context_device_name_linux();
    #endif
}

result_t* get_device_context_operating_system_version_info(os_version_info_type_t version_info_type) {
    #if defined(_WIN32)
        return _get_device_context_operating_system_version_info_windows(version_info_type);
    #elif defined(__linux__)
        return _get_device_context_operating_system_version_info_linux(version_info_type);
    #endif
}

result_t* get_device_context_hostname() {
    #if defined(_WIN32)
        return _get_device_context_hostname_windows();
    #elif defined(__linux__)
        return _get_device_context_hostname_linux();
    #endif
}

#if defined(_WIN32)
    result_t* _get_device_context_device_name_windows() {
        HKEY hkey;

        HRESULT hresult = RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            TEXT("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ComputerName"),
            0, KEY_READ, &hkey
        );

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegOpenKeyEx() failed");
        }

        wchar_t device_name[MAX_COMPUTERNAME_LENGTH + 1];
        unsigned long device_name_size = sizeof(device_name);

        unsigned long type = REG_SZ;

        hresult = RegQueryValueEx(hkey, TEXT("ComputerName"), NULL, &type, (unsigned char*) device_name, &device_name_size);

        if (FAILED(hresult)) {
            RegCloseKey(hkey);

            // dont error if RegCloseKey failed, RegQueryValueEx() matters more

            return result_error(hresult, BENJI_ERROR_PACKET, "RegQueryValueEx() failed");
        }

        hresult = RegCloseKey(hkey);

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegCloseKey() failed");
        }

        return result_success(wcharp_to_charp(device_name));
    }

    result_t* _get_device_context_operating_system_version_info_windows(os_version_info_type_t version_info_type) {
        char* operating_system_version_info = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!operating_system_version_info) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        HMODULE hmodule = GetModuleHandle(TEXT("ntdll.dll"));

        if (hmodule) {
            rtl_get_version_t rtl_get_version = (rtl_get_version_t) GetProcAddress(hmodule, "RtlGetVersion");

            if (rtl_get_version != NULL) {
                RTL_OSVERSIONINFOW rtl_os_version_info = {0};
                rtl_os_version_info.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

                if (rtl_get_version(&rtl_os_version_info) == 0) {
                    switch (version_info_type) {
                        case BENJI_OPERATING_SYSTEM_VERSION_NAME: {
                            operating_system_version_info = _get_windows_name_from_version(
                                rtl_os_version_info.dwMajorVersion,
                                rtl_os_version_info.dwMinorVersion,
                                rtl_os_version_info.dwBuildNumber
                            );

                            break;
                        }

                        case BENJI_OPERATING_SYSTEM_VERSION_NUMBER: {
                            sprintf(
                                operating_system_version_info,
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
                    return result_error(GetLastError(), BENJI_ERROR_PACKET, "Failed to get OS version info");
                }
            }
            else {
                return result_error(GetLastError(), BENJI_ERROR_PACKET, "GetProcAddress() failed");
            }
        }
        else {
            return result_error(GetLastError(), BENJI_ERROR_PACKET, "GetModuleHandle() failed");
        }

        return result_success(operating_system_version_info);
    }

    result_t* _get_device_context_hostname_windows() {
        unsigned long hostname_size = 0;

        GetComputerNameEx(ComputerNameDnsHostname, NULL, &hostname_size);

        if (GetLastError() != ERROR_MORE_DATA) {
            hostname_size = BENJI_BASIC_STRING_LENGTH; // default to something definitely long enough
        }

        wchar_t* hostname = malloc(BENJI_CAPACITY(hostname_size, wchar_t));

        if (!hostname) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        if (GetComputerNameEx(ComputerNameDnsHostname, hostname, &hostname_size)) {
            return result_success(wcharp_to_charp(hostname));
        }
        else {
            return result_error(GetLastError(), BENJI_ERROR_PACKET, "GetComputerNameEx() failed");
        }
    }

    char* _get_windows_name_from_version(unsigned long major_version, unsigned long minor_version, unsigned long build_number) {
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
#elif defined(__linux__)
    result_t* _get_device_context_device_name_linux() {
        // TODO: add linux stuff
    }

    result_t* _get_device_context_operating_system_version_info_linux(os_version_info_type_t version_info_type) {
        // TODO: add linux stuff
    }

    result_t* _get_device_context_hostname_linux() {
        // TODO: add linux stuff
    }
#endif

result_t* device_context_info_to_map(const device_context_info_t device_context_info) {
    map_t* device_context_info_map = map_init();

    map_insert(device_context_info_map, "device_name", device_context_info.device_name);
    map_insert(device_context_info_map, "operating_system_name", device_context_info.operating_system_name);
    map_insert(device_context_info_map, "operating_system_version", device_context_info.operating_system_version);
    map_insert(device_context_info_map, "hostname", device_context_info.hostname);

    return result_success(device_context_info_map);
}

void free_device_context_info(device_context_info_t* info) {
    if (!info) {
        return;
    }

    free(info->device_name);
    info->device_name = NULL;

    free(info->operating_system_name);
    info->operating_system_name = NULL;

    free(info->operating_system_version);
    info->operating_system_version = NULL;

    free(info->hostname);
    info->hostname = NULL;

    free(info);
    info = NULL;
}