nclude <stdio.h>
#include <stdlib.h>
#include <string.h>      
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "server.h"

void swap(char* a, char* b) {
    *a = (*a) ^ (*b);
    *b = (*a) ^ (*b);
    *a = (*a) ^ (*b);
}

void toggle_capitalization(char* ch) {
    if (*ch >= 97 && *ch <= 122) {
        // a to z
        *ch = (*ch) - 32;
    } else if (*ch >= 65 && *ch <= 90) {
        // A to Z
        *ch = (*ch) + 32;
    }
}

void reverse_order(char* str) {
    int len = strlen(str);
    int idx = 0;
    for (; idx < len / 2; idx++) {
        swap(str + idx, str + len - idx - 1);
    }
}

void reverse_capitalization(char* str) {
    int len = strlen(str);
    int idx = 0;
    for (; idx < len; idx++) {
        toggle_capitalization(str + idx);
    }
}

int main(int argc , char *argv[])
{
    // Variables
    int ssock, csock, addrlen, read_size, ret;
    struct sockaddr_in server, client;
    char message[2000];
    
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
        //print the error message
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
    
    memset(message, 0, sizeof(message));
    // Receive a message from the client. recv() is an alternative for read().
    // Similarly, send() can replace write().
    if ((read_size = read(csock, message, sizeof(message))) > 0) {
        printf("Client: %s\n", message);
        reverse_order(message);
        reverse_capitalization(message);
        printf("After processing: %s\n", message);
        if ((ret = write(csock, message, strlen(message))) <= 0) {
            perror("Error writing\n");
        }
    } else {
        perror("Error receiving\n");
    }
    
    
    close(csock);
    close(ssock);
    printf("End connection\n");
    
    return 0;
}
