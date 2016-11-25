#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>

#include "process.h"

int exists_task_by_id(int pid)
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
