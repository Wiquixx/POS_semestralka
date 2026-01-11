#include "threads.h"
#include "input.h"
#include "protocol.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

void *input_thread_func(void *arg) {
    InputThreadArgs *args = (InputThreadArgs *)arg;
    int sockfd = args->sockfd;
    volatile int *running = args->running;
    volatile int *paused = args->paused;
    input_init();
    while (*running) {
        if (*paused) {
            // Notify server of pause
            send(sockfd, MSG_PAUSE, strlen(MSG_PAUSE), 0);
            int action = menu_show_pause();
            if (action == 1) {
                // Notify server of resume
                send(sockfd, MSG_RESUME, strlen(MSG_RESUME), 0);
                *paused = 0; // Resume
            } else if (action == 2) {
                send(sockfd, "quit", 4, 0);
                *running = 0;
                break;
            }
            continue;
        }
        char ch;
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            if (ch == 'p' || ch == 'P') {
                *paused = 1;
                continue;
            }
            // WASD movement
            switch (ch) {
                case 'w': case 'W':
                case 'a': case 'A':
                case 's': case 'S':
                case 'd': case 'D': {
                    char msg[3] = {MSG_DIR, ch >= 'a' ? ch - 32 : ch, '\0'}; // Uppercase
                    send(sockfd, msg, 2, 0);
                    break;
                }
                default:
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
    volatile int *paused = args->paused;
    char buf[2048];
    char last_dir[64] = "D (RIGHT)";
    unsigned int last_score = 0;
    while (*running) {
        if (*paused) {
            sleep(1); // Sleep 100ms while paused
            continue;
        }
        ssize_t r = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        if (*paused) continue; // Double-check pause before printing
        // Debug: print the whole message received from the server
        //fprintf(stderr, "[DEBUG] Received message:\n%s\n", buf);
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
                //fprintf(stderr, "[DEBUG] Parsed time: %u\n", time);
            }
            // Strip SCORE and TIME lines from world display
            char *score_line_strip = strstr(buf, "SCORE:");
            char *time_line_strip = strstr(buf, "TIME:");
            // Remove SCORE and TIME lines by replacing them with empty lines
            if (score_line_strip) {
                char *end = strchr(score_line_strip, '\n');
                if (end) memmove(score_line_strip, end + 1, strlen(end + 1) + 1);
            }
            time_line_strip = strstr(buf, "TIME:"); // re-search in case buffer changed
            if (time_line_strip) {
                char *end = strchr(time_line_strip, '\n');
                if (end) memmove(time_line_strip, end + 1, strlen(end + 1) + 1);
            }
            // Clear terminal
            printf("\033[2J\033[H");
            printf("%s", buf); // world (without SCORE/TIME)
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
