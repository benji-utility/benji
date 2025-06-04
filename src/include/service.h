#ifndef __BENJI_SERVICE_H
#define __BENJI_SERVICE_H

#ifndef BENJI_USE_SERVER_UTILS
    #define BENJI_USE_SERVER_UTILS
#endif

#ifdef _WIN32
    #include <tchar.h>
#endif

#include "config_loader.h"
#include "server.h"
#include "utils.h"

#ifdef _WIN32
    #ifndef BENJI_SERVICE_WSAEVENTS_COUNT
        #define BENJI_SERVICE_WSAEVENTS_COUNT (2) /* i dont want this to be hardcoded, but idk if sizeof will help with WSAEVENTs */
    #endif
#endif

static struct _BENJI_SERVER_INFO {
    const char* hostname;
    uint16_t port;
} server_info;

static BENJI_SOCKET server_socket;

#if defined(_WIN32)
    static SERVICE_STATUS service_status;
    static SERVICE_STATUS_HANDLE service_status_handle;

    static WSAEVENT service_stop_event;
    static WSAEVENT service_socket_event;

    BENJIAPI void service_main(unsigned long argc, LPTSTR* argv);
    BENJIAPI void service_control_handler(unsigned long request);
    BENJIAPI unsigned long service_worker_thread();
    BENJIAPI void report_service_status(unsigned long current_state, unsigned long exit_code, unsigned long wait_hint);
#elif defined(__linux__)
    /* TODO: add linux stuff */
#endif

// prolly the closest anything in this project will get to a constructor
void collect_server_details(config_details_t config_details);

#endif