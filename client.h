#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <string.h>       // strlen
#include <sys/socket.h>   // socket
#include <arpa/inet.h>    // inet_addr
#include <unistd.h>       // write
#include <sys/stat.h>
#include <sys/types.h>

void start_client(void);

#endif

