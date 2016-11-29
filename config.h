#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <stdio.h>
/*Config structure*/
typedef struct Config{
    
    char* device_file;
    int  kb_fd;
    FILE* keylog_log;
    
}Config;

/*Free up space used by config struct*/
void config_cleanup(Config *config);

#endif
