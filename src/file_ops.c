#include "file_ops.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

void handle_file_request(int client_fd, const char *path)
{
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_directory, path);

    printf("dir: %s, path: %s\n", base_directory, full_path);

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
