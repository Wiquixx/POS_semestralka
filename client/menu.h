#ifndef MENU_H
#define MENU_H

// Shows the main menu, returns selected mode (1=standard, 2=time, 3=quit)
int menu_show_main(void);
// Shows the obstacles menu, returns selected obstacles (1=with, 2=without)
int menu_show_obstacles(void);
// Shows the pause menu, returns 1 for resume, 2 for back to menu
int menu_show_pause(void);
// Getters for menu values
int menu_get_time(void);
int menu_get_x(void);
int menu_get_y(void);

#endif // MENU_H
