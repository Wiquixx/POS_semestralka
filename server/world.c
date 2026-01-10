#include "world.h"
#include <stdlib.h>
#include <string.h>

int world_create(World *out, size_t w, size_t h) {
    if (!out) return -1;
    out->w = w; out->h = h;
    out->food_x = 0;
    out->food_y = 0;
    if (w == 0 || h == 0) {
        out->grid = NULL;
    } else {
        out->grid = malloc(w * h);
        if (!out->grid) return -1;
        // initialize to dots
        memset(out->grid, '.', w * h);
    }
    out->game_over = 0;
    out->score = 0;
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
    // placeholder: do nothing and return success
    (void)w;
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
    buf[idx] = '\0';
}
