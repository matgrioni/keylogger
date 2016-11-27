#include <stdlib.h>

#include "config.h"

void config_cleanup(Config *config) {
	free(config->log_file);
	free(config->device_file);
	config->log_file = NULL;
	config->device_file = NULL;
}
