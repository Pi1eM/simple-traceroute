// Piotr Kowalczyk
// 291392


#include "srlib.h"

int check_data(char *ip_addr);
int send_packs(char *ip_addr, int id, int ttl, int sock);
u_int16_t compute_icmp_checksum(const void *buff, int length);


int check_data(char *ip_addr)
{
    int dots = 0;
    int prev = 0;
    for (unsigned int i = 0; i < strlen(ip_addr); i++)
    {
        if (ip_addr[i] == '.')
            if (prev < 0)
                return 1;
            else
            {
                dots++;
                prev = -1;
            }
        else if (ip_addr[i] >= '0' && ip_addr[i] <= '9')
        {
            if (prev > 2)
                return 1;
            else
                prev++;
        }
    }
    if (dots != 3 || prev == -1)
        return 1;
    return 0;
}

int send_packs(char *ip_addr, int id, int ttl, int sock)
{
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = id;
    icmp_header.un.echo.sequence = 3 * ttl - 1;

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;

    int err = inet_pton(AF_INET, ip_addr, &(recipient.sin_addr));
    if (err <= 0)
    {
        if (err == 0)
            fprintf(stderr, "\nError: invalid address\n");
        else fprintf(stderr, "\nError: %s\n", strerror(errno));
        return 1;
    }

    for (int i = 0; i < 3; i++)
    {
        icmp_header.un.echo.sequence++;
        icmp_header.checksum = 0;
        icmp_header.checksum = compute_icmp_checksum((u_int16_t*)&icmp_header, sizeof(icmp_header));

        setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

        ssize_t bytes_sent = sendto(sock, &icmp_header, sizeof(icmp_header), 0, (struct sockaddr*)&recipient, sizeof(recipient));
        if (bytes_sent < 0)
        {
            fprintf(stderr, "\nError: sending error: %s\n", strerror(errno));
            return 2;   
        }
    }
    return 0;
}

u_int16_t compute_icmp_checksum(const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

