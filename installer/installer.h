#ifndef __BENJI_INSTALLER_H
#define __BENJI_INSTALLER_H

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #include <stdio.h>
    #include <stdbool.h>
    #include <string.h>
    #include <windows.h>

    #include "error.h"

    #ifndef BENJI_SERVICE_NAME
        #define BENJI_SERVICE_NAME "BenjiService"
    #endif

    #ifndef BENJI_CONFIG_REGISTRY_PATH
        #define BENJI_CONFIG_REGISTRY_PATH "SYSTEM\\CurrentControllerSet\\Services\\" BENJI_SERVICE_NAME
    #endif

    #ifndef BENJI_CONFIG_REGISTRY_KEY
        #define BENJI_CONFIG_REGISTRY_KEY "ConfigPath"
    #endif

    #ifndef BENJI_STRING_EQUALS
        #define BENJI_STRING_EQUALS(a, b) (strcmp(a, b) == 0)
    #endif

    int install_service(const char* executable_filepath, const char* config_filepath);
    int start_service();
    int uninstall_service();

    int write_config_to_registry(const char* config_filepath);
#endif

#endif