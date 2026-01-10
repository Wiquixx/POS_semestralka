#ifndef SERVER_LOGIC_H
#define SERVER_LOGIC_H

#include "world.h"

// Apply player input to the world (single-byte directions)
void logic_apply_input(World *w, char input);
// Draw initial snake on the world grid
void logic_draw_initial(World *w);

#endif // SERVER_LOGIC_H

