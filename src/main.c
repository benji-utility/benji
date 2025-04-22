#include "include/service.h"
#include "include/logger.h"
#include "include/result.h"

int main(int argc, const char* argv[]) {
    #ifdef _WIN32
        winsock_init();

        SERVICE_TABLE_ENTRY service_table[] = {
            {BENJI_SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) service_main},
            {NULL, NULL}
        };

        if (!StartServiceCtrlDispatcher(service_table)) {
            return GetLastError();
        }
    #endif

    return EXIT_SUCCESS;
}