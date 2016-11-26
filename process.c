#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>

#include "process.h"

int exists_process_by_id(pid_t pid)
{
    DIR *dp;
    struct dirent *ep;

    dp = opendir("/proc/");
    if (dp != NULL)
    {
        while ((ep = readdir(dp)))
        {
            int cur_id = atoi(ep->d_name);
            if (pid == cur_id)
                return 1;
        }
    }

    return 0;
}

pid_t process_periodic_check(pid_t pid, int s, int limit, pid_t (*cbk)(void))
{
    int cur_s = 0;
    while (cur_s < limit || limit < 0)
    {
        int exists = exists_process_by_id(pid);
        if (!exists)
            return cbk();

        sleep(s);
        cur_s += s;
    }

    return -1;
}
