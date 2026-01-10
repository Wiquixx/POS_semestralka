#include "server.h"
#include "../client/protocol.h"
#include "world.h"

#ifdef __unix__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

int server_init(void) { return 0; }

// Simple blocking server: accept one client and send a welcome message.
int server_run(void) {
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return -1; }

    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PROTO_PORT);

    if (bind(srv, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(srv); return -1; }
    if (listen(srv, 1) < 0) { perror("listen"); close(srv); return -1; }

    printf("Server listening on port %d\n", PROTO_PORT);
    int client = accept(srv, NULL, NULL);
    if (client < 0) { perror("accept"); close(srv); return -1; }
    printf("Client connected\n");

    // Send a simple connection-success message to the client
    const char *msg = "CONNECTED\n";
    ssize_t out = send(client, msg, strlen(msg), 0);
    (void)out;

    char last_dir = DIR_RIGHT;
    struct timeval last_tick, now;
    gettimeofday(&last_tick, NULL);
    char buf[256];
    int running = 1;
    World game_world;
    int world_initialized = 0;
    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(client, &readfds);
        struct timeval tv = {0, 100000}; // 100ms
        int r = select(client+1, &readfds, NULL, NULL, &tv);
        if (r > 0 && FD_ISSET(client, &readfds)) {
            ssize_t n = recv(client, buf, sizeof(buf)-1, 0);
            if (n > 0) {
                buf[n] = '\0';
                if (buf[0] == MSG_WORLD_SIZE && n >= 3) {
                    size_t x = (unsigned char)buf[1];
                    size_t y = (unsigned char)buf[2];
                    if (world_create(&game_world, x, y) == 0) {
                        world_initialized = 1;
                        printf("World created: %zux%zu\n", x, y);
                        // Print the world grid
                        for (size_t row = 0; row < y; ++row) {
                            for (size_t col = 0; col < x; ++col) {
                                putchar(game_world.grid[row * x + col]);
                            }
                            putchar('\n');
                        }
                    } else {
                        printf("World creation failed\n");
                    }
                } else if (buf[0] == MSG_DIR && n >= 2) {
                    last_dir = buf[1];
                } else if (strncmp(buf, MSG_QUIT, 4) == 0) {
                    running = 0;
                    break;
                }
            } else if (n == 0) {
                running = 0;
                break;
            }
        }
        gettimeofday(&now, NULL);
        if ((now.tv_sec > last_tick.tv_sec) || (now.tv_sec == last_tick.tv_sec && now.tv_usec - last_tick.tv_usec >= 1000000)) {
            if (world_initialized) {
                size_t bufsize = game_world.w * game_world.h + game_world.h + 1;
                char *worldbuf = malloc(bufsize + 64); // extra for direction
                if (worldbuf) {
                    world_serialize(&game_world, worldbuf);
                    size_t worldlen = strlen(worldbuf);
                    snprintf(worldbuf + worldlen, 64, "%s%c\n", MSG_LAST_ARROW, last_dir);
                    send(client, worldbuf, strlen(worldbuf), 0);
                    free(worldbuf);
                }
            }
            last_tick = now;
        }
    }
    if (world_initialized) world_destroy(&game_world);

    close(client);
    close(srv);
    printf("Server shut down\n");
    return 0;
}

void server_shutdown(void) { }

#else

// Non-unix stubs
int server_init(void) { return 0; }
int server_run(void) { return 0; }
void server_shutdown(void) { }

#endif

// Single main() for all platforms
int main(void) {
    if (server_init() != 0) return 1;
    server_run();
    server_shutdown();
    return 0;
}
