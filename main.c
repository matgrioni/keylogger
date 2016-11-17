#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <unistd.h>

static const int HTTP_PORT = 80;
static const int HTTPS_PORT = 443;

static const int PACKET_BUFFER_SIZE = 1000;

int main()
{
    int socket_fd;
    int recv_size;
    struct sockaddr conn_in;
    char packet[PACKET_BUFFER_SIZE];

    socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socket_fd < 0)
    {
        printf("Error creating socket. Exiting...");
        exit(1);
    }

    while (1)
    {
        socklen_t conn_in_size = sizeof(conn_in);
        recv_size = recvfrom(socket_fd, packet, PACKET_BUFFER_SIZE, 0, &conn_in, &conn_in_size);
        
        if (recv_size < 0)
        {
            printf("Error receiving packet. Exiting...");
            exit(1);
        }

        struct iphdr *iph = (struct iphdr*) packet;
        if (iph->protocol == 6)
            printf("TCP baby!");
    }

    close(socket_fd);

    return 0;
}
