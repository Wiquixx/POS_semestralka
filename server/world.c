#include "world.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

int world_create(World *out, size_t w, size_t h, int obstacles) {
    if (!out) return -1;
    out->w = w; out->h = h;
    out->food_x = 0;
    out->food_y = 0;
    out->obstacles = obstacles;
    if (w == 0 || h == 0) {
        out->grid = NULL;
    } else {
        out->grid = malloc(w * h);
        if (!out->grid) return -1;
        // initialize to dots
        memset(out->grid, '.', w * h);
        if (obstacles) {
            size_t num_obstacles = (size_t)(0.5 + sqrt((double)(w * h)));
            size_t placed = 0;
            srand((unsigned int)time(NULL));
            while (placed < num_obstacles) {
                size_t x = rand() % w;
                size_t y = rand() % h;
                // Check if position is valid for obstacle
                int valid = 1;
                // Not on wall
                if (x == 0 || y == 0 || x == w-1 || y == h-1) valid = 0;
                // Not within 1-tile radius of another obstacle
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        size_t nx = x + dx;
                        size_t ny = y + dy;
                        if (nx < w && ny < h && out->grid[ny * w + nx] == 'X') {
                            valid = 0;
                        }
                    }
                }
                if (valid && out->grid[y * w + x] == '.') {
                    printf("%d,%d\n", (int)x, (int)y);
                    out->grid[y * w + x] = 'X';
                    placed++;
                }
            }
        }
    }
    out->game_over = 0;
    out->score = 0;
    out->time = 0; // initialize time to 0
    return 0;
}

void world_destroy(World *w) {
    if (!w) return;
    free(w->grid); w->grid = NULL;
    w->w = w->h = 0;
    w->food_x = w->food_y = 0;
    w->game_over = 0;
    w->score = 0;
}

int world_tick(World *w) {
    if (!w) return -1;
    w->time += 1; // increment time by 1 each tick
    return 0;
}

unsigned int world_get_score(const World *w) {
    if (!w) return 0;
    return w->score;
}

void world_serialize(const World *w, char *buf) {
    if (!w || !w->grid || !buf) return;
    size_t idx = 0;
    for (size_t row = 0; row < w->h; ++row) {
        for (size_t col = 0; col < w->w; ++col) {
            buf[idx++] = w->grid[row * w->w + col];
        }
        buf[idx++] = '\n';
    }
    // Append score as a line
    idx += sprintf(buf + idx, "SCORE:%u\n", w->score);
    // Append time as a line
    idx += sprintf(buf + idx, "TIME:%u\n", w->time);
    // Debug: print time value being sent
    //fprintf(stderr, "[DEBUG] Server sending time: %u\n", w->time);
    buf[idx] = '\0';
}
