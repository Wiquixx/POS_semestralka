#include "input.h"
#include <stdio.h>
#include <string.h>

// Very simple, portable input helper for students.
// It reads a whole line (up to bufsize-1 chars) using fgets and returns
// the number of characters (excluding the terminating NUL). On error it
// returns -1.

int input_init(void) { return 0; }

void input_restore(void) { /* nothing to restore in this simple version */ }

int input_read(char *buf, int bufsize) {
    if (!buf || bufsize <= 0) return -1;
    if (!fgets(buf, bufsize, stdin)) return -1;
    // remove trailing newline if present
    size_t n = strcspn(buf, "\n");
    buf[n] = '\0';
    return (int)n;
}
