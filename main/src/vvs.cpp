/*
osd模块通信处理
通信方式UDP
主要功能设置/查询
IP
MAC
亮度
对比度
管理模式
参数信息
*/
#include <stdio.h>
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

#include "vvs.h"
//typedef  int(*)(unsigned char*,int)  MsgFun
static int flag = -1;
static pthread_t recvThreadFd;
static pthread_t sendThreadFd;
static int socketFd = -1;
static struct sockaddr_in recvAddr;
static int recvAddrLen;
static struct sockaddr_in sendAddr;
static int sendAddrLen;
//MsgFun pAddMsg = NULL;
static int (*pAddMsg)(unsigned char *, int);

static char *recvIp = "225.1.1.41";
static int recvPort = 4001;
static char *sendIp = "225.1.1.45";
static int sendPort = 4005;
static struct ip_mreq mreq;
static struct hostent *group;
static struct in_addr ia;
static struct sockaddr_in peeraddr;
/*
初始化VVS模块
*/
int VvsInit()
{
    int ret;
    
    if (flag > 0)
    {
        return 1;
    }
    
    //创建socket
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (socketFd < 0)
    {
        perror("socket");
        return -1;
    }
    
    /* 设置要加入组播的地址 */
    bzero(&mreq, sizeof(struct ip_mreq));
    
    if ((group = gethostbyname(recvIp)) == (struct hostent *) 0)
    {
        perror("gethostbyname");
        shutdown(socketFd, 2);
        return -1;
    }
    
    bcopy((void *) group->h_addr, (void *) &ia, group->h_length);
    /* 设置组地址 */
    bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));
    /* 设置发送组播消息的源主机的地址信息 */
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    if (setsockopt(socketFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1)
    {
        perror("VvsInit setsockopt");
        shutdown(socketFd, 2);
        return -1;
    }
    
    memset(&peeraddr, 0, sizeof(struct sockaddr_in));
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(4001);
    
    if (inet_pton(AF_INET, recvIp, &peeraddr.sin_addr) <= 0)
    {
        printf("Wrong dest IP address!/n");
        shutdown(socketFd, 2);
        return -1;
    }
    
    if (bind(socketFd, (struct sockaddr *) &peeraddr,  sizeof(struct sockaddr_in)) == -1)
    {
        printf("Bind error/n");
        shutdown(socketFd, 2);
        return -1;
    }
    
    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = inet_addr(sendIp);
    sendAddr.sin_port = htons(sendPort);
    //初始化成功
    printf("VvsInit is ok\n");
    //启动接收线程
    //ret = pthread_create(&recvThreadFd,NULL,recvPthread,NULL);
    //启动发送线程
    //ret = pthread_create(&sendThreadFd,NULL,sendPthread,NULL);
    flag = 1;
    return 1;
}
//接收数据
int VvsReadData(unsigned char *buf, int len)
{
    int len1;
    
    if (flag < 0)
    {
        return -1;
    }
    
    if ((len1 = recvfrom(socketFd, buf, len, 0, NULL, NULL)) < 0)
    {
        perror("recvfrom");
        return -2;
    }
    
    return len1;
}
//发送数据
int VvsWriteData(unsigned char *buf, int len)
{
    int len1;
    
    if (flag < 0)
    {
        return -1;
    }
    
    if ((len1 = sendto(socketFd, buf, len, 0, (struct sockaddr *)&sendAddr, sizeof(struct sockaddr))) < 0)
    {
        perror("sendto");
        return -2;
    }
    
    return len1;
}
