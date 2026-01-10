#include "client.h"
#include "persistence.h"
#include "render.h"
#include "protocol.h"

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

// Thread function: read from the server and print messages
static void *receiver_thread(void *arg) {
    (void)arg;
    // First, read an initial welcome line from the server (text terminated by '\n')
    char line[512];
    size_t pos = 0;
    while (running) {
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

    // Then keep reading and echoing any further data (simple behavior)
    while (running) {
        char buf[256];
        ssize_t r = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        printf("Server: %s", buf);
        fflush(stdout);
    }

    running = 0;
    return NULL;
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
    printf("Commands: type any text and press Enter; q to quit.\n");
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

    running = 1;
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receiver_thread, NULL) != 0) {
        fprintf(stderr, "Failed to create receiver thread\n");
        close(sockfd);
        return -1;
    }

    // main loop: read lines from stdin and send to server
    char line[128];
    while (running && fgets(line, sizeof(line), stdin)) {
        // trim newline
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;
        if (line[0] == 'q' && line[1] == '\0') {
            // send quit and exit
            send(sockfd, "quit", 4, 0);
            break;
        }
        // send the line to the server
        send(sockfd, line, strlen(line), 0);
    }

    // signal receiver to stop and wait for it
    running = 0;
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    pthread_join(recv_thread, NULL);

    render_shutdown();
    return 0;
}

void client_destroy(Client *c) {
    (void)c; render_shutdown(); }

int main(void) {
    printf("Menu\n");
    printf("1 - Start server and connect\n");
    printf("0 - Quit\n");
    printf("> ");
    fflush(stdout);
    char choice[16];
    if (!fgets(choice, sizeof(choice), stdin)) return 0;
    choice[strcspn(choice, "\n")] = '\0';
    if (strcmp(choice, "0") == 0) {
        return 0;
    }
    if (strcmp(choice, "1") == 0) {
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
        if (client_init(&c) != 0) return 1;
        client_run(&c);
        client_destroy(&c);
        // Optionally, kill server process after client exits
        kill(pid, SIGTERM);
        return 0;
    }
    printf("Unknown option.\n");
    return 0;
}

#endif
