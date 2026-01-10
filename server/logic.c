#include "logic.h"
#include "snake.h"
#include <stdlib.h>
#include <time.h>

// Forward declarations for snake helpers
SnakeDirection input_to_dir(char input);
int is_reverse(SnakeDirection current, SnakeDirection next);

// Helper: check if position is inside snake
static int is_in_snake(const Snake *snake, size_t x, size_t y) {
    for (size_t i = 0; i < snake->size; ++i) {
        if (snake->x[i] == x && snake->y[i] == y) return 1;
    }
    return 0;
}

// Helper: get position in front of snake head
static void get_front_of_head(const Snake *snake, size_t w, size_t h, size_t *fx, size_t *fy) {
    size_t x = snake->x[0], y = snake->y[0];
    switch (snake->dir) {
        case DIR_UP:    *fx = x; *fy = (y == 0) ? h - 1 : y - 1; break;
        case DIR_DOWN:  *fx = x; *fy = (y + 1 == h) ? 0 : y + 1; break;
        case DIR_LEFT:  *fx = (x == 0) ? w - 1 : x - 1; *fy = y; break;
        case DIR_RIGHT: *fx = (x + 1 == w) ? 0 : x + 1; *fy = y; break;
        default: *fx = x; *fy = y; break;
    }
}

// Helper: place food at random valid position
static void place_food(World *w, const Snake *snake) {
    size_t fx, fy;
    get_front_of_head(snake, w->w, w->h, &fx, &fy);
    srand((unsigned int)time(NULL));
    while (1) {
        size_t x = rand() % w->w;
        size_t y = rand() % w->h;
        if (!is_in_snake(snake, x, y) && !(x == fx && y == fy)) {
            w->food_x = x;
            w->food_y = y;
            break;
        }
    }
}

static Snake *snake = NULL;
static int snake_initialized = 0;

void logic_apply_input(World *w, char input) {
    if (!snake_initialized) {
        snake = malloc(sizeof(Snake));
        snake_init(snake, w->w, w->h);
        place_food(w, snake);
        snake_move(snake, w, 0); // draw initial snake
        w->grid[w->food_y * w->w + w->food_x] = 'o';
        snake_initialized = 1;
    }
    // Only update direction if input is not a reverse
    SnakeDirection next_dir = input_to_dir(input);
    if (next_dir != DIR_NONE && !is_reverse(snake->dir, next_dir)) {
        snake->dir = next_dir;
    }
    // Move snake
    size_t prev_tail_x = snake->x[snake->size-1];
    size_t prev_tail_y = snake->y[snake->size-1];
    snake_move(snake, w, 0);
    // Check self-collision and end game immediately
    for (size_t i = 1; i < snake->size; ++i) {
        if (snake->x[0] == snake->x[i] && snake->y[0] == snake->y[i]) {
            w->game_over = 1;
            logic_cleanup();
            return; // End function, no further actions
        }
    }
    // Check food collision
    if (snake->x[0] == w->food_x && snake->y[0] == w->food_y) {
        // Grow snake
        if (snake->size < snake->max_size) {
            snake->size++;
            snake->x[snake->size-1] = prev_tail_x;
            snake->y[snake->size-1] = prev_tail_y;
        }
        place_food(w, snake);
    }
    w->grid[w->food_y * w->w + w->food_x] = 'o';
}

void logic_draw_initial(World *w) {
    if (!snake_initialized) {
        snake = malloc(sizeof(Snake));
        snake_init(snake, w->w, w->h);
        place_food(w, snake);
        snake_move(snake, w, 0); // draw initial snake
        w->grid[w->food_y * w->w + w->food_x] = 'o';
        snake_initialized = 1;
    } else {
        snake_move(snake, w, 0);
        w->grid[w->food_y * w->w + w->food_x] = 'o';
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
