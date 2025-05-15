#include "installer.h"

int main(int argc, const char* argv[]) {
    if (BENJI_STRING_EQUALS(argv[1], "install")) {
        if (argc < 4) {
            return EXIT_FAILURE;
        }

        int error_code = install_service(argv[2], argv[3]);

        if (error_code < 0) {
            return error_code;
        }

        return start_service();
    }
    else if (BENJI_STRING_EQUALS(argv[1], "uninstall")) {
        return uninstall_service();
    }
    else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#if defined(_WIN32)
    int install_service(const char* executable_filepath, const char* config_filepath) {
        SC_HANDLE sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

        if (!sc_manager) {
            return BENJI_ERROR_UNABLE_TO_OPEN_SC_MANAGER;
        }

        SC_HANDLE service = CreateServiceA(
            sc_manager,
            BENJI_SERVICE_NAME,
            NULL,
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_DEMAND_START,
            SERVICE_ERROR_NORMAL,
            executable_filepath,
            NULL, NULL, NULL, NULL, NULL
        );

        if (!service) {
            CloseServiceHandle(sc_manager);

            return BENJI_ERROR_UNABLE_TO_CREATE_SERVICE;
        }

        CloseServiceHandle(service);
        CloseServiceHandle(sc_manager);

        write_config_to_registry(config_filepath);
    }

    int start_service() {
        SC_HANDLE sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

        if (!sc_manager) {
            return BENJI_ERROR_UNABLE_TO_OPEN_SC_MANAGER;
        }

        SC_HANDLE service = OpenServiceA(sc_manager, BENJI_SERVICE_NAME, SERVICE_START);

        if (!service) {
            CloseServiceHandle(sc_manager);

            return BENJI_ERROR_UNABLE_TO_OPEN_SERVICE;
        }

        if (!StartServiceA(service, 0, NULL)) {
            CloseServiceHandle(service);
            CloseServiceHandle(sc_manager);

            return BENJI_ERROR_UNABLE_TO_START_SERVICE;
        }

        CloseServiceHandle(service);
        CloseServiceHandle(sc_manager);

        return 0;
    }

    int uninstall_service() {
        SC_HANDLE sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

        if (!sc_manager) {
            return BENJI_ERROR_UNABLE_TO_OPEN_SC_MANAGER;
        }

        SC_HANDLE service = OpenServiceA(sc_manager, BENJI_SERVICE_NAME, SERVICE_STOP | DELETE);

        if (!service) {
            CloseServiceHandle(sc_manager);

            return BENJI_ERROR_UNABLE_TO_OPEN_SERVICE;
        }

        SERVICE_STATUS service_status;

        if (!ControlService(service, SERVICE_CONTROL_STOP, &service_status)) {
            CloseServiceHandle(service);
            CloseServiceHandle(sc_manager);

            return BENJI_ERROR_UNABLE_TO_START_SERVICE;
        }

        if (!DeleteService(service)) {
            CloseServiceHandle(service);
            CloseServiceHandle(sc_manager);

            return BENJI_ERROR_UNABLE_TO_DELETE_SERVICE;
        }

        CloseServiceHandle(service);
        CloseServiceHandle(sc_manager);

        return 0;
    }

    int write_config_to_registry(const char* config_filepath) {
        HKEY hkey;

        HRESULT hresult = RegCreateKeyExA(
            HKEY_LOCAL_MACHINE,
            BENJI_CONFIG_REGISTRY_PATH,
            0, NULL, 0,
            KEY_SET_VALUE,
            NULL, &hkey, NULL
        );

        if (FAILED(hresult)) {
            return BENJI_ERROR_UNABLE_TO_CREATE_REGISTRY_KEY;
        }

        hresult = RegSetValueExA(
            hkey,
            BENJI_CONFIG_REGISTRY_KEY,
            0, REG_SZ,
            (const unsigned char*) config_filepath,
            (unsigned long) (strlen(config_filepath) + 1)
        );

        if (FAILED(hresult)) {
            RegCloseKey(hkey);

            return BENJI_ERROR_UNABLE_TO_SET_REGISTRY_KEY_VALUE;
        }

        RegCloseKey(hkey);

        return 0;
    }
#endif