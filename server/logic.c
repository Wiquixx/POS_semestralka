#include "logic.h"
#include "snake.h"
#include <stdlib.h>

// Forward declarations for snake helpers
SnakeDirection input_to_dir(char input);
int is_reverse(SnakeDirection current, SnakeDirection next);

static Snake *snake = NULL;
static int snake_initialized = 0;

void logic_apply_input(World *w, char input) {
    if (!snake_initialized) {
        snake = malloc(sizeof(Snake));
        snake_init(snake, w->w, w->h);
        snake_move(snake, w, 0); // draw initial snake
        snake_initialized = 1;
    }
    // Only update direction if input is not a reverse
    SnakeDirection next_dir = input_to_dir(input);
    if (next_dir != DIR_NONE && !is_reverse(snake->dir, next_dir)) {
        snake->dir = next_dir;
    }
    snake_move(snake, w, 0); // move snake forward in current direction
}

void logic_draw_initial(World *w) {
    if (!snake_initialized) {
        snake = malloc(sizeof(Snake));
        snake_init(snake, w->w, w->h);
        snake_move(snake, w, 0); // draw initial snake
        snake_initialized = 1;
    } else {
        snake_move(snake, w, 0);
    }
}

void logic_cleanup() {
    if (snake) {
        snake_destroy(snake);
        free(snake);
        snake = NULL;
        snake_initialized = 0;
    }
}
