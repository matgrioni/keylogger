#include "ip.h"

int process_packet(const u_char *packet, struct ethernet_header **eth, struct ip_header **ip, struct tcp_header **tcp, const u_char **payload, u_int *size_payload)
{
    u_int size_ip;
    u_int size_tcp;

    *eth = (struct ethernet_header*) packet;
    *ip = (struct ip_header*) (packet + ETHERNET_HEADER_SIZE);
    size_ip = IP_HL(*ip) * 4;
    if (size_ip < 20)
    {
        *payload = '\0';
        return -1;
    }

    *tcp = (struct tcp_header*) (packet + ETHERNET_HEADER_SIZE + size_ip);
    size_tcp = TH_OFF(*tcp) * 4;
    if (size_tcp < 20)
    {
        *payload = '\0';
        return -1;
    }

    *size_payload = ntohs((*ip)->ip_len) - (size_ip + size_tcp);
    *payload = packet + ETHERNET_HEADER_SIZE + size_ip + size_tcp;

    return 0;
}
