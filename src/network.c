#include "network.h"
#include "http_handlers.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ncurses.h>
#include "main.h"
#include "ui.h"

int server_fd;

void signal_handler(int signal_number)
{
    printw("\nReceived signal to stop, shutting down...\n");
    close(server_fd);
    exit(0);
}

void run_server()
{
    pthread_mutex_init(&ncurses_mutex, NULL);
    init_ncurses();

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {htonl(INADDR_ANY)}};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket creation failed");
        return;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopt SO_REUSEADDR failed");
        return;
    }

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
    {
        perror("binding failed");
        return;
    }

    if (listen(server_fd, 10) != 0)
    {
        perror("listen failed");
        return;
    }

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*client_fd < 0)
        {
            perror("error in accepting conn");
            free(client_fd);
            continue;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_connection, client_fd) != 0)
        {
            perror("creating thraed");
            close(*client_fd);
            free(client_fd);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_fd);
}
