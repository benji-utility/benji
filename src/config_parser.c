#include "include/config_parser.h"

result_t* open_config(const char* filepath) {
    FILE* file = fopen(filepath, "r");

    if (!file) {
        return result_error(ERROR_FILE_NOT_FOUND, "Unable to open config file '%s'", BENJI_ERROR_PACKET, filepath);
    }

    char error_buffer[BENJI_BASIC_STRING_LENGTH];

    toml_table_t* toml_table = toml_parse(file, error_buffer, BENJI_BASIC_STRING_LENGTH);

    if (!toml_table) {
        return result_error(-1, "Error parsing config file: '%s'", BENJI_ERROR_PACKET, error_buffer);
    }
}