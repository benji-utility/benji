#include "include/config_loader.h"

result_t* open_config() {
    #if defined(_WIN32)
        HKEY hkey;

        char config_path[BENJI_BASIC_STRING_LENGTH];
        unsigned long config_path_length = sizeof(config_path);

        HRESULT hresult = RegOpenKeyExA(
            HKEY_LOCAL_MACHINE,
            BENJI_CONFIG_REGISTRY_PATH,
            0, KEY_READ, &hkey
        );

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegOpenKeyEx() failed");
        }

        unsigned long type = REG_SZ;

        hresult = RegQueryValueExA(
            hkey, BENJI_CONFIG_REGISTRY_KEY, NULL, &type, (unsigned char*) config_path, &config_path_length
        );

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegQueryValueEx() failed");
        }

        hresult = RegCloseKey(hkey);

        if (FAILED(hresult)) {
            return result_error(hresult, BENJI_ERROR_PACKET, "RegCloseKey() failed");
        }

        config_path[config_path_length] = '\0';

        log_message(BENJI_LOG_LEVEL_INFO, "Collected config path from Windows registry: '%s'", config_path);
    #endif

    FILE* file = fopen(config_path, "r");

    if (!file) {
        return result_error(ERROR_FILE_NOT_FOUND, BENJI_ERROR_PACKET, "Unable to open config file '%s'", config_path);
    }

    char error_buffer[BENJI_BASIC_STRING_LENGTH];

    toml_table_t* toml_table = toml_parse_file(file, error_buffer, BENJI_BASIC_STRING_LENGTH);

    if (!toml_table) {
        return result_error(-1, BENJI_ERROR_PACKET, "Error parsing config file: '%s'", error_buffer);
    }

    log_message(BENJI_LOG_LEVEL_INFO, "Loaded config succesfully");

    return result_success(toml_table);
}

config_details_t get_details_from_config(toml_table_t* config) {
    config_details_t config_details;

    toml_table_t* server_table = toml_table_table(config, "server");

    if (server_table) {
        toml_value_t hostname = toml_table_string(server_table, "hostname");
        toml_value_t port = toml_table_int(server_table, "port");

        config_details.server_config.hostname = hostname.ok ? hostname.u.s : BENJI_DEFAULT_SERVER_HOSTNAME;
        config_details.server_config.port = port.ok ? port.u.i : BENJI_DEFAULT_SERVER_PORT;
    }
    else {
        // unable to get the server config table, just use the default values
        config_details.server_config.hostname = BENJI_DEFAULT_SERVER_HOSTNAME;
        config_details.server_config.port = BENJI_DEFAULT_SERVER_PORT;
    }

    return config_details;
}