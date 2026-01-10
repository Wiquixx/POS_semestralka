#ifndef SERVER_LOGIC_H
#define SERVER_LOGIC_H

#include "world.h"

// Apply player input to the world (single-byte directions)
void logic_apply_input(World *w, char input);

#endif // SERVER_LOGIC_H

