#include <stdio.h>
#include <string.h>
#include "menu.h"

int menu_show_main(void) {
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
    if (strcmp(choice, "2") == 0) return 2;
    return 3;
}

int menu_show_obstacles(void) {
    printf("With Obstacles (1) Without Obstacles (2)\n");
    printf("> ");
    fflush(stdout);
    char choice[16];
    if (!fgets(choice, sizeof(choice), stdin)) return 1;
    choice[strcspn(choice, "\n")] = '\0';
    if (strcmp(choice, "2") == 0) return 2;
    return 1;
}
