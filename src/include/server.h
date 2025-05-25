#ifndef __BENJI_SERVER_H
#define __BENJI_SERVER_H

#ifndef BENJI_USE_SERVER_UTILS
    #define BENJI_USE_SERVER_UTILS
#endif

#ifndef BENJI_USE_SYS_INFO_UTILS
    #define BENJI_USE_SYS_INFO_UTILS
#endif

#include "utils.h"
#include "hardware.h"
#include "logger.h"

#ifndef BENJI_RECV_RETRY_WAIT_TIME
    #define BENJI_RECV_RETRY_WAIT_TIME (50) /* in ms */
#endif

#ifndef BENJI_SLEEP
    #if defined(_WIN32)
        #define BENJI_SLEEP(ms) Sleep(ms)
    #elif defined(__linux__)
        #define BENJI_SLEEP(ms) sleep(ms)
    #endif
#endif

#ifndef close_socket_with_result
    #define close_socket_with_result() \
        result_t* close_client_socket_result = close_socket(client_socket); \
        \
        if (close_client_socket_result->is_error) { \
            result_error_payload_t close_client_socket_result_error = result_unwrap_error(close_client_socket_result); \
            \
            log_warning(close_client_socket_result_error); \
            \
            return result_error( \
                close_client_socket_result_error.code, \
                BENJI_ERROR_PACKET, \
                close_client_socket_result_error.message \
            ); \
        } \
        \
        result_free(close_client_socket_result);
#endif

static enum _BENJI_SERVER_STATUS {
    BENJI_SERVER_STOPPED,
    BENJI_SERVER_RUNNING
} server_status;

BENJIAPI result_t* server_init(const char* hostname, uint16_t port);
BENJIAPI result_t* server_update(BENJI_SOCKET server_socket);

BENJIAPI result_t* server_handle_client(BENJI_SOCKET server_socket, char*** data_groups, size_t* data_group_count);
BENJIAPI result_t* server_accept_client(BENJI_SOCKET server_socket);
BENJIAPI result_t* server_receive_from_client(BENJI_SOCKET client_socket);
BENJIAPI result_t* server_send_to_client(BENJI_SOCKET client_socket, const char* data);

size_t server_parse_client_data(const char* client_data, char*** data_groups);

#endif