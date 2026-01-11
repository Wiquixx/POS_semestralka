#include "snake.h"
#include <stdlib.h>
#include <string.h>

SnakeDirection input_to_dir(char input) {
    switch (input) {
        case 'W': return DIR_UP;
        case 'S': return DIR_DOWN;
        case 'A': return DIR_LEFT;
        case 'D': return DIR_RIGHT;
        default: return DIR_NONE;
    }
}

int is_reverse(SnakeDirection current, SnakeDirection next) {
    return (current == DIR_UP && next == DIR_DOWN) ||
           (current == DIR_DOWN && next == DIR_UP) ||
           (current == DIR_LEFT && next == DIR_RIGHT) ||
           (current == DIR_RIGHT && next == DIR_LEFT);
}

void snake_init(Snake *snake, size_t world_w, size_t world_h) {
    snake->max_size = world_w * world_h;
    snake->size = 2;
    snake->x = malloc(snake->max_size * sizeof(size_t));
    snake->y = malloc(snake->max_size * sizeof(size_t));
    // Place body at (0,0) and head at (1,0)
    snake->x[0] = 1; snake->y[0] = 0; // head
    snake->x[1] = 0; snake->y[1] = 0; // body
    snake->dir = DIR_RIGHT;
}

void snake_destroy(Snake *snake) {
    free(snake->x);
    free(snake->y);
}

void snake_move(Snake *snake, World *world, char input) {
    SnakeDirection next_dir = input_to_dir(input);
    if (next_dir != DIR_NONE && !is_reverse(snake->dir, next_dir)) {
        snake->dir = next_dir;
    }
    // Clear previous snake positions (but not obstacles or fruit)
    for (size_t i = 0; i < world->w * world->h; ++i) {
        if (world->grid[i] == 'S' || world->grid[i] == 's') {
            world->grid[i] = '.';
        }
    }
    for (size_t i = snake->size - 1; i > 0; --i) {
        snake->x[i] = snake->x[i-1];
        snake->y[i] = snake->y[i-1];
    }
    size_t x = snake->x[0];
    size_t y = snake->y[0];
    // Wall collision logic
    if (world->obstacles) {
        switch (snake->dir) {
            case DIR_UP:    if (y == 0) { world->game_over = 1; return; } else { y = y - 1; } break;
            case DIR_DOWN:  if (y + 1 == world->h) { world->game_over = 1; return; } else { y = y + 1; } break;
            case DIR_LEFT:  if (x == 0) { world->game_over = 1; return; } else { x = x - 1; } break;
            case DIR_RIGHT: if (x + 1 == world->w) { world->game_over = 1; return; } else { x = x + 1; } break;
            default: break;
        }
    } else {
        switch (snake->dir) {
            case DIR_UP:    y = (y == 0) ? world->h - 1 : y - 1; break;
            case DIR_DOWN:  y = (y + 1 == world->h) ? 0 : y + 1; break;
            case DIR_LEFT:  x = (x == 0) ? world->w - 1 : x - 1; break;
            case DIR_RIGHT: x = (x + 1 == world->w) ? 0 : x + 1; break;
            default: break;
        }
    }
    snake->x[0] = x;
    snake->y[0] = y;
    // Draw snake head
    world->grid[y * world->w + x] = 'S';
    // Draw snake body
    for (size_t i = 1; i < snake->size; ++i) {
        world->grid[snake->y[i] * world->w + snake->x[i]] = 's';
    }
}
