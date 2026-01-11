#include "client.h"

#include "protocol.h"
#include "input.h"
#include "threads.h"
#include "menu.h"

#ifdef __unix__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

static volatile int running = 0;
static volatile int paused = 0;
static int sockfd = -1;

static int connect_server_simple(const char *host, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        close(fd);
        return -1;
    }
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { close(fd); return -1; }
    return fd;
}

// Remove global variables, pass mode and obstacles as arguments
int client_run(int mode, int obstacles, MenuState *menu_state) {
    unsigned int highscore = 0;
    //printf("Commands: use WASD; p to pause.\n");
    //printf("Current highscore: %u\n", highscore);

    int attempts = 5;
    for (int i = 0; i < attempts; ++i) {
        //printf("Pripajam\n");
        sockfd = connect_server_simple("127.0.0.1", PROTO_PORT);
        if (sockfd >= 0) break;
        sleep(1);
    }
    if (sockfd < 0) {
        fprintf(stderr, "Could not connect to server on 127.0.0.1:%d\n", PROTO_PORT);
        return -1;
    }

    // Send world size if needed
    if (mode == 2) {
        unsigned char msg[5];
        msg[0] = MSG_WORLD_SIZE;
        msg[1] = (unsigned char)menu_state->x;
        msg[2] = (unsigned char)menu_state->y;
        msg[3] = (obstacles == 1) ? 'O' : 'N';
        msg[4] = (unsigned char)menu_state->time;
        send(sockfd, msg, 5, 0);
    } else if (mode == 1) {
        if (obstacles == 1) {
            unsigned char msg[4];
            msg[0] = MSG_WORLD_SIZE;
            msg[1] = (unsigned char)menu_state->x;
            msg[2] = (unsigned char)menu_state->y;
            msg[3] = 'O';
            send(sockfd, msg, 4, 0);
        } else {
            unsigned char msg[3];
            msg[0] = MSG_WORLD_SIZE;
            msg[1] = (unsigned char)menu_state->x;
            msg[2] = (unsigned char)menu_state->y;
            send(sockfd, msg, 3, 0);
        }
    }

    running = 1;
    paused = 0;
    GameState game_state = {0};
    pthread_mutex_t game_state_mutex, menu_state_mutex;
    pthread_mutex_init(&game_state_mutex, NULL);
    pthread_mutex_init(&menu_state_mutex, NULL);
    pthread_t input_thread, recv_thread;
    InputThreadArgs input_args = { sockfd, &running, &paused, menu_state, &game_state, &game_state_mutex, &menu_state_mutex };
    ReceiverThreadArgs recv_args = { sockfd, &running, &paused, menu_state, &game_state, &game_state_mutex, &menu_state_mutex };
    pthread_create(&input_thread, NULL, input_thread_func, &input_args);
    pthread_create(&recv_thread, NULL, receiver_thread_func, &recv_args);
    pthread_join(input_thread, NULL);
    pthread_join(recv_thread, NULL);
    pthread_mutex_destroy(&game_state_mutex);
    pthread_mutex_destroy(&menu_state_mutex);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    return 0;
}

int main(void) {
    MenuState menu_state = {0 };
    while (1) {
        int mode = menu_show_main(&menu_state);
        if (mode == 3) {
            return 0;
        }
        int obstacles = menu_show_obstacles(&menu_state);
        if (obstacles == -1) {
            continue; // restart menu on invalid X/Y input
        }
        // Start server as subprocess
        //printf("Starting server...\n");
        pid_t pid = fork();
        if (pid == 0) {
            // Child: exec server
            execl("../server/server", "server", (char*)NULL);
            perror("Failed to start server");
            exit(1);
        } else if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        // Parent: wait a bit for server to start
        sleep(2); // Increased wait time
        system("clear");
        int result = client_run(mode, (obstacles == 1) ? 1 : 0, &menu_state);
        system("clear");
        if (result != 0) {
            fprintf(stderr, "Client failed to connect or run.\n");
        }
    }
}

#endif

