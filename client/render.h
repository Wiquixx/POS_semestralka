#ifndef CLIENT_RENDER_H
#define CLIENT_RENDER_H

#include <stddef.h>

// Simple text rendering API for the game world
void render_init(void);
void render_shutdown(void);

// Render the world snapshot (char grid) of size w*h
void render_world(const char *grid, size_t w, size_t h);

#endif // CLIENT_RENDER_H

