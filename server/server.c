#include "server.h"
#include "../client/protocol.h"

#ifdef __unix__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

    // Echo loop: read and discard/echo client input until client disconnects
    char buf[256];
    while (1) {
        ssize_t r = recv(client, buf, sizeof(buf), 0);
        if (r > 0) {
            // echo back what we received (optional)
            send(client, buf, (size_t)r, 0);
            continue;
        }
        if (r == 0) {
            // client closed connection
            break;
        }
        // error
        if (errno == EINTR) continue;
        perror("recv");
        break;
    }

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
