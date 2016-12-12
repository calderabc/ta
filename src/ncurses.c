#include <curses.h>

#include "ncurses.h"
#include "communicate.h"
#include "log.h"

#define LEFT_EDGE 2

void nc_init()
{
    log_write("nc_out_init--start");
    initscr();
    cbreak();
    noecho();
    log_write("nc_out_init--end");
}

void nc_dest()
{
}

void nc_play_time_str(const char *time_str)
{
    mvprintw(LINES - 2, LEFT_EDGE, "Play Time: %s", time_str);
    refresh();
}


void nc_track_list(Track *track_list, int track_count)
{
    log_write_int("nc_track_list--start: track_count", track_count);
    if (track_list == NULL)
        return;

    log_write("nc_track_list: track_count was not NULL.");

    int i;
    for(i = 0; i < track_count; i++)
    {
        log_write_int("Printing track info. Track no", track_list[i].number);
        mvprintw(1 + i, 2, "%i: %s", track_list[i].number, track_list[i].name);
    }
    refresh();
    log_write("nc_track_list--end");
}

void nc_track(Track *track)
{
    log_write("nc_track--start");

    char duration_str[11];
    seconds_to_str(duration_str, (int)track->duration);

    mvprintw(LINES - 5, LEFT_EDGE, "Playing:");
    mvprintw(LINES - 4, LEFT_EDGE, "%i - %s", track->number, track->name);
    /* mvprintw(LINES - 3, LEFT_EDGE, "%i", track.sample_rate); */
    mvprintw(LINES - 2, COLS - 9, duration_str);

    refresh();

    log_write("nc_track--end");
}

void nc_operation(OutCode operation)
{
}
