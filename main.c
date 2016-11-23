#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <pcap.h>

#include "ip_header.h"

#define TCP_PROTOCOL 6

/* TODO: Can these be used in pcap filter */
#define HTTP_PORT 80
#define HTTPS_PORT 443

/* TODO: Uncomment this. static const int PCAP_ERRBUF_SIZE = 100 */

int main()
{
    /* Variables for pcap sniffing, such as error buffer, device to be read
       and filters, masks, etc. */
    char *dev, errbuf[100];
    pcap_t *handle;
    struct bpf_program filter;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct pcap_pkthdr header;

    /* Information and structs for parsing the packet read by pcap */
    const u_char *packet, *payload;

    const struct ethernet_header *eth_header;
    const struct ip_header *ip_header;
    const struct tcp_header *tcp_header;

    u_int size_ip;
    u_int size_tcp;

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
    handle = pcap_open_live(dev, BUFSIZ, false, 3000, errbuf);
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

    packet = pcap_next(handle, &header);

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

    payload = packet + ETHERNET_HEADER_LEN + size_ip + size_tcp;
    printf("%s", payload);

    pcap_close(handle);

    return 0;
}
