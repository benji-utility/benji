#if defined(_WIN32)
    #include "include/service.h"
#elif defined(__linux__)
    #include "include/daemon.h"
#endif

#include "include/logger.h"
#include "include/result.h"

int main(int argc, const char* argv[]) {
    // i dont really understand how this macro witchcraft works. but it does, so im not gonna question it
    result_t* config_result = open_config(BENJI_STRINGIFY_VALUE_OF(BENJI_CONFIG_PATH));

    if (config_result->is_error) {
        result_error_payload_t config_result_error = result_unwrap_error(config_result);

        log_error_info(config_result_error.message);

        return config_result_error.code;
    }

    toml_table_t* config = (toml_table_t*) result_unwrap_value(config_result);
    config_details_t config_details = get_details_from_config(config);

    collect_server_details(config_details);

    #if defined(_WIN32)
        winsock_init();

        SERVICE_TABLE_ENTRY service_table[] = {
            {BENJI_SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) service_main},
            {NULL, NULL}
        };

        if (!StartServiceCtrlDispatcher(service_table)) {
            return GetLastError();
        }
    #elif defined(__linux__)
        spawn_daemon();
    #endif

    return EXIT_SUCCESS;
}