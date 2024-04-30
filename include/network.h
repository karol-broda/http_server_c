#ifndef NETWORK_H
#define NETWORK_H

extern int server_fd;

void run_server();
void signal_handler(int signal_number);

#endif
