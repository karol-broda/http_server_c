#include "utils.h"
#include <string.h>
#include <stdbool.h>
#include "main.h"

bool parse_path_req(const char *request, char *path, size_t path_size)
{
    const char *start = strstr(request, " ");
    if (!start)
        return false;

    start++;
    const char *end = strstr(start, " ");
    if (!end)
        end = request + strlen(request);

    size_t len = end - start;
    if (len > path_size - 1)
        len = path_size - 1;

    strncpy(path, start, len);
    path[len] = '\0';
    return true;
}

bool starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr >= lenpre && strncmp(pre, str, lenpre) == 0;
}

void parse_header(const char *request, char *userAgent, size_t userAgentSize, char *headerPrefix)
{
    const char *start = strstr(request, headerPrefix);
    if (start)
    {
        start += strlen(headerPrefix);
        const char *end = strstr(start, "\r\n");
        if (end)
        {
            size_t len = (end - start < userAgentSize) ? (end - start) : (userAgentSize - 1);
            strncpy(userAgent, start, len);
            userAgent[len] = '\0';
        }
    }
}

bool check_path(const char *request, const char *custom_path)
{
    char path[1024];
    if (!parse_path_req(request, path, sizeof(path)))
        return false;

    size_t custom_path_len = strlen(custom_path);
    if (custom_path[custom_path_len - 1] == '*')
    {
        char prefix[1024];
        strncpy(prefix, custom_path, custom_path_len - 1);
        prefix[custom_path_len - 1] = '\0';
        return starts_with(prefix, path);
    }
    return strcmp(path, custom_path) == 0;
}

void get_last_path_part(const char *path, char *segment, size_t segment_size)
{
    const char *last_slash = strrchr(path, '/');
    if (!last_slash)
    {
        strncpy(segment, path, segment_size);
    }
    else
    {
        last_slash++;
        size_t len = strlen(last_slash);
        if (len > segment_size - 1)
            len = segment_size - 1;
        strncpy(segment, last_slash, len);
        segment[len] = '\0';
    }
}
