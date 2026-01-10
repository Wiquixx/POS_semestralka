#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int persistence_save_map(const char *path, const char *grid, size_t w, size_t h) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    if (fwrite(&w, sizeof(size_t), 1, f) != 1) { fclose(f); return -1; }
    if (fwrite(&h, sizeof(size_t), 1, f) != 1) { fclose(f); return -1; }
    if (fwrite(grid, 1, w*h, f) != w*h) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

int persistence_load_map(const char *path, char **out_grid, size_t *out_w, size_t *out_h) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    size_t w,h;
    if (fread(&w, sizeof(size_t), 1, f) != 1) { fclose(f); return -1; }
    if (fread(&h, sizeof(size_t), 1, f) != 1) { fclose(f); return -1; }
    char *grid = malloc(w*h);
    if (!grid) { fclose(f); return -1; }
    if (fread(grid, 1, w*h, f) != w*h) { free(grid); fclose(f); return -1; }
    fclose(f);
    *out_grid = grid;
    *out_w = w; *out_h = h;
    return 0;
}

int persistence_save_highscore(const char *path, unsigned int score) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "%u\n", score);
    fclose(f);
    return 0;
}

int persistence_load_highscore(const char *path, unsigned int *out_score) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    unsigned int s = 0;
    if (fscanf(f, "%u", &s) != 1) { fclose(f); return -1; }
    fclose(f);
    *out_score = s;
    return 0;
}
