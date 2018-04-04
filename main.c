// Piotr Kowalczyk
// 291392


#include "srlib.h"


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "\nError: bad arguments\n");
        return EXIT_FAILURE;
    }
    char *ip_addr = argv[1];
    if (check_data(ip_addr) > 0)
    {
        fprintf(stderr, "\nError: bad IP address\n");
        return EXIT_FAILURE;
    }
    int id = getpid();
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        fprintf(stderr, "\nError: socket: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    int rec = 0;
    for (int ttl = 1; rec == 0 && ttl <= 30; ttl++)
    {
        int err = send_packs(ip_addr, id, ttl, sockfd);
        if (err) 
            return EXIT_FAILURE;
        clock_t send_time = clock();
        rec = rec_icmp_reqs(sockfd, ttl, id, send_time, ip_addr);
    }

    return 0;
}


