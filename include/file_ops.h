#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <sys/types.h>

void handle_file_request(int client_fd, const char *path);
void save_file_from_post(int client_fd, const char *path, const char *data);
void stream_file(int client_fd, const char *file_path);
void handle_openapi_request(int client_fd);
void handle_static_file_request(int client_fd, const char *path);

#endif
