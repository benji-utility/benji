#ifndef __BENJI_SERVICE_H
#define __BENJI_SERVICE_H

#ifndef BENJI_USE_SERVER_UTILS
    #define BENJI_USE_SERVER_UTILS
#endif

#include <tchar.h>

#include "server.h"
#include "utils.h"

#ifndef BENJI_SERVICE_NAME
    #define BENJI_SERVICE_NAME TEXT("BenjiService")
#endif

static SERVICE_STATUS service_status;
static SERVICE_STATUS_HANDLE service_status_handle;
static HANDLE service_stop_event;

static BENJI_SOCKET server_socket;

BENJIAPI void service_main(unsigned long argc, LPTSTR* argv);
BENJIAPI void service_control_handler(unsigned long request);
BENJIAPI unsigned long service_worker_thread();
BENJIAPI void report_service_status(unsigned long current_state, unsigned long exit_code, unsigned long wait_hint);

#endif