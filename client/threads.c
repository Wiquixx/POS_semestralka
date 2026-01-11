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
    char buf[2048];
    char last_dir[64] = "D (RIGHT)";
    unsigned int last_score = 0;
    while (*running) {
        ssize_t r = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        // Debug: print the whole message received from the server
        fprintf(stderr, "[DEBUG] Received message:\n%s\n", buf);
        // Find last direction marker
        char *last_arrow = strstr(buf, MSG_LAST_ARROW);
        if (last_arrow) {
            *last_arrow = '\0';
            // Parse score and time without modifying the buffer further
            unsigned int score = 0, time = 0;
            char *score_line = strstr(buf, "SCORE:");
            if (score_line) {
                sscanf(score_line, "SCORE:%u", &score);
                last_score = score;
            }
            char *time_line = strstr(buf, "TIME:");
            if (time_line) {
                sscanf(time_line, "TIME:%u", &time);
                // Debug: print raw time value
                fprintf(stderr, "[DEBUG] Parsed time: %u\n", time);
            }
            // Clear terminal
            printf("\033[2J\033[H");
            printf("%s", buf); // world
            // Parse direction
            char dir = last_arrow[strlen(MSG_LAST_ARROW)];
            const char *dir_str = "?";
            switch (dir) {
                case DIR_UP: dir_str = "W (UP)"; break;
                case DIR_DOWN: dir_str = "S (DOWN)"; break;
                case DIR_LEFT: dir_str = "A (LEFT)"; break;
                case DIR_RIGHT: dir_str = "D (RIGHT)"; break;
            }
            snprintf(last_dir, sizeof(last_dir), "%s", dir_str);
            //printf("Last direction: %s\n", last_dir);
            printf("Current score: %u\n", score);
            // Print time in min:sec format
            printf("Time: %u:%02u\n", time / 60, time % 60);
            fflush(stdout);
        }
        if (strstr(buf, "quit") || strstr(buf, "MENU")) {
            printf("Game ended. Final score: %u\n", last_score);
            printf("Press any button to return to menu...\n");
            *running = 0;
            break;
        }
    }
    *running = 0;
    return NULL;
}
