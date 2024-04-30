#ifndef HTTP_HANDLERS_H
#define HTTP_HANDLERS_H

void *handle_connection(void *client_socket);
char *get_post_body(const char *request);

#endif // HTTP_HANDLERS_H
