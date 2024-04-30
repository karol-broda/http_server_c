#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdbool.h>

bool parse_path_req(const char *request, char *path, size_t path_size);
bool starts_with(const char *pre, const char *str);
bool check_path(const char *request, const char *custom_path);
void get_last_path_part(const char *path, char *segment, size_t segment_size);
void parse_header(const char *request, char *userAgent, size_t userAgentSize, char *headerPrefix);

#endif // UTILS_H
