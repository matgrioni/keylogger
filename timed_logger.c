#include <stdio.h>

#include "timed_logger.h"

void timestamped_write(struct loginfo *log, const char* str)
{
    time_t cur_time = time(NULL);
    if (cur_time - log->diff > log->last_stamp)
    {
        char* conv = ctime(&cur_time);
        log->last_stamp = cur_time;
        fprintf(log->file, "%s\n", conv);
    }

    fprintf(log->file, "%s", str);
}
