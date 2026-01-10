#include "render.h"
#include <stdio.h>
#include <stdlib.h>

void render_init(void) {
    // nothing for now
}

void render_shutdown(void) {
    // nothing
}

void render_world(const char *grid, size_t w, size_t h) {
    // simple ascii rendering: print rows
    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            putchar(grid[y * w + x]);
        }
        putchar('\n');
    }
}

