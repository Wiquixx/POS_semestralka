#include "threads.h"
#include "input.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

void *input_thread_func(void *arg) {
    InputThreadArgs *args = (InputThreadArgs *)arg;
    int sockfd = args->sockfd;
    volatile int *running = args->running;
    input_init();
    while (*running) {
        int dir = input_read_wasd();
        if (dir) {
            char msg[3] = {MSG_DIR, (char)dir, '\0'};
            send(sockfd, msg, 2, 0);
        } else {
            char ch;
            if (read(STDIN_FILENO, &ch, 1) == 1 && ch == 'q') {
                send(sockfd, "quit", 4, 0);
                *running = 0;
                break;
            }
        }
    }
    input_restore();
    return NULL;
}

void *receiver_thread_func(void *arg) {
    ReceiverThreadArgs *args = (ReceiverThreadArgs *)arg;
    int sockfd = args->sockfd;
    volatile int *running = args->running;
    char line[512];
    size_t pos = 0;
    // Initial welcome message
    while (*running) {
        char ch;
        ssize_t r = recv(sockfd, &ch, 1, 0);
        if (r <= 0) break;
        line[pos++] = ch;
        if (ch == '\n' || pos + 1 >= sizeof(line)) break;
    }
    if (pos > 0) {
        line[pos] = '\0';
        printf("Server: %s", line);
    }
    // Main receive/render loop
    while (*running) {
        char buf[256];
        ssize_t r = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        if (strncmp(buf, MSG_LAST_ARROW, strlen(MSG_LAST_ARROW)) == 0 && r >= (int)strlen(MSG_LAST_ARROW)+1) {
            char dir = buf[strlen(MSG_LAST_ARROW)];
            const char *dir_str = "?";
            switch (dir) {
                case DIR_UP: dir_str = "W (UP)"; break;
                case DIR_DOWN: dir_str = "S (DOWN)"; break;
                case DIR_LEFT: dir_str = "A (LEFT)"; break;
                case DIR_RIGHT: dir_str = "D (RIGHT)"; break;
                default: dir_str = "?"; break;
            }
            printf("Last direction: %s\n", dir_str);
            fflush(stdout);
        }
    }
    *running = 0;
    return NULL;
}
