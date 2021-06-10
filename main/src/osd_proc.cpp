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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/un.h>

#include "osd_proc.h"
//typedef  int(*)(unsigned char*,int)  MsgFun
static int flag = -1;
static pthread_t recvThreadFd;
static pthread_t sendThreadFd;
static int socketFd = -1;
static int socketSendFd = -1;
static struct sockaddr_in recvAddr;
static int recvAddrLen;
static struct sockaddr_in sendAddr;
static int sendAddrLen;
//MsgFun pAddMsg = NULL;
static int (*pAddMsg)(unsigned char *, int);

static char *recvIp = "192.168.101.99";
static int recvPort = 5003;
static char *sendIp = "192.168.101.160";
static int sendPort = 5004;

//#define RECV_SOCKET "/root/socket/main2osd.socket"
#define RECV_SOCKET "/root/socket/osd2main.socket"
static struct sockaddr_un recvSocketAddr;
#define SEND_SOCKET "/root/socket/main2osd.socket"
static struct sockaddr_un sendSocketAddr;
/*
��ʼ��VVSģ��
*/
int OsdInit()
{
    int ret;
    
    if (flag > 0)
    {
        return 1;
    }
    
    //����socket socketSendFd
    socketFd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    
    //socketFd = socket(PF_UNIX,SOCK_DGRAM,0);
    if (socketFd < 0)
    {
        perror("socket");
        shutdown(socketFd, 2);
        return -1;
    }
    
    socketSendFd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    
    if (socketSendFd < 0)
    {
        perror("socketSendFd");
        shutdown(socketSendFd, 2);
        return -1;
    }
    
    unlink(RECV_SOCKET);
    //unlink(SEND_SOCKET);
    bzero(&recvSocketAddr, sizeof(recvSocketAddr));
    recvSocketAddr.sun_family = AF_LOCAL;
    strcpy(recvSocketAddr.sun_path, RECV_SOCKET);
    //���ý��ն˿ںš��󶨶˿ں�
    ret = bind(socketFd , (struct sockaddr *)&recvSocketAddr , sizeof(recvSocketAddr));
    
    if (ret < 0)
    {
        perror("bind osd");
        shutdown(socketFd, 2);
        shutdown(socketSendFd, 2);
        return -2;
    }
    
    //���÷���IP���˿ں�
    bzero(&sendSocketAddr, sizeof(sendSocketAddr));
    sendSocketAddr.sun_family = AF_UNIX;
    strcpy(sendSocketAddr.sun_path, SEND_SOCKET);
    //��ʼ���ɹ�
    printf("OsdInit is ok\n");
    //���������߳�
    //ret = pthread_create(&recvThreadFd,NULL,recvPthread,NULL);
    //���������߳�
    //ret = pthread_create(&sendThreadFd,NULL,sendPthread,NULL);
    flag = 1;
    return 1;
}
//��������
int OsdReadData(unsigned char *buf, int len)
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
int OsdWriteData(unsigned char *buf, int len)
{
    int len1;
    
    if (flag < 0)
    {
        return -1;
    }
    
    if ((len1 = sendto(socketSendFd, buf, len, 0, (struct sockaddr *)&sendSocketAddr, sizeof(sendSocketAddr))) < 0)
    {
        perror("osd sendto");
        return -2;
    }
    
    return len1;
}
/*
dst:�����buf
type:���ݰ�����
cmd:���ݰ�������
buf:��������
bufLen:���ݳ���
*/
static int target(unsigned char *dst, int type, int cmd, unsigned char *buf, int bufLen)
{
    int i;
    unsigned char sum;
    unsigned char xorSum;
    
    if (NULL == dst)
    {
        perror("target");
        return -1;
    }
    
    dst[0] = 0xFE;
    dst[1] = type;
    dst[2] = cmd;
    dst[3] = bufLen;
    
    for (i = 0; i < bufLen; i++)
    {
        dst[4 + i] = buf[i];
    }
    
    for (i = 1, sum = 0; i < bufLen + 4; i++)
    {
        sum += dst[i];
    }
    
    for (i = 1, xorSum = 0; i < bufLen + 4; i++)
    {
        xorSum ^= dst[i];
    }
    
    dst[bufLen + 4] = sum;
    dst[bufLen + 5] = xorSum;
    dst[bufLen + 6] = 0xFF;
    return bufLen + 7;
}

//����IP
int setIp(unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = ip1;
    dat[1] = ip2;
    dat[2] = ip3;
    dat[3] = ip4;
    ret = target(buf, MAIN2OSD, SET_IP_OSD, dat, 4);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}
//����mac
int setMac(unsigned char mac1, unsigned char mac2, unsigned char mac3, unsigned char mac4, unsigned char mac5, unsigned char mac6)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = mac1;
    dat[1] = mac2;
    dat[2] = mac3;
    dat[3] = mac4;
    dat[4] = mac5;
    dat[5] = mac6;
    ret = target(buf, MAIN2OSD, SET_MAC, dat, 6);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}
//��������
int setLight(unsigned char light)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = light;
    ret = target(buf, MAIN2OSD, SET_LIGHT, dat, 1);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}

//��������
int setPwm(unsigned char pwm)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = pwm;
    ret = target(buf, MAIN2OSD, SET_PWM, dat, 1);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}

//���öԱȶ�
int setContrast(unsigned char contrast)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = contrast;
    ret = target(buf, MAIN2OSD, SET_CONTRAST, dat, 1);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}
//���ù���
int setLcd(unsigned char on_off)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = on_off;
    ret = target(buf, MAIN2OSD, SET_LCD, dat, 1);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}

//����485��������
int set485(unsigned char beg485)
{
    int ret;
    unsigned char buf[512];
    unsigned char dat[128];
    dat[0] = beg485;
    ret = target(buf, MAIN2OSD, SET_485, dat, 1);
    
    if (ret > 0)
    {
        OsdWriteData(buf, ret);
        return ret;
    }
    
    return 0;
}
