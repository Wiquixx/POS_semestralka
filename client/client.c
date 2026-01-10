#include "client.h"
#include "persistence.h"
#include "render.h"
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
#include <signal.h>
#include <sys/select.h>
#include <time.h>

struct Client { int sock; };

static volatile int running = 0;
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

int client_init(Client *c) {
    (void)c; // no special init required here
    render_init();
    return 0;
}

int client_run(Client *c) {
    (void)c;
    unsigned int highscore = 0;
    persistence_load_highscore("highscore.txt", &highscore);

    printf("Simple client\n");
    printf("Commands: use WASD; q to quit.\n");
    printf("Current highscore: %u\n", highscore);

    // try to connect to server
    int attempts = 5;
    for (int i = 0; i < attempts; ++i) {
        sockfd = connect_server_simple("127.0.0.1", PROTO_PORT);
        if (sockfd >= 0) break;
        sleep(1);
    }
    if (sockfd < 0) {
        fprintf(stderr, "Could not connect to server on 127.0.0.1:%d\n", PROTO_PORT);
        return -1;
    }

    // Send world size if needed (mode and obstacles must be passed in or made global)
    extern int g_mode, g_obstacles;
    if (g_mode == 2 || (g_mode == 1 && g_obstacles == 2)) {
        unsigned char msg[3];
        msg[0] = MSG_WORLD_SIZE;
        msg[1] = (unsigned char)menu_get_x();
        msg[2] = (unsigned char)menu_get_y();
        send(sockfd, msg, 3, 0);
    }

    running = 1;
    pthread_t input_thread, recv_thread;
    InputThreadArgs input_args = { sockfd, &running };
    ReceiverThreadArgs recv_args = { sockfd, &running };
    pthread_create(&input_thread, NULL, input_thread_func, &input_args);
    pthread_create(&recv_thread, NULL, receiver_thread_func, &recv_args);
    pthread_join(input_thread, NULL);
    pthread_join(recv_thread, NULL);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    render_shutdown();
    return 0;
}

void client_destroy(Client *c) {
    (void)c; render_shutdown(); }

int main(void) {
    int mode, obstacles;
    while (1) {
        mode = menu_show_main();
        if (mode == 3) {
            return 0;
        }
        obstacles = menu_show_obstacles();
        if (obstacles == -1) {
            continue; // restart menu on invalid X/Y input
        }
        // Start server as subprocess
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
        sleep(1);
        // Now run client logic
        Client c;
        // Set globals for mode/obstacles
        extern int g_mode, g_obstacles;
        g_mode = mode;
        g_obstacles = obstacles;
        if (client_init(&c) != 0) return 1;
        client_run(&c);
        client_destroy(&c);
        return 0;
    }
}

#endif

// Add global variables for mode and obstacles
int g_mode = 0;
int g_obstacles = 0;
