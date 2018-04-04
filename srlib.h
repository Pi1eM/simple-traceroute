#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>


#ifndef SRLIB_H
#define SRLIB_H


int check_data(char *ip_addr);
int send_packs(char *ip_addr, int id, int ttl, int sock);

int rec_icmp_reqs(int sock, int ttl, int id, clock_t send_time, char *ip_addr);


#endif
