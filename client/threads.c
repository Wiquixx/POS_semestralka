#include "threads.h"
#include "input.h"
#include "protocol.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <limits.h>
#include <stdlib.h>

void *input_thread_func(void *arg) {
    InputThreadArgs *args = (InputThreadArgs *)arg;
    int sockfd = args->sockfd;
    volatile int *running = args->running;
    volatile int *paused = args->paused;
    MenuState *menu_state = args->menu_state;
    GameState *game_state = args->game_state;
    input_init();
    while (*running) {
        if (*paused) {
            send(sockfd, MSG_PAUSE, strlen(MSG_PAUSE), 0);
            int action = menu_show_pause(game_state->time, game_state->score);
            if (action == 1) {
                send(sockfd, MSG_RESUME, strlen(MSG_RESUME), 0);
                *paused = 0;
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
                    char msg[3] = {MSG_DIR, (unsigned char)(ch >= 'a' ? ch - 32 : ch), '\0'}; // Uppercase
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
    MenuState *menu_state = args->menu_state;
    GameState *game_state = args->game_state;
    char buf[2048];
    char last_dir[64] = "D (RIGHT)";
    while (*running) {
        if (*paused) {
            menu_show_pause(game_state->time, game_state->score);
            sleep(1);
            continue;
        }
        ssize_t r = recv(sockfd, buf, sizeof(buf)-1, 0);
        if (r <= 0) break;
        buf[r] = '\0';
        if (*paused) continue;
        char *last_arrow = strstr(buf, MSG_LAST_ARROW);
        if (last_arrow) {
            *last_arrow = '\0';
            char *score_line = strstr(buf, "SCORE:");
            if (score_line) {
                char *score_start = score_line + 6;
                char *endptr;
                unsigned long val = strtoul(score_start, &endptr, 10);
                if (endptr != score_start && val <= UINT_MAX) {
                    game_state->score = (unsigned int)val;
                }
            }
            char *time_line = strstr(buf, "TIME:");
            if (time_line) {
                char *time_start = time_line + 5;
                char *endptr;
                unsigned long val = strtoul(time_start, &endptr, 10);
                if (endptr != time_start && val <= UINT_MAX) {
                    game_state->time = (unsigned int)val;
                }
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
                default: dir_str = "?"; break;
            }
            snprintf(last_dir, sizeof(last_dir), "%s", dir_str);
            printf("Current score: %u\n", game_state->score);
            // Print time in min:sec format
            printf("Time: %u:%02u\n", game_state->time / 60, game_state->time % 60);
            fflush(stdout);
        }
        if (strstr(buf, "quit") || strstr(buf, "MENU")) {
            printf("Game ended. \nFinal score: %u\n", game_state->score);
            int start_time = menu_state->time;
            if (start_time > 0) {
                int time_spent;
                if (game_state->time == 1) {
                    time_spent = (int)start_time;
                    game_state->time = 0;
                } else {
                    time_spent = (int)(start_time - game_state->time);
                }
                printf("Time spent: %d:%02d (Start: %d:%02d, Left: %d:%02d)\n",
                    time_spent / 60, time_spent % 60,
                    start_time / 60, start_time % 60,
                    game_state->time / 60, game_state->time % 60);
            } else {
                printf("Time spent: %u:%02u\n", game_state->time / 60, game_state->time % 60);
            }
            printf("Press any button to return to menu...\n");
            *running = 0;
            break;
        }
    }
    *running = 0;
    return NULL;
}
