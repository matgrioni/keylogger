#include <getopt.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "config.h"
#include "util.h"
#include "device_detect.h"

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


