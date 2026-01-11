#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

#include <pthread.h>
#include "menu.h"

typedef struct {
    unsigned int score;
    unsigned int time;
} GameState;

// Thread for handling user input and sending to server
typedef struct {
    int sockfd;
    volatile int *running;
    volatile int *paused; // Add paused flag
    MenuState *menu_state;
    GameState *game_state;
    pthread_mutex_t *game_state_mutex;
    pthread_mutex_t *menu_state_mutex;
} InputThreadArgs;

void *input_thread_func(void *arg);

// Thread for receiving data from server and rendering
typedef struct {
    int sockfd;
    volatile int *running;
    volatile int *paused; // Add paused flag
    MenuState *menu_state;
    GameState *game_state;
    pthread_mutex_t *game_state_mutex;
    pthread_mutex_t *menu_state_mutex;
} ReceiverThreadArgs;

void *receiver_thread_func(void *arg);

#endif // CLIENT_THREADS_H
