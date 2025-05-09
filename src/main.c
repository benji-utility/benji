#if defined(_WIN32)
    #include "include/service.h"
#elif defined(__linux__)
    #include "include/daemon.h"
#endif

#include "include/config_parser.h"
#include "include/logger.h"
#include "include/result.h"

int main(int argc, const char* argv[]) {
    result_t* config = open_config(BENJI_CONFIG_PATH);

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