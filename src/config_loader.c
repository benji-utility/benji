#include "include/config_loader.h"

result_t* open_config() {
    char config_path[BENJI_BASIC_STRING_LENGTH];
    unsigned long config_path_length = sizeof(config_path);

    #if defined(_WIN32)
        HRESULT hresult = RegGetValueA(
            HKEY_LOCAL_MACHINE,
            BENJI_CONFIG_REGISTRY_PATH,
            BENJI_CONFIG_REGISTRY_KEY,
            RRF_RT_REG_SZ | RRF_NOEXPAND,
            NULL,
            (void*) config_path,
            &config_path_length
        );

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegGetValueA() failed");
        }

        log_message(BENJI_LOG_LEVEL_INFO, "Collected config path from Windows registry: '%s'", config_path);
    #elif defined(__linux__)
        /* TODO: add linux stuff */
    #endif

    FILE* file = fopen(config_path, "r");

    if (!file) {
        return result_error(-1, BENJI_ERROR_PACKET, "Unable to open config file '%s'", config_path);
    }

    char error_buffer[BENJI_BASIC_STRING_LENGTH];

    toml_table_t* toml_table = toml_parse_file(file, error_buffer, BENJI_BASIC_STRING_LENGTH);

    fclose(file);

    if (!toml_table) {
        return result_error(-1, BENJI_ERROR_PACKET, "Error parsing config file: '%s'", error_buffer);
    }

    log_message(BENJI_LOG_LEVEL_INFO, "Loaded config succesfully");

    return result_success(toml_table);
}

config_details_t get_details_from_config(toml_table_t* config) {
    // set initial defaults
    config_details_t config_details = (config_details_t) {
        .server_config_details.hostname = BENJI_DEFAULT_SERVER_HOSTNAME,
        .server_config_details.port = BENJI_DEFAULT_SERVER_PORT,

        .telemetry_config_details.max_processes = BENJI_DEFAULT_MAX_PROCESSES
    };

    toml_table_t* server_table = toml_table_table(config, "server");
    toml_table_t* telemetry_table = toml_table_table(config, "telemetry");

    if (server_table) {
        toml_value_t hostname = toml_table_string(server_table, "hostname");
        toml_value_t port = toml_table_int(server_table, "port");

        if (hostname.ok) {
            config_details.server_config_details.hostname = hostname.u.s;
        }

        if (port.ok) {
            config_details.server_config_details.port = port.u.i;
        }
    }

    if (telemetry_table) {
        toml_value_t max_processes = toml_table_int(server_table, "max_processes");

        if (max_processes.ok) {
            config_details.telemetry_config_details.max_processes = max_processes.u.i;
        }
    }

    return config_details;
}