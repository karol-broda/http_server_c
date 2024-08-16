#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

pthread_mutex_t ncurses_mutex;
extern char base_directory[1024];
extern int port;
extern int use_ncurses;
extern volatile int running;

#endif
