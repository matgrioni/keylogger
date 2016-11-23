#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <pcap.h>

#include "ip.h"
#include "http.h"

#define TCP_PROTOCOL 6

/* TODO: Can these be used in pcap filter */
#define HTTP_PORT 80
#define HTTPS_PORT 443

/* TODO: Uncomment this. static const int PCAP_ERRBUF_SIZE = 100 */

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    FILE *fd = (FILE*) args;
    const u_char *payload;

    const struct ethernet_header *eth_header;
    const struct ip_header *ip_header;
    const struct tcp_header *tcp_header;

    u_int size_ip;
    u_int size_tcp;
    u_int size_payload;

    eth_header = (struct ethernet_header*) packet;
    ip_header = (struct ip_header*) (packet + ETHERNET_HEADER_LEN);
    size_ip = IP_HL(ip_header) * 4;
    if (size_ip < 20)
    {
        printf("Invalid packet received. Exiting...");
        exit(1);
    }

    tcp_header = (struct tcp_header*) (packet + ETHERNET_HEADER_LEN + size_ip);
    size_tcp = TH_OFF(tcp_header) * 4;
    if (size_tcp < 20)
    {
        printf("Invalid packet received. Exiting...");
        exit(1);
    }

    size_payload = ntohs(ip_header->ip_len) - (size_ip + size_tcp);
    payload = packet + ETHERNET_HEADER_LEN + size_ip + size_tcp;
    if (size_payload > 0)
    {
        if (is_http_request(payload, size_payload))
        {
            fprintf(fd, "%s\n", payload);
        }
    }
}

int main()
{
    FILE *network_log = fopen("log/network.txt", "a");

    /* Variables for pcap sniffing, such as error buffer, device to be read
       and filters, masks, etc. */
    char *dev, errbuf[100];
    pcap_t *handle;
    struct bpf_program filter;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct pcap_pkthdr header;

    /* Create the default pcap device and exit if error occurs */
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL)
    {
        printf("No default device: %s. Exiting...", errbuf);
        exit(1);
    }

    /* Find the netmask for the chosen device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) < 0)
    {
        printf("Can't get netmask for device: %s. Exiting...", errbuf);
        exit(1);
    }

    /* Create a handle to start sniffint packets in non promiscuous
       mode. */
    handle = pcap_open_live(dev, 1518, true, 1000, errbuf);
    if (handle == NULL)
    {
        printf("Couldn't open device: %s. Exiting...", errbuf);
        exit(1);
    }

    /* Handle the creation of the filter compilation and installing */
    if (pcap_compile(handle, &filter, "tcp port 80 or tcp port 443", 0, net) < 0)
    {
        printf("Couldn't create filter: %s. Exiting...", pcap_geterr(handle));
        exit(1);
    }

    if (pcap_setfilter(handle, &filter) < 0)
    {
        printf("Couldn't install filter: %s. Exiting...", pcap_geterr(handle));
        exit(1);
    }

    pcap_loop(handle, -1, process_packet, (u_char*) network_log);

    pcap_freecode(&filter);
    pcap_close(handle);

    return 0;
}
