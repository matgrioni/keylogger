#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <unistd.h>

#define TCP_PROTOCOL 6

#define HTTP_PORT 80
#define HTTPS_PORT 443

static const int PACKET_BUFFER_SIZE = 65536;

int main()
{
    int socket_fd;
    int packet_size;
    struct sockaddr conn;
    char packet[PACKET_BUFFER_SIZE];

    socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socket_fd < 0)
    {
        printf("Error creating socket. Exiting...");
        exit(1);
    }

    while (true)
    {
        socklen_t conn_size = sizeof(conn);
        packet_size = recvfrom(socket_fd, packet, PACKET_BUFFER_SIZE, 0, &conn, &conn_size);
        
        if (packet_size < 0)
        {
            printf("Error receiving packet. Exiting...");
            exit(1);
        }

        struct iphdr *iph = (struct iphdr*) packet;
        if (iph->protocol == TCP_PROTOCOL)
        {
            unsigned short ip_header_len = iph->ihl * 4;
            struct tcphdr *tcph = (struct tcphdr*) (packet + ip_header_len);

            int total_header_size = ip_header_len + tcph->doff * 4;

            for (int i = total_header_size; i < packet_size; i++)
            {
                printf("%c", (unsigned int) packet[i]);
            }
        }
    }

    close(socket_fd);

    return 0;
}
