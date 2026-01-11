#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "menu.h"

static void clear_terminal(void);

static int menu_time = 0;
static int menu_x = 0;
static int menu_y = 0;
static int menu_obstacles = 0;

static void clear_terminal(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
#endif
}

// Helper for time input, returns -1 on invalid input
static int ask_time(void) {
    printf("How many seconds should the game last? ");
    fflush(stdout);
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return -1;
    char *endptr;
    long t = strtol(buf, &endptr, 10);
    if (endptr == buf || (*endptr != '\0' && *endptr != '\n') || t <= 0) return -1;
    menu_time = (int)t;
    return 0;
}

// Helper for X/Y input, returns -1 on invalid input
static int ask_xy(void) {
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
    menu_x = (int)x;
    menu_y = (int)y;
    return 0;
}

int menu_show_main(void) {
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
            if (ask_time() == -1) continue;
            return 2;
        }
        if (strcmp(choice, "3") == 0) return 3;
    }
}

int menu_show_obstacles(void) {
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
            menu_obstacles = 1;
            if (ask_xy() == -1) return -1;
            return 1;
        }
        if (strcmp(choice, "2") == 0) {
            menu_obstacles = 0;
            if (ask_xy() == -1) return -1;
            return 2;
        }
    }
}

int menu_get_x(void) { return menu_x; }
int menu_get_y(void) { return menu_y; }
int menu_get_obstacles(void) { return menu_obstacles; }


