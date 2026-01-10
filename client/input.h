#ifndef CLIENT_INPUT_H
#define CLIENT_INPUT_H

// Input handling for client (terminal raw mode)

int input_init(void);
void input_restore(void);

// read next key sequence into buf, return number of bytes
int input_read(char *buf, int bufsize);

#endif // CLIENT_INPUT_H

