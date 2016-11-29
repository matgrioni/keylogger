#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <stdio.h>
/*Config structure*/
struct Config{
    char* device_file;
    int  kb_fd;
};

/*Free up space used by config struct*/
void config_cleanup(struct Config *config);

#endif
