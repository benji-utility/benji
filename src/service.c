#ifdef _WIN32
    #include "include/service.h"

    BENJIAPI void service_main(unsigned long argc, LPTSTR* argv) {
        service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
        service_status.dwServiceSpecificExitCode = 0;
        service_status.dwCheckPoint = 0;

        report_service_status(SERVICE_START_PENDING, 0, 0);

        service_status_handle = RegisterServiceCtrlHandler(
            TEXT(BENJI_SERVICE_NAME),
            (LPHANDLER_FUNCTION) service_control_handler
        );

        if (service_status_handle == NULL) {
            // TODO: more error context
            log_message(BENJI_LOG_LEVEL_ERROR, "Service status handle is NULL");

            return;
        }

        result_t* server_socket_result = server_init(server_info.hostname, server_info.port);

        if (server_socket_result->is_error) {
            result_error_payload_t server_socket_result_error = result_unwrap_error(server_socket_result);

            log_error_payload(BENJI_LOG_LEVEL_ERROR, server_socket_result_error);

            return;
        }

        server_socket = (BENJI_SOCKET) (uintptr_t) result_unwrap_value(server_socket_result);

        service_stop_event = WSACreateEvent();
        service_socket_event = WSACreateEvent();

        WSAEventSelect(server_socket, service_socket_event, FD_ACCEPT);

        report_service_status(SERVICE_RUNNING, 0, 0);

        HANDLE worker_thread = CreateThread(NULL, 0, service_worker_thread, NULL, 0, NULL);

        // basically run this indefinitely and rely on the thread to close properly
        WaitForSingleObject(worker_thread, INFINITE);

        CloseHandle(worker_thread);
    }

    BENJIAPI void service_control_handler(unsigned long request) {
        switch (request) {
            case SERVICE_CONTROL_STOP: // this and shutdown are handled the same, so perform the same actions
            case SERVICE_CONTROL_SHUTDOWN: {
                report_service_status(SERVICE_STOP_PENDING, 0, 0);

                WSASetEvent(service_stop_event);

                result_t* close_server_socket_result = close_socket(server_socket);

                if (close_server_socket_result->is_error) {
                    result_error_payload_t close_server_socket_result_error = result_unwrap_error(close_server_socket_result);

                    log_error_payload(BENJI_LOG_LEVEL_WARNING, close_server_socket_result_error);
                }

                result_free(close_server_socket_result);

                WSACloseEvent(service_socket_event);
                WSACloseEvent(service_stop_event);

                #ifdef _WIN32
                    winsock_cleanup();
                #endif

                report_service_status(SERVICE_STOPPED, 0, 0);

                log_message(BENJI_LOG_LEVEL_INFO, "Service shutdown gracefully");

                break;
            }

            default: {
                report_service_status(service_status.dwCurrentState, 0, 0);

                break;
            }
        }
    }

    BENJIAPI unsigned long service_worker_thread() {
        WSAEVENT events[BENJI_SERVICE_EVENTS] = {service_socket_event, service_stop_event};

        // this is safe ;)
        while (true) {
            unsigned long wait = WSAWaitForMultipleEvents(BENJI_SERVICE_EVENTS, events, false, WSA_INFINITE, false);

            if (wait == WSA_WAIT_EVENT_0) {
                WSANETWORKEVENTS network_events;

                WSAEnumNetworkEvents(server_socket, service_socket_event, &network_events);

                if (network_events.lNetworkEvents & FD_ACCEPT) {
                    result_t* server_update_result = server_update(server_socket);

                    if (server_update_result->is_error) {
                        result_error_payload_t server_update_result_error = result_unwrap_error(server_update_result);

                        log_error_payload(BENJI_LOG_LEVEL_WARNING, server_update_result_error);
                    }
                }
            }
            else {
                // service stop triggered
                break;
            }
        }

        return BENJI_NO_ERROR;
    }

    // TODO: make this log a change in status
    BENJIAPI void report_service_status(unsigned long current_state, unsigned long exit_code, unsigned long wait_hint) {
        service_status.dwCurrentState = current_state;
        service_status.dwWin32ExitCode = exit_code;
        service_status.dwWaitHint = wait_hint;

        SetServiceStatus(service_status_handle, &service_status);
    }

    void collect_server_details(config_details_t config_details) {
        server_info.hostname = config_details.server_config.hostname;
        server_info.port = config_details.server_config.port;
    }
#endif