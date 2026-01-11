#ifndef MENU_H
#define MENU_H

typedef struct {
    int time;
    int x;
    int y;
    int obstacles;
} MenuState;

// Shows the main menu, returns selected mode (1=standard, 2=time, 3=quit)
int menu_show_main(MenuState *state);
// Shows the obstacles menu, returns selected obstacles (1=with, 2=without)
int menu_show_obstacles(MenuState *state);
// Shows the pause menu, returns 1 for resume, 2 for back to menu
int menu_show_pause(unsigned int current_time, unsigned int current_score);

#endif // MENU_H
