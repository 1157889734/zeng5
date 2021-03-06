
#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include<errno.h>
#include <unistd.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "pisc.h"

static int Socket_send_Handler_data = 0;//udp???Ͳ???????
static int Socket_recv_Handler_data = 0;//udp???ղ???????

int udp_socket_handler_great(int *socket_handler, short bind_port, int time_out_us)
{
    int n;
    int socklen = 0;
    struct timeval timeout;
    struct sockaddr_in localaddr_send;
    socklen = sizeof(struct sockaddr_in);
    
    if ((*socket_handler = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("pisc init upd socket creating err\n");
        return -1;
    }
    
    if (time_out_us)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = time_out_us;
        
        if (setsockopt(*socket_handler, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) < 0)
        {
            printf("pisc init upd setsockopt  err\n");
            return -1;
        }
    }
    
    n = 1;
    
    if (setsockopt(*socket_handler, SOL_SOCKET, SO_BROADCAST, &n, sizeof(n)) < 0)
    {
        printf("pisc init upd setsockopt  err\n");
        return -1;
    }
    
    memset(&localaddr_send, 0, socklen);
    localaddr_send.sin_family = AF_INET;
    localaddr_send.sin_port = htons(bind_port);
    
    if (bind(*socket_handler, (struct sockaddr *)&localaddr_send, socklen) < 0)
    {
        printf("pisc init upd bind err!\n");
        return -1;
    }
    
    return 0;
}

// udp????
void udp_socket_send(int socket_handler, char *remote_ip, short remote_port, unsigned char *param, int length)
{
    int socklen;
    struct sockaddr_in remoteaddr;
    socklen = sizeof(struct sockaddr_in);
    memset(&remoteaddr, 0, socklen);
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(remote_port);
    
    if (inet_pton(AF_INET, remote_ip, &remoteaddr.sin_addr) <= 0)
    {
        return;
    }
    
    sendto(socket_handler, param, length, 0, (struct sockaddr *)&remoteaddr, socklen);
    return;
}


/*
??ʼ??VVSģ??
*/
int PiscInit()
{
    int ret;
    ret = -1;
    
    while (ret < 0)
    {
        ret = udp_socket_handler_great(&Socket_send_Handler_data, 50151, 0);
        printf("PiscInit udp ret =%d\n", ret);
    }
    
    ret = -1;
    
    while (ret < 0)
    {
        ret = udp_socket_handler_great(&Socket_recv_Handler_data, 50152, 0);
        printf("PiscInit udp ret =%d\n", ret);
    }
    
    printf("PiscInit ok\n");
}

//????????
int PiscReadData(unsigned char *buf, int len)
{
    int len1;
    
    if ((len1 = recvfrom(Socket_recv_Handler_data, buf, len, 0, NULL, NULL)) < 0)
    {
        perror("recvfrom");
        return -2;
    }
    
    return len1;
}
//????????
int PiscWriteData(unsigned char *buf, int len)
{
    udp_socket_send(Socket_send_Handler_data, "192.168.255.255", 50152, buf, (len));
    return 0;
#if 0
    
    if ((len1 = sendto(Socket_send_Handler_data, buf, len, 0, (struct sockaddr *)&sendAddr, sizeof(struct sockaddr))) < 0)
    {
        perror("sendto");
        return -2;
    }
    
    return len1;
#endif
}
