#include "ui.h"
#include <ncurses.h>
#include "main.h"
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

WINDOW *error_win;
WINDOW *status_win;
int max_y, max_x;

extern int use_ncurses;

void init_ncurses()
{
    if (!use_ncurses)
        return; // Do nothing if ncurses is disabled

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    start_color();

    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

    getmaxyx(stdscr, max_y, max_x);

    status_win = newwin(1, max_x, 0, 0);
    wbkgd(status_win, COLOR_PAIR(1));
    wattron(status_win, A_BOLD);
    mvwprintw(status_win, 0, 0, "Server running on port: %d", port);
    wattroff(status_win, A_BOLD);
    wrefresh(status_win);

    error_win = newwin(3, max_x, max_y - 3, 0);
    wbkgd(error_win, COLOR_PAIR(2));
    wattron(error_win, A_BOLD);
    mvwprintw(error_win, 0, 0, "Error Log:");
    wattroff(error_win, A_BOLD);
    wrefresh(error_win);

    scrollok(error_win, TRUE);
}

void update_connections_display(const char *message)
{
    if (use_ncurses)
    {
        pthread_mutex_lock(&ncurses_mutex);
        mvwprintw(status_win, 0, 0, "Server running on port: %d | %s", port, message);
        wrefresh(status_win);
        pthread_mutex_unlock(&ncurses_mutex);
    }
    else
    {
        printf("Server running on port: %d | %s\n", port, message);
    }
}

void display_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (use_ncurses)
    {
        pthread_mutex_lock(&ncurses_mutex);
        wattron(error_win, COLOR_PAIR(2) | A_BOLD);

        vwprintw(error_win, fmt, args);
        wrefresh(error_win);

        wattroff(error_win, COLOR_PAIR(2) | A_BOLD);
        pthread_mutex_unlock(&ncurses_mutex);
    }
    else
    {
        vfprintf(stderr, fmt, args);
    }

    va_end(args);
}

void cleanup_ncurses()
{
    if (!use_ncurses)
        return; // Do nothing if ncurses is disabled

    delwin(status_win);
    delwin(error_win);
    endwin();
}
