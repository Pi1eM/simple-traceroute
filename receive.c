// Piotr Kowalczyk
// 291392


#include "srlib.h"


int rec_icmp_reqs(int sock, int ttl, int id, clock_t send_time, char *ip_addr);
void print_addr_time(char *ip_addrs[3], double times[3], int k);


int rec_icmp_reqs(int sock, int ttl, int id, clock_t send_time, char *ip_addr)
{
    printf("%d. ", ttl);

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sock, &descriptors);
    struct timeval tv;
    tv.tv_sec = 1; 
    tv.tv_usec = 0;

    char *ip_addrs[3];
    double times[3];
    int ret = 0;
    int k = 0;
    int i = 0;
    while(i < 3)
    {
        int ready = select(sock + 1, &descriptors, NULL, NULL, &tv);
        clock_t rec_time = clock();
        double time = (double)(rec_time - send_time)/((double)CLOCKS_PER_SEC/1000);
        if (ready < 0)
        {
            fprintf(stderr, "\nError: select: %s\n", strerror(errno));
            return -1;
        }
        if (ready == 0)
        {
            print_addr_time(ip_addrs, times, k);
            return ret;
        }

        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);
        u_int8_t buffer[IP_MAXPACKET];
        ssize_t packet_len = recvfrom(sock, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
        if (packet_len <= 0)
        {
            fprintf(stderr, "\nError: recvfrom: %s\n", strerror(errno));
            return -1;
        }

        char sender_ip_str[20];
        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
        
        struct iphdr* ip_header = (struct iphdr*)buffer;
        u_int8_t* icmp_packet = buffer + 4 * ip_header->ihl;
        struct icmphdr* icmp_header = (struct icmphdr*)icmp_packet;

        if (icmp_header->type == ICMP_TIME_EXCEEDED)
        {
            struct iphdr* ip_rec = 
                (struct iphdr*)(icmp_packet + sizeof(struct icmphdr));
            u_int8_t* icmp_recpack = 
                (u_int8_t*)ip_rec + sizeof(struct iphdr);
            struct icmphdr* icmp_rec = (struct icmphdr*)icmp_recpack;
            if (icmp_rec->un.echo.id == id &&
                    icmp_rec->un.echo.sequence >= 3 * ttl)
            {
                times[i] = time;
                ip_addrs[i] = sender_ip_str;
                k++;
            }
            else continue;
        }
        else if (icmp_header->type == ICMP_ECHOREPLY)
        {
            if (icmp_header->un.echo.id == id 
                && icmp_header->un.echo.sequence >= ttl)
            {
                times[i] = time;
                ip_addrs[i] = ip_addr;
                k++;
                ret++;
            }
        }
        else continue;
        i++;
    }
    print_addr_time(ip_addrs, times, k);
    return ret;
}


void print_addr_time(char *ip_addrs[3], double times[3], int k)
{
    if (k == 0) 
    {
        printf("*\n");
        return;
    }
    printf("%s ", ip_addrs[0]);
    if (strcmp(ip_addrs[0], ip_addrs[1]) && k > 1)
    {
        printf("%s ", ip_addrs[1]);
        if ((strcmp(ip_addrs[0], ip_addrs[2]) || strcmp(ip_addrs[1], ip_addrs[2])) && k > 2)
            printf("%s ", ip_addrs[2]);
    }
    if (k == 3)
        printf("%.3f ms\n", (times[0]+times[1]+times[2])/3);
    else printf("???\n");
}


