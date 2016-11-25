#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "process.h"

pid_t exec_ghost();

int main(int argc, char **argv)
{
    pid_t id;
    if (argc < 2)
        id = exec_ghost();
    else
        id = atoi(argv[1]);

    pid_t cur_pid = id;
    while(1)
    {
        cur_pid = process_periodic_check(cur_pid, 5, -1, exec_ghost);
    }

    return 0;
}

pid_t exec_ghost()
{
    pid_t parent_id = getpid();
    pid_t id = fork();

    if (id < 0)
    {
        printf("There was an error forking. Exiting...\n");
        exit(1);
    }
    else if (id == 0)
    {
        char str_id[6];
        sprintf(str_id, "%d", parent_id);

        printf("Starting ghost...\n");
        execlp("./ghost", "./ghost", str_id, NULL);
    }

    return id;
}
