#ifndef TIMED_LOGGER_H_
#define TIMED_LOGGER_H_

#include <time.h>

#define NEVER_WRITTEN -1

struct loginfo
{
    FILE* file;
    time_t last_stamp;
    time_t diff;
};

void timestamped_write(struct loginfo *log, const char* str);

#endif
