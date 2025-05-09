#ifdef _WIN32
    #ifndef __BENJI_SERVICE_H
    #define __BENJI_SERVICE_H

    #ifndef BENJI_USE_SERVER_UTILS
        #define BENJI_USE_SERVER_UTILS
    #endif

    #include <tchar.h>

    #include "config_parser.h"
    #include "server.h"
    #include "utils.h"

    #ifndef BENJI_SERVICE_NAME
        #define BENJI_SERVICE_NAME TEXT("BenjiService")
    #endif

    #ifndef BENJI_SERVICE_EVENTS
        #define BENJI_SERVICE_EVENTS (2) /* i dont want this to be hardcoded, but idk if sizeof will help with WSAEVENTs */
    #endif

    static struct _BENJI_SERVER_INFO {
        const char* hostname;
        uint16_t port;
    } server_info;

    static SERVICE_STATUS service_status;
    static SERVICE_STATUS_HANDLE service_status_handle;

    static WSAEVENT service_stop_event;
    static WSAEVENT service_socket_event;

    static BENJI_SOCKET server_socket;

    BENJIAPI void service_main(unsigned long argc, LPTSTR* argv);
    BENJIAPI void service_control_handler(unsigned long request);
    BENJIAPI unsigned long service_worker_thread();
    BENJIAPI void report_service_status(unsigned long current_state, unsigned long exit_code, unsigned long wait_hint);

    // prolly the closest anything in this project will get to a constructor
    void collect_server_details(config_details_t config_details);

    #endif
#endif