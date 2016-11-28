nclude <stdio.h>
#include <stdlib.h>
#include <string.h>       // strlen
#include <sys/socket.h>   // socket
#include <arpa/inet.h>    // inet_addr
#include <unistd.h>       // write
#include <fcntl.h>

#include "client.h"


int main(int argc , char *argv[])
{
    // Variables
    int sock, read_size, ret;
    struct sockaddr_in server;
    char message[2000];
    
    char *addr = "127.0.0.1";
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
    
    printf("Enter message: ");
    scanf("%s" , message);
    
    if ((ret = write(sock, message, strlen(message))) <= 0) {
        perror("Error writing\n");
        close(sock);
        exit(0);
    }
    // Receive a message from server
    if ((read_size = read(sock, message, sizeof(message))) > 0) {
        printf("Server: %s\n", message);
    } else if(read_size == 0) {
        printf("Server disconnected\n");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("Error receiving message from server\n");
    }
    
    close(sock);
    return 0;
}
