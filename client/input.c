#include "input.h"
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

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


