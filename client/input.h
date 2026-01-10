#ifndef CLIENT_INPUT_H
#define CLIENT_INPUT_H

// Input handling for client (terminal raw mode)

int input_init(void);
void input_restore(void);

// read next key sequence into buf, return number of bytes
int input_read(char *buf, int bufsize);

// Reads arrow keys and returns DIR_UP/DOWN/LEFT/RIGHT or 0 if not an arrow
int input_read_arrow(void);

// Reads WASD keys and returns 'W', 'A', 'S', 'D' or 0 if not a movement key
int input_read_wasd(void);

#endif // CLIENT_INPUT_H
