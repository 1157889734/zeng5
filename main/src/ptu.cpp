/*
osdģ��ͨ�Ŵ���
ͨ�ŷ�ʽUDP
��Ҫ��������/��ѯ
IP
MAC
����
�Աȶ�
����ģʽ
������Ϣ
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

#include "ptu.h"
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

static char *recvIp = "192.168.255.255";
static int recvPort = 8889;
static char *sendIp = "225.1.1.160";
static int sendPort = 8888;
static struct ip_mreq mreq;
static struct hostent *group;
static struct in_addr ia;
static struct sockaddr_in peeraddr;
/*
��ʼ��VVSģ��
*/
int PtuUdpInit()
{
    int ret;
    
    if (flag > 0)
    {
        return 1;
    }
    
    //����socket
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (socketFd < 0)
    {
        perror("socket");
        return -1;
    }
    
    /* ����Ҫ�����鲥�ĵ�ַ */
    bzero(&mreq, sizeof(struct ip_mreq));
    
    if ((group = gethostbyname(recvIp)) == (struct hostent *) 0)
    {
        perror("gethostbyname");
    }
    
    bcopy((void *) group->h_addr, (void *) &ia, group->h_length);
    /* �������ַ */
    bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));
    /* ���÷����鲥��Ϣ��Դ�����ĵ�ַ��Ϣ */
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    if (setsockopt(socketFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1)
    {
        perror("setsockopt");
    }
    
    memset(&peeraddr, 0, sizeof(struct sockaddr_in));
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_port = htons(recvPort);
    
    if (inet_pton(AF_INET, recvIp, &peeraddr.sin_addr) <= 0)
    {
        printf("Wrong dest IP address!/n");
    }
    
    if (bind(socketFd, (struct sockaddr *) &peeraddr,  sizeof(struct sockaddr_in)) == -1)
    {
        printf("Bind error/n");
    }
    
    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = inet_addr(sendIp);
    sendAddr.sin_port = htons(sendPort);
    //��ʼ���ɹ�
    printf("PtuUdpInit is ok\n");
    //���������߳�
    //ret = pthread_create(&recvThreadFd,NULL,recvPthread,NULL);
    //���������߳�
    //ret = pthread_create(&sendThreadFd,NULL,sendPthread,NULL);
    flag = 1;
    return 1;
}
//��������
int PtuUdpReadData(unsigned char *buf, int len)
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
//��������
int PtuUdpWriteData(unsigned char *buf, int len)
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