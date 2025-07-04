#ifndef __BENJI_CONFIG_LOADER_H
#define __BENJI_CONFIG_LOADER_H

#include "toml-c/toml.h"

#include "utils.h"
#include "logger.h"

#ifndef BENJI_CONFIG_REGISTRY_PATH
    #define BENJI_CONFIG_REGISTRY_PATH "SYSTEM\\CurrentControllerSet\\Services\\" BENJI_SERVICE_NAME
#endif

#ifndef BENJI_CONFIG_REGISTRY_KEY
    #define BENJI_CONFIG_REGISTRY_KEY "ConfigPath"
#endif

#ifndef BENJI_DEFAULT_SERVER_HOSTNAME
    #define BENJI_DEFAULT_SERVER_HOSTNAME "127.0.0.1"
#endif

#ifndef BENJI_DEFAULT_SERVER_PORT
    #define BENJI_DEFAULT_SERVER_PORT (8000)
#endif

typedef struct _BENJI_CONFIG_DETAILS {
    struct _BENJI_SERVER_CONFIG {
        const char* hostname;
        uint16_t port; // 1-65535
    } server_config;
} config_details_t;

#include "utils.h"
#include "result.h"

result_t* open_config();

// no need to return a result, as we have default config values
config_details_t get_details_from_config(toml_table_t* config);

#endif