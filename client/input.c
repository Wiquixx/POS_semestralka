#include "input.h"
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

// Very simple, portable input helper for students.
// It reads a whole line (up to bufsize-1 chars) using fgets and returns
// the number of characters (excluding the terminating NUL). On error it
// returns -1.

int input_init(void) {
    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) return -1;
    raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return -1;
    return 0;
}

void input_restore(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int input_read(char *buf, int bufsize) {
    if (!buf || bufsize <= 0) return -1;
    if (!fgets(buf, bufsize, stdin)) return -1;
    // remove trailing newline if present
    size_t n = strcspn(buf, "\n");
    buf[n] = '\0';
    return (int)n;
}

// Reads WASD keys and returns 'W', 'A', 'S', 'D' or 0 if not a movement key
int input_read_wasd(void) {
    char ch;
    if (read(STDIN_FILENO, &ch, 1) != 1) return 0;
    switch (ch) {
        case 'w': case 'W': return 'W';
        case 'a': case 'A': return 'A';
        case 's': case 'S': return 'S';
        case 'd': case 'D': return 'D';
        default: return 0;
    }
}
