#ifndef CLIENT_PERSISTENCE_H
#define CLIENT_PERSISTENCE_H

#include <stddef.h>

// Save/load map files; simple API
int persistence_save_map(const char *path, const char *grid, size_t w, size_t h);
int persistence_load_map(const char *path, char **out_grid, size_t *out_w, size_t *out_h);

// Highscore save/load (simple text file with a number)
int persistence_save_highscore(const char *path, unsigned int score);
int persistence_load_highscore(const char *path, unsigned int *out_score);

#endif // CLIENT_PERSISTENCE_H

