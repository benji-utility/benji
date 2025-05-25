#if defined(_WIN32)
    #include "include/service.h"
#elif defined(__linux__)
    #include "include/daemon.h"
#endif

#include "include/logger.h"
#include "include/result.h"

int main(int argc, const char* argv[]) {
    result_t* config_result = open_config();

    if (config_result->is_error) {
        result_error_payload_t config_result_error = result_unwrap_error(config_result);

        log_error_payload(BENJI_LOG_LEVEL_ERROR, config_result_error);

        return EXIT_FAILURE;
    }

    toml_table_t* toml_config = (toml_table_t*) result_unwrap_value(config_result);

    config_details_t config_details = get_details_from_config(toml_config);

    toml_free(toml_config);

    collect_server_details(config_details);

    #if defined(_WIN32)
        winsock_init();

        SERVICE_TABLE_ENTRY service_table[] = {
            {TEXT(BENJI_SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION) service_main},
            {NULL, NULL}
        };

        if (!StartServiceCtrlDispatcher(service_table)) {
            // TODO: make this a result
            return GetLastError();
        }
    #elif defined(__linux__)
        spawn_daemon();
    #endif

    return EXIT_SUCCESS;
}