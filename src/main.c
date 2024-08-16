#include "main.h"
#include "network.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ncurses.h>

char base_directory[1024] = {0};
int port = 4221;
int use_ncurses = 1;
pthread_mutex_t ncurses_mutex;
pthread_t key_listener_thread;
volatile int running = 1;

int main(int argc, char **argv)
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, NULL);

    if (sigaction(SIGPIPE, &sa, NULL) != 0)
    {
        perror("Failed to set SIGPIPE handler");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--directory") == 0 || strcmp(argv[i], "-d") == 0) && i + 1 < argc)
        {
            strncpy(base_directory, argv[i + 1], sizeof(base_directory) - 1);
            base_directory[sizeof(base_directory) - 1] = '\0';
            i++;
        }
        else if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) && i + 1 < argc)
        {
            port = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "--no-ui") == 0)
        {
            use_ncurses = 0;
        }
    }

    if (use_ncurses)
    {
        init_ncurses();
        pthread_mutex_init(&ncurses_mutex, NULL);
        if (pthread_create(&key_listener_thread, NULL, key_listener, NULL) != 0)
        {
            perror("Failed to create key listener thread");
            exit(EXIT_FAILURE);
        }
    }

    run_server();

    if (use_ncurses)
    {
        pthread_join(key_listener_thread, NULL);
        cleanup_ncurses();
        pthread_mutex_destroy(&ncurses_mutex);
    }

    return 0;
}
