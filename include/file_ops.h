#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <sys/types.h>

void handle_file_request(int client_fd, const char *path);
void save_file_from_post(int client_fd, const char *path, const char *data);

#endif
