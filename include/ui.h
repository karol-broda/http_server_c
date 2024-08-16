#ifndef UI_H
#define UI_H

void *key_listener(void *arg);
void init_ncurses();
void update_connections_display(const char *message);
void cleanup_ncurses();
void display_error(const char *msg, ...);

#endif
