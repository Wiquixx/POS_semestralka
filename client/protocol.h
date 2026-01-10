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

// Movement directions for WASD
#define DIR_UP    'W'
#define DIR_DOWN  'S'
#define DIR_LEFT  'A'
#define DIR_RIGHT 'D'

// Server message: last arrow
#define MSG_LAST_ARROW "LAST:"

#endif // CLIENT_PROTOCOL_H

