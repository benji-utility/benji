#include "installer.h"

int main(int argc, const char* argv[]) {
    int return_code = 0;

    if (argc < 2) {
        return_code = BENJI_ERROR_INSUFFICIENT_ARGUMENT_COUNT;

        goto end;
    }

    if (BENJI_STRING_EQUALS(argv[1], "help")) {
        printf("Help Menu:\n");
        printf("\t%s help\n", argv[0]);
        printf("\t%s install <path to service executable> <path to config.toml>\n", argv[0]);
        printf("\t%s uninstall\n", argv[0]);

        return EXIT_SUCCESS;
    }
    else if (BENJI_STRING_EQUALS(argv[1], "install")) {
        if (argc < 4) {
            return_code = BENJI_ERROR_INSUFFICIENT_ARGUMENT_COUNT;

            goto end;
        }

        int error_code = install_service(argv[2], argv[3]);

        return_code = (error_code < 0) ? error_code : start_service();
    }
    else if (BENJI_STRING_EQUALS(argv[1], "uninstall")) {
        return_code = uninstall_service();
    }
    else {
        return_code = BENJI_ERROR_UNKNOWN_COMMAND;
    }

    end:
    if (return_code == BENJI_ERROR_NONE) {
        printf("Success\n");

        return EXIT_SUCCESS;
    }
    else {
        printf("Failed with error code %i\n", return_code);

        return EXIT_FAILURE;
    }
}

#if defined(_WIN32)
    int install_service(const char* executable_filepath, const char* config_filepath) {
        SC_HANDLE sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

        if (!sc_manager) {
            return BENJI_ERROR_UNABLE_TO_OPEN_SC_MANAGER;
        }

        SC_HANDLE service;

        while (true) {
            service = CreateServiceA(
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

            // jank spaghetti code funkery (which can mean whatever tf you want it to mean)
            if (!service) {
                int last_error = GetLastError();

                if (last_error == ERROR_SERVICE_EXISTS) {
                    int uninstall_return_code = uninstall_service();

                    if (uninstall_return_code != BENJI_ERROR_NONE) {
                        CloseServiceHandle(sc_manager);

                        return uninstall_return_code;
                    }
                }
                else {
                    CloseServiceHandle(sc_manager);

                    return BENJI_ERROR_UNABLE_TO_CREATE_SERVICE;
                }
            }
            else {
                break;
            }
        }

        CloseServiceHandle(service);
        CloseServiceHandle(sc_manager);

        // this can come after the service creation because the
        // config is loaded only after the service is actually started
        return write_config_to_registry(config_filepath);
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

        return BENJI_ERROR_NONE;
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

        return BENJI_ERROR_NONE;
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

        return BENJI_ERROR_NONE;
    }
#endif