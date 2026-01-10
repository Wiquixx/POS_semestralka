#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <stddef.h>
#include "protocol.h"

typedef struct Client Client;

// Initialize client (allocate resources if needed)
int client_init(Client *c);

// Run main client loop (menu, rendering, input)
int client_run(Client *c);

// Clean up
void client_destroy(Client *c);

#endif // CLIENT_CLIENT_H

