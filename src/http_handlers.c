#include "http_handlers.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "main.h"
#include <errno.h>
#include "file_ops.h"
#include <sys/socket.h>

void *handle_connection(void *client_socket)
{
    int client_fd = *((int *)client_socket);
    free(client_socket);

    char client_request[4096] = {0};
    int request_size = recv(client_fd, client_request, sizeof(client_request), 0);
    if (request_size < 0)
    {
        printf("err: reading from socket: %s\n", strerror(errno));
        close(client_fd);
        return NULL;
    }

    char path[1024];
    if (!parse_path_req(client_request, path, sizeof(path)))
    {
        printf("err: path parsing\n");
        return NULL;
    }
    printf("request: %s\n", client_request);
    if (check_path(client_request, "/"))
    {
        char response200[] = "HTTP/1.1 200 OK\r\n\r\nfinally";
        send(client_fd, response200, strlen(response200), 0);
    }
    else if (check_path(client_request, "/echo/*"))
    {
        char last_segment[1024];
        get_last_path_part(path, last_segment, sizeof(last_segment));
        char response[2048];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n%s", strlen(last_segment), last_segment);
        send(client_fd, response, strlen(response), 0);
    }
    else if (check_path(client_request, "/user-agent"))
    {
        char userAgent[1024] = {0};
        parse_header(client_request, userAgent, sizeof(userAgent), "User-Agent: ");
        char response[2048];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n%s", strlen(userAgent), userAgent);
        send(client_fd, response, strlen(response), 0);
    }
    else if (check_path(client_request, "/html-test"))
    {
        FILE *file = fopen("./tests/data/index.html", "rb");
        if (!file)
        {
            perror("File opening failed");
            char response500[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n500 - Internal Server Error";
            send(client_fd, response500, strlen(response500), 0);
        }
        else
        {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);
            char *file_contents = (char *)malloc(file_size + 1);
            fread(file_contents, file_size, 1, file);
            file_contents[file_size] = '\0';

            char response_header[512];
            snprintf(response_header, sizeof(response_header), "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %ld\r\n\r\n", file_size);
            send(client_fd, response_header, strlen(response_header), 0);
            send(client_fd, file_contents, file_size, 0);
            free(file_contents);
            fclose(file);
        }
    }
    else if (check_path(client_request, "/random-lord"))
    {
        FILE *file = fopen("./tests/data/der_lord.txt", "rb");
        if (!file)
        {
            perror("File opening failed");
            char response500[] = "HTTP/1.1 500 Internal Server Error\r\n\r\n500 - Internal Server Error";
            send(client_fd, response500, strlen(response500), 0);
        }
        else
        {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);

            int line_count = 0;
            char ch;
            while (!feof(file))
            {
                ch = fgetc(file);
                if (ch == '\n')
                {
                    line_count++;
                }
            }

            int random_line = rand() % line_count;
            rewind(file);
            int current_line = 0;
            while (current_line < random_line)
            {
                ch = fgetc(file);
                if (ch == '\n')
                {
                    current_line++;
                }
            }

            char line_buffer[1024];
            fgets(line_buffer, sizeof(line_buffer), file);

            char response_header[512];
            snprintf(response_header, sizeof(response_header), "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: %ld\r\n\r\n", strlen(line_buffer));
            send(client_fd, response_header, strlen(response_header), 0);
            send(client_fd, line_buffer, strlen(line_buffer), 0);

            fclose(file);
        }
    }
    else if (strncmp(client_request, "POST ", 5) == 0 && starts_with("/files/", path))
    {
        char *file_name = path + 7;
        char *post_data = get_post_body(client_request);
        if (post_data)
        {
            save_file_from_post(client_fd, file_name, post_data);
            free(post_data);
        }
        else
        {
            char response404[] = "HTTP/1.1 404 Not Found\r\n\r\n404 - Not Found";
            send(client_fd, response404, strlen(response404), 0);
        }
    }
    else if (check_path(client_request, "/files/*"))
    {
        char last_segment[1024];
        get_last_path_part(path, last_segment, sizeof(last_segment));
        handle_file_request(client_fd, last_segment);
    }
    else
    {
        char response404[] = "HTTP/1.1 404 Not Found\r\n\r\n404 - Not Found";
        send(client_fd, response404, strlen(response404), 0);
    }

    close(client_fd);
    return NULL;
}

char *get_post_body(const char *request)
{
    const char *body_start = strstr(request, "\r\n\r\n");
    if (!body_start)
    {
        return NULL;
    }
    return strdup(body_start + 4);
}