#include <stdlib.h>

#include "config.h"

void config_cleanup(struct Config *config) {
	free(config->device_file);
	config->device_file = NULL;
    config->kb_fd = 0;
}
