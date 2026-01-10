#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <stdint.h>

// Protocol shared between client and server
#define PROTO_PORT 42069

// Snapshot header sent before the grid (network byte order)
typedef struct {
    uint16_t w;
    uint16_t h;
    uint16_t score;
} SnapshotHeader;

// Messages exchanged
#define MSG_DIR 'D' // followed by one byte: U/D/L/R
#define MSG_QUIT "quit"

#endif // CLIENT_PROTOCOL_H

