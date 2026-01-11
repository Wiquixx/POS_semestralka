#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "menu.h"
#include <termios.h>
#include <unistd.h>

static void clear_terminal(void) {
    system("clear");
}

// Helper for time input, returns -1 on invalid input
static int ask_time(MenuState *state) {
    printf("Enter time in seconds (10 - 600): ");
    fflush(stdout);
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return -1;
    char *endptr;
    long t = strtol(buf, &endptr, 10);
    if (endptr == buf || (*endptr != '\0' && *endptr != '\n') || t < 10 || t > 600) return -1;
    state->time = (int)t;
    return 0;
}

// Helper for X/Y input, returns -1 on invalid input
static int ask_xy(MenuState *state) {
    printf("Enter X size (10-20): ");
    fflush(stdout);
    char bufx[16];
    if (!fgets(bufx, sizeof(bufx), stdin)) return -1;
    char *endptrx;
    long x = strtol(bufx, &endptrx, 10);
    if (endptrx == bufx || (*endptrx != '\0' && *endptrx != '\n') || x < 10 || x > 20) return -1;
    printf("Enter Y size (10-20): ");
    fflush(stdout);
    char bufy[16];
    if (!fgets(bufy, sizeof(bufy), stdin)) return -1;
    char *endptry;
    long y = strtol(bufy, &endptry, 10);
    if (endptry == bufy || (*endptry != '\0' && *endptry != '\n') || y < 10 || y > 20) return -1;
    state->x = (int)x;
    state->y = (int)y;
    return 0;
}

int menu_show_main(MenuState *state) {
    while (1) {
        clear_terminal();
        printf("MENU\n");
        printf("Start Game - standard (1)\n");
        printf("Start Game - time (2)\n");
        printf("Quit (3)\n");
        printf("> ");
        fflush(stdout);
        char choice[16];
        if (!fgets(choice, sizeof(choice), stdin)) return 3;
        choice[strcspn(choice, "\n")] = '\0';
        if (strcmp(choice, "1") == 0) return 1;
        if (strcmp(choice, "2") == 0) {
            if (ask_time(state) == -1) continue;
            return 2;
        }
        if (strcmp(choice, "3") == 0) return 3;
    }
}

int menu_show_obstacles(MenuState *state) {
    while (1) {
        clear_terminal();
        printf("With Obstacles (1)\n");
        printf("Without obstacles (2)\n");
        printf("> ");
        fflush(stdout);
        char choice[16];
        if (!fgets(choice, sizeof(choice), stdin)) return 1;
        choice[strcspn(choice, "\n")] = '\0';
        if (strcmp(choice, "1") == 0) {
            state->obstacles = 1;
            if (ask_xy(state) == -1) return -1;
            return 1;
        }
        if (strcmp(choice, "2") == 0) {
            state->obstacles = 0;
            if (ask_xy(state) == -1) return -1;
            return 2;
        }
    }
}

// Shows the pause menu, returns 1 for resume, 2 for back to menu
int menu_show_pause(unsigned int current_time, unsigned int current_score) {
    struct termios orig, temp;
    tcgetattr(STDIN_FILENO, &orig);
    temp = orig;
    temp.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp);
    int result = 2;
    while (1) {
        clear_terminal();
        printf("PAUSE MENU\n");
        printf("Current time: %u:%02u\n", current_time / 60, current_time % 60);
        printf("Current score: %u\n", current_score);
        printf("Resume (1)\n");
        printf("Back to Menu (2)\n");
        printf("> ");
        fflush(stdout);
        char choice[16];
        if (!fgets(choice, sizeof(choice), stdin)) break;
        choice[strcspn(choice, "\n")] = '\0';
        if (strcmp(choice, "1") == 0) { result = 1; break; }
        if (strcmp(choice, "2") == 0) { result = 2; break; }
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
    return result;
}
