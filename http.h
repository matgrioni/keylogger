/*
 * The various HTTP methods that can be used in an HTTP request. These will be
 * used to determine if a given payload or string reprsents an HTTP request.
 * This will be accomplished through the is_http_request method.
 *
 * is_http_request method simply checks if the first word in a given payload
 * matches a possible HTTP method.
 */

#include <string.h>

int is_http_request(const char *payload, int size);
