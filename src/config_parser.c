#include "include/config_parser.h"

result_t* open_config(const char* filepath) {
    FILE* file = fopen(filepath, "r");

    if (!file) {
        return result_error(ERROR_FILE_NOT_FOUND, BENJI_ERROR_PACKET, "Unable to open config file '%s'", filepath);
    }

    char error_buffer[BENJI_BASIC_STRING_LENGTH];

    toml_table_t* toml_table = toml_parse_file(file, error_buffer, BENJI_BASIC_STRING_LENGTH);

    if (!toml_table) {
        return result_error(-1, BENJI_ERROR_PACKET, "Error parsing config file: '%s'", error_buffer);
    }

    return result_success(toml_table);
}