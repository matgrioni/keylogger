#ifndef _SERVER_CLIENT_H_
#define _SERVER_CLIENT_H_

#include <string.h>       // strlen
#include <sys/socket.h>   // socket
#include <arpa/inet.h>    // inet_addr
#include <unistd.h>       // write
#include <fcntl.h>

void run_client(void);
void run_server(void);

#endif

