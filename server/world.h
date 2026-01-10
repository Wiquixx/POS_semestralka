#ifndef SERVER_WORLD_H
#define SERVER_WORLD_H

#include <stddef.h>

// Minimal World placeholder kept for structure compatibility
typedef struct World {
    size_t w, h;
    char *grid; // optional grid buffer, size w*h

    int game_over; // non-zero when game ended (placeholder)
    unsigned int score; // placeholder score
} World;

// create a new world (placeholder)
int world_create(World *out, size_t w, size_t h);
void world_destroy(World *w);

// tick the world (placeholder)
int world_tick(World *w);

// get current score
unsigned int world_get_score(const World *w);

// Serialize world to a string buffer (buffer must be at least w*h+1 bytes)
void world_serialize(const World *w, char *buf);

#endif // SERVER_WORLD_H
