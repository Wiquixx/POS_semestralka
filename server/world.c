#include "world.h"
#include <stdlib.h>
#include <string.h>

int world_create(World *out, size_t w, size_t h) {
    if (!out) return -1;
    out->w = w; out->h = h;
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
