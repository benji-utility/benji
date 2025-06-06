#include "include/server.h"

BENJIAPI result_t* server_init(const char* hostname, uint16_t port) {
    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET; // ipv4 address family

    #if defined(_WIN32)
        server_address.sin_addr.S_un.S_addr = inet_addr(hostname);
    #elif defined(__linux__)
        server_address.sin_addr.s_addr = inet_addr(hostname);
    #endif

    server_address.sin_port = htons(port);

    server_status = BENJI_SERVER_STOPPED;

    log_message(BENJI_LOG_LEVEL_INFO, "Creating server socket...");

    result_t* server_socket_result = create_socket();

    return_if_error(server_socket_result);

    BENJI_SOCKET server_socket = (BENJI_SOCKET) (uintptr_t) result_unwrap_value(server_socket_result);

    log_message(BENJI_LOG_LEVEL_INFO, "Server socket created successfully");

    log_message(BENJI_LOG_LEVEL_INFO, "Binding server socket to server address...");

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == BENJI_SOCKET_ERROR) {
        result_t* close_server_socket_result = close_socket(server_socket);

        return_if_error_with_warning(close_server_socket_result);

        #if defined(_WIN32)
            int error_code = WSAGetLastError();
        #elif defined(__linux__)
            int error_code = -1;
        #endif

        return result_error(error_code, BENJI_ERROR_PACKET, "Failed to bind server socket to server address");
    }

    log_message(BENJI_LOG_LEVEL_INFO, "Server socket binded successfully");

    log_message(BENJI_LOG_LEVEL_INFO, "Putting server socket into listening mode...");

    if (listen(server_socket, BENJI_MAX_SOCK_CONNS) == BENJI_SOCKET_ERROR) {
        result_t* close_server_socket_result = close_socket(server_socket);

        return_if_error_with_warning(close_server_socket_result);

        #if defined(_WIN32)
            int error_code = WSAGetLastError();
        #elif defined(__linux__)
            int error_code = -1;
        #endif

        return result_error(error_code, BENJI_ERROR_PACKET, "Failed to put server socket into listening mode");
    }

    log_message(BENJI_LOG_LEVEL_INFO, "Server socket put into listening mode succesfully");

    socklen_t server_address_length = sizeof(server_address);

    getsockname(server_socket, (struct sockaddr*) &server_address, &server_address_length);

    log_message(BENJI_LOG_LEVEL_INFO, "Server created at '127.0.0.1:%d'", ntohs(server_address.sin_port));

    server_status = BENJI_SERVER_RUNNING;

    return result_success((void*) (uintptr_t) server_socket);
}

BENJIAPI result_t* server_update(BENJI_SOCKET server_socket) {
    char** data_groups;
    size_t data_group_count;

    result_t* client_handle_result = server_handle_client(server_socket, &data_groups, &data_group_count);
    return_if_error_with_warning(client_handle_result);

    BENJI_SOCKET client_socket = (BENJI_SOCKET) (uintptr_t) result_unwrap_value(client_handle_result);

    // make sure it is long enough to contain all the data
    char* json = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH * BENJI_BASIC_STRING_LENGTH, char));

    if (!json) {
        return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
    }

    json[0] = '\0';

    if (data_groups == NULL || data_group_count == 0) {
        log_message(BENJI_LOG_LEVEL_WARNING, "Client data is either empty or incorrectly formatted, closing client connection...");

        close_socket_with_result(client_socket);

        return result_error(-1, BENJI_ERROR_PACKET, "Client data is either empty or incorrectly formatted");
    }

    for (size_t i = 0; i < data_group_count; i++) {
        if (data_groups[i] == NULL) {
            log_message(BENJI_LOG_LEVEL_WARNING, "Invalid data group, closing client connection...");

            close_socket_with_result(client_socket);

            return result_error(-1, BENJI_ERROR_PACKET, "Invalid data group");
        }

        char* header;

        result_t* map_data_result = get_hardware_info(data_groups[i], &header);

        if (map_data_result->is_error) {
            close_socket_with_result(client_socket);
        }

        return_if_error_with_warning(map_data_result);

        map_t* map_data = (map_t*) result_unwrap_value(map_data_result);

        char* json_block = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

        if (!json_block) {
            return result_error(-1, BENJI_ERROR_PACKET, "malloc() failed");
        }

        json_block[0] = '\0';

        sprintf(json_block, "%s,", map_serialize(map_data, header));
        strcat(json, json_block);

        log_message(BENJI_LOG_LEVEL_INFO, "Collected data: '%s'", json_block);

        free(json_block);

        map_free(map_data);
    }

    free(data_groups);

    size_t json_length = strlen(json);

    json[json_length - 1] = '\0'; // remove the trailing comma

    char response[json_length + 2];
    sprintf(response, "{%s}", json);

    free(json);

    result_t* response_result = server_send_to_client(client_socket, response);

    if (response_result->is_error) {
        close_socket_with_result(client_socket);
    }

    return_if_error_with_warning(response_result);

    close_socket_with_result(client_socket);

    return result_success(NULL); // nothing to return, but this signifies a successful server cycle
}

