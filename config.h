#ifndef _CONFIG_H_
#define _CONFIG_H_

/*Config structure*/
typedef struct Config{
   char *log_file;
   char *device_file;
}Config;

/*Free up space used by config struct*/
void config_cleanup(Config *config);

#endif
