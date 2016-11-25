#include <unistd.h>

/*
 * Check if there is a process running with the given id.
 */
int exists_process_by_id(pid_t pid);

/*
 * Periodically check for the existence of the provided process every
 * s seconds. Put in a limit of seconds to check, or a negative number
 * to check indefinitely.
 */
void process_periodic_check(pid_t pid, int s, int limit, void (*cbk)(void));