BENJIAPI result_t* server_handle_client(BENJI_SOCKET server_socket, char*** data_groups, size_t* data_group_count) {
    result_t* client_socket_result = server_accept_client(server_socket);
    return_if_error(client_socket_result);

    BENJI_SOCKET client_socket = (BENJI_SOCKET) (uintptr_t) result_unwrap_value(client_socket_result);

    result_t* client_data_result = server_receive_from_client(client_socket);
    return_if_error(client_data_result);

    char* client_data = (char*) result_unwrap_value(client_data_result);

    log_message(BENJI_LOG_LEVEL_INFO, "Received client data: '%s'", client_data);

    *data_group_count = server_parse_client_data(client_data, data_groups);

    free(client_data);

    return result_success((void*) (uintptr_t) client_socket);
}

BENJIAPI result_t* server_accept_client(BENJI_SOCKET server_socket) {
    struct sockaddr_storage client;

    socklen_t client_length = sizeof(client);

    BENJI_SOCKET client_socket = accept(server_socket, (struct sockaddr*) &client, &client_length);

    if (client_socket == BENJI_INVALID_SOCKET) {
        #if defined(_WIN32)
            int error_code = WSAGetLastError();
        #elif defined(__linux__)
            int error_code = -1;
        #endif

        return result_error(error_code, BENJI_ERROR_PACKET, "accept() failed");
    }

    #if defined(_WIN32)
        u_long non_blocking_mode = true;

        ioctlsocket(client_socket, FIONBIO, &non_blocking_mode);
    #elif defined(__linux__)
        int flags = fcntl(client_socket, F_GETFL, 0);
        fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
    #endif

    return result_success((void*) (uintptr_t) client_socket);
}

BENJIAPI result_t* server_receive_from_client(BENJI_SOCKET client_socket) {
    char* data = NULL;
    size_t data_size = 0;

    char buffer[BENJI_BASIC_STRING_LENGTH];

    size_t bytes_received = 0;

    unsigned int tries = 0;

    do {
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == BENJI_SOCKET_ERROR) {
            #if defined(_WIN32)
                bool is_blocking = WSAGetLastError() == WSAEWOULDBLOCK;
            #elif defined(__linux__)
                bool is_blocking = errno == EAGAIN || errno == EWOULDBLOCK;
            #endif

            if (is_blocking) {
                if (tries < BENJI_MAX_TRIES) {
                    tries++;

                    BENJI_SLEEP(BENJI_RECV_RETRY_WAIT_TIME); // wait 50ms and try again

                    continue;
                }
                else {
                    break;
                }
            }
            else {
                #if defined(_WIN32)
                    int error_code = WSAGetLastError();
                #elif defined(__linux__)
                    int error_code = -1;
                #endif

                return result_error(error_code, BENJI_ERROR_PACKET, "recv() failed");
            }
        }

        buffer[bytes_received] = '\0';

        data = realloc(data, data_size + bytes_received + 1);
        if (data == NULL) {
            free(data);

            return result_error(-1, BENJI_ERROR_PACKET, "realloc() failed");
        }

        memcpy(data + data_size, buffer, bytes_received + 1);

        data_size += bytes_received;
    } while (bytes_received > 0);

    return result_success(data);
}

BENJIAPI result_t* server_send_to_client(BENJI_SOCKET client_socket, const char* data) {
    int bytes_sent = send(client_socket, data, strlen(data) + 1, 0);

    if (bytes_sent == BENJI_SOCKET_ERROR) {
        #if defined(_WIN32)
            int error_code = WSAGetLastError();
        #elif defined(__linux__)
            int error_code = -1;
        #endif

        return result_error(error_code, BENJI_ERROR_PACKET, "send() failed");
    }

    return result_success((void*) (uintptr_t) bytes_sent);
}

size_t server_parse_client_data(const char* client_data, char*** data_groups) {
    return strsplit(client_data, data_groups, ';');
}