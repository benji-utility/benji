#ifdef __linux__
    #ifndef __BENJI_DAEMON_H
    #define __BENJI_DAEMON_H

    #ifndef BENJI_USE_SERVER_UTILS
        #define BENJI_USE_SERVER_UTILS
    #endif

    #ifndef BENJI_USE_SYS_INFO_UTILS
        #define BENJI_USE_SYS_INFO_UTILS
    #endif

    #include <unistd.h>
    #include <signal.h>
    #include <sys/stat.h>

    #include "utils.h"

    void spawn_daemon();

    #endif
#endif