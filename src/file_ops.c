#include "file_ops.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <ncurses.h>

void handle_file_request(int client_fd, const char *path)
{
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_directory, path);

    printw("dir: %s, path: %s\n", base_directory, full_path);

    FILE *file = fopen(full_path, "rb");
    if (file == NULL)
    {
        char response404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, response404, strlen(response404), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size);
    if (buffer == NULL)
    {
        fclose(file);
        char response500[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        send(client_fd, response500, strlen(response500), 0);
        return;
    }

    fread(buffer, 1, file_size, file);
    char header[1024];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %ld\r\n\r\n", file_size);
    send(client_fd, header, strlen(header), 0);
    send(client_fd, buffer, file_size, 0);

    fclose(file);
    free(buffer);
}

void save_file_from_post(int client_fd, const char *path, const char *data)
{
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_directory, path);

    FILE *file = fopen(full_path, "wb");
    if (file == NULL)
    {
        char response500[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        send(client_fd, response500, strlen(response500), 0);
        return;
    }

    fwrite(data, 1, strlen(data), file);
    fclose(file);

    char response201[] = "HTTP/1.1 201 Created\r\n\r\nFile saved successfully.";
    send(client_fd, response201, strlen(response201), 0);
}

void stream_file(int client_fd, const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        perror("File opening failed");
        char response500[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n500 - Internal Server Error";

        send(client_fd, response500, strlen(response500), 0);
        close(client_fd);
        return;
    }

    char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n";

    if (send(client_fd, response_header, strlen(response_header), 0) == -1)
    {
        perror("Error sending response header");
        fclose(file);
        close(client_fd);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
            len--;
        }

        char json_chunk[512];

        snprintf(json_chunk, sizeof(json_chunk), "{\"line\": \"%s\"}\r\n", buffer);
        size_t json_len = strlen(json_chunk);
        char chunk_size[16];
        snprintf(chunk_size, sizeof(chunk_size), "%zx\r\n", json_len);

        if (send(client_fd, chunk_size, strlen(chunk_size), 0) == -1)
        {
            perror("Error sending chunk size");
            fclose(file);
            close(client_fd);
            return;
        }

        if (send(client_fd, json_chunk, json_len, 0) == -1)
        {
            perror("Error sending chunk");
            fclose(file);
            close(client_fd);
            return;
        }
        if (send(client_fd, "\r\n", 2, 0) == -1)
        {
            perror("Error sending chunk end");
            fclose(file);
            close(client_fd);
            return;
        }
        usleep(500000);
    }

    if (send(client_fd, "0\r\n\r\n", 5, 0) == -1)
    {
        perror("Error sending final chunk");
    }

    fclose(file);
    close(client_fd);
}
