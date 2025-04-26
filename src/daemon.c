#ifdef __linux__
    #include "include/daemon.h"

    void spawn_daemon() {
        pid_t pid = fork();

        if (pid != 0) {
            exit(pid > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
        }

        if (setsid() < 0) {
            exit(EXIT_FAILURE);
        }

        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);

        pid = fork();

        if (pid != 0) {
            exit(pid > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
        }

        umask(0);

        chdir("/");

        // close open file descriptors
        for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
            close(fd);
        }
    }
#endif