#ifndef SERVER_WORLD_H
#define SERVER_WORLD_H

#include <stddef.h>

// Minimal World placeholder kept for structure compatibility
typedef struct World {
    size_t w, h;
    char *grid; // optional grid buffer, size w*h

    int game_over; // non-zero when game ended (placeholder)
    unsigned int score; // placeholder score
    size_t food_x, food_y; // position of food
    int obstacles; // 1 if obstacles are enabled, 0 otherwise
} World;

// create a new world; if obstacles is nonzero, generate obstacles
int world_create(World *out, size_t w, size_t h, int obstacles);
void world_destroy(World *w);

// tick the world (placeholder)
int world_tick(World *w);

// get current score
unsigned int world_get_score(const World *w);

// Serialize world to a string buffer (buffer must be at least w*h+1 bytes)
void world_serialize(const World *w, char *buf);

// Obstacles are represented as 'X' in grid

#endif // SERVER_WORLD_H
