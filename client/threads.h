#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

#include <pthread.h>

// Thread for handling user input and sending to server
typedef struct {
    int sockfd;
    volatile int *running;
    volatile int *paused; // Add paused flag
} InputThreadArgs;

void *input_thread_func(void *arg);

// Thread for receiving data from server and rendering
typedef struct {
    int sockfd;
    volatile int *running;
    volatile int *paused; // Add paused flag
} ReceiverThreadArgs;

void *receiver_thread_func(void *arg);

#endif // CLIENT_THREADS_H
