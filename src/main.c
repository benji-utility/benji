#if defined(BENJI_IS_ON_WINDOWS)
    #include "include/service.h"
#elif defined(BENJI_IS_ON_LINUX)
    #include "include/daemon.h"
#endif

#include "include/logger.h"
#include "include/result.h"

int main(int argc, const char* argv[]) {
    #if defined(BENJI_IS_ON_WINDOWS)
        winsock_init();

        SERVICE_TABLE_ENTRY service_table[] = {
            {BENJI_SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) service_main},
            {NULL, NULL}
        };

        if (!StartServiceCtrlDispatcher(service_table)) {
            return GetLastError();
        }
    #elif defined(BENJI_IS_ON_LINUX)
        spawn_daemon();
    #endif

    return EXIT_SUCCESS;
}