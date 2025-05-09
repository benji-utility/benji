#ifndef __BENJI_CONFIG_PARSER_H
#define __BENJI_CONFIG_PARSER_H

#include <toml-c/toml.h>

#include "utils.h"
#include "result.h"

result_t* open_config(const char* filepath);

#endif