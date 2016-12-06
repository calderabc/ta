#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "ui.h"
#include "player.h"
#include "input.h"
#include "output.h"
#include "communicate.h"
#include "log.h"

#define USE_FFMPEG
#ifdef USE_FFMPEG
#include "ffmpeg.h"
#endif

static inline
pid_t fork_me(void (*go)(int), int fd)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        /* Executing the child process. */
        go(fd);
        /* TODO: What should the child process return? */
        /* It shouldn't matter because the 'go' functions
          should only return on program termination. */
        return 0;
    }
    else
        /* Executing the parent process. */
        return pid;

    /* TODO: Add error checking if process cannot be forked */
    /* pid returned to parent would be -1 so check errno */
}


/* Shutdown everything and cleanup. */
static inline
void ta_dest(pid_t in_pid, pid_t out_pid, pthread_t plr_thread_id)
{
    /* todo: send catchable signals to input, output, and player, and make
     * them terminate gracefully. */
    /* kill input process */
    kill(in_pid, SIGKILL);
    /* kill output process */
    kill (out_pid, SIGKILL);
    /* terminate the player thread. */
    pthread_kill(plr_thread_id, SIGKILL);

    /* TODO: Do any other program cleanup. */
}


int main (int argc, char* argv[])
{
    /* init_ui(); */

    /* File descriptors for the communication pipes.
       'in' == input, 'out' == output,  'plr' == player */
    int in_write_to_main;   /* input  */
    int main_read_from_in;  /*________*/
    int main_write_to_out;  /* output */
    int out_read_from_main; /*________*/
    int plr_write_to_main;  /* player */
    int main_read_from_plr; /*        */
    int main_write_to_plr;  /*        */
    int plr_read_from_main; /*________*/

    /* Create pipe from input to main then fork input process. */
    comm_connect(&main_read_from_in, &in_write_to_main);
    pid_t in_pid = fork_me(&in_process_go, in_write_to_main);

    /* Create pipe from main to output then fork output process. */
    comm_connect(&out_read_from_main, &main_write_to_out);
    pid_t out_pid = fork_me(&out_process_go, out_read_from_main);

    /* Create read/write pipes between main and player. */
    comm_connect(&main_read_from_plr, &plr_write_to_main);
    comm_connect(&plr_read_from_main, &main_write_to_plr);

    /* TODO: Free up file descriptors
       which are not used by this process. */

    /* If I don't want to use defaults
    pthread_attr_t play_thread_attr
    pthread_attr_init(&create);
    */

    char *file_names[] = {"/home/acalder/music/Steven_Wilson/Hand._Cannot._Erase./10.Happy_Returns.flac",
                          "/home/acalder/music/Steven_Wilson/Hand._Cannot._Erase./11.Ascendant_Here_On....flac"};

    /* Spawn player thread. */
    pthread_t plr_thread_id;
    pthread_create(&plr_thread_id,
                   NULL,
                   &plr_thread_go,
                   (void *)&file_names);
    Comm command;
    while (1)
    {

        /* printf("main is here\n"); */
        log_write("main_while_start");
        comm_recv(main_read_from_in, &command);

        /* User quit the program. */
        if ((InCode)command.code == Q || (InCode)command.code == ESC)
        {
            ta_dest(in_pid, out_pid, plr_thread_id);
            break;
        }

        comm_send(main_write_to_out, &command);
    }

  /* Holy explicit Batman! */
  exit(0);
}
