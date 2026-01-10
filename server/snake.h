#ifndef SERVER_SNAKE_H
#define SERVER_SNAKE_H

#include <stddef.h>
#include "world.h"

// Directions for snake movement
typedef enum SnakeDirection {
    DIR_UP = 0,
    DIR_DOWN = 1,
    DIR_LEFT = 2,
    DIR_RIGHT = 3,
    DIR_NONE = -1
} SnakeDirection;

// Snake struct
typedef struct Snake {
    size_t size; // current size
    size_t max_size; // max possible size (for buffer)
    size_t *x; // array of x positions
    size_t *y; // array of y positions
    SnakeDirection dir; // current direction
} Snake;

void snake_init(Snake *snake, size_t world_w, size_t world_h);
void snake_destroy(Snake *snake);
void snake_move(Snake *snake, World *world, char input);
SnakeDirection input_to_dir(char input);
int is_reverse(SnakeDirection current, SnakeDirection next);

#endif // SERVER_SNAKE_H
