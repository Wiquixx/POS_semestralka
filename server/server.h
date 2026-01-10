#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

// Server public API (initialization, run loop)
int server_init(void);
int server_run(void);
void server_shutdown(void);

#endif // SERVER_SERVER_H

