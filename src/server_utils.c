#ifndef BENJI_USE_SERVER_UTILS
    #define BENJI_USE_SERVER_UTILS
#endif

#include "include/utils.h"
#include "include/logger.h"

#ifdef _WIN32
    BENJIAPI void winsock_init() {
        struct WSAData wsa_data;

        log_message(BENJI_LOG_LEVEL_INFO, "Initializing Winsock...");

        if (WSAStartup(WINSOCK_VERSION, &wsa_data) != BENJI_NO_ERROR) {
            terminate(WSAGetLastError());
        }

        log_message(BENJI_LOG_LEVEL_INFO, "Winsock initialized successfully");
    }

    BENJIAPI void winsock_cleanup() {
        log_message(BENJI_LOG_LEVEL_INFO, "Cleaning up Winsock...");

        if (WSACleanup() == BENJI_SOCKET_ERROR) {
            exit(WSAGetLastError());
        }

        log_message(BENJI_LOG_LEVEL_INFO, "Winsock cleaned up successfully");
    }
#endif

BENJIAPI result_t* create_socket() {
    BENJI_SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == BENJI_SOCKET_ERROR) {
        #if defined(_WIN32)
            int error_code = WSAGetLastError();
        #elif defined(__linux__)
            int error_code = -1;
        #endif

        return result_error(error_code, BENJI_ERROR_PACKET, "Failed to create socket");
    }

    return result_success((void*) (uintptr_t) sock);
}

BENJIAPI result_t* close_socket(BENJI_SOCKET sock) {
    #if defined(_WIN32)
        int return_code = closesocket(sock);
    #elif defined(__linux__)
        int return_code = close(sock);
    #endif

    if (return_code == BENJI_SOCKET_ERROR) {
        #if defined(_WIN32)
            int error_code = WSAGetLastError();
        #elif defined(__linux__)
            int error_code = -1;
        #endif

        return result_error(error_code, BENJI_ERROR_PACKET, "Failed to close socket");
    }

    return result_success(NULL);
}

BENJIAPI void terminate(const int exit_code) {
    #ifdef _WIN32
        winsock_cleanup();
    #endif

    exit(exit_code);
}