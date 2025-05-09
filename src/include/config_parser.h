#ifndef __BENJI_CONFIG_PARSER_H
#define __BENJI_CONFIG_PARSER_H

#include "toml-c/toml.h"

#ifndef BENJI_CONFIG_PATH
    #define BENJI_CONFIG_PATH ""
#endif

#include "utils.h"
#include "result.h"

result_t* open_config(const char* filepath);

#endif