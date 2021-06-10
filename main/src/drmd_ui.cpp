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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/un.h>

#include "drmd_ui.h"
#include "param.h"
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
static int recvPort = 5001;
static char *sendIp = "192.168.101.160";
static int sendPort = 5002;

//#define RECV_SOCKET "/root/socket/main2drmd.socket"
#define RECV_SOCKET "/root/socket/drmd2main.socket"
static struct sockaddr_un recvSocketAddr;
#define SEND_SOCKET "/root/socket/main2drmd.socket"
static struct sockaddr_un sendSocketAddr;
/*
初始化VVS模块
*/
int DrmdInit()
{
    int ret;
    
    if (flag > 0)
    {
        return 1;
    }
    
    //创建socket
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
    //设置接收端口号、绑定端口号
    ret = bind(socketFd , (struct sockaddr *)&recvSocketAddr , sizeof(recvSocketAddr));
    
    if (ret < 0)
    {
        perror("bind drmd");
        shutdown(socketFd, 2);
        shutdown(socketSendFd, 2);
        return -2;
    }
    
    //设置发送IP、端口号
    bzero(&sendSocketAddr, sizeof(sendSocketAddr));
    sendSocketAddr.sun_family = AF_UNIX;
    strcpy(sendSocketAddr.sun_path, SEND_SOCKET);
    //初始化成功
    printf("DrmdInit is ok\n");
    //启动接收线程
    //ret = pthread_create(&recvThreadFd,NULL,recvPthread,NULL);
    //启动发送线程
    //ret = pthread_create(&sendThreadFd,NULL,sendPthread,NULL);
    flag = 1;
    return 1;
}
//接收数据
int DrmdReadData(unsigned char *buf, int len)
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
int DrmdWriteData(unsigned char *buf, int len)
{
    int len1;
    
    if (flag < 0)
    {
        return -1;
    }
    
    if ((len1 = sendto(socketSendFd, buf, len, 0, (struct sockaddr *)&sendSocketAddr, sizeof(sendSocketAddr))) < 0)
    {
        perror("sendto");
        return -2;
    }
    
    return len1;
}
static unsigned char head = 0xfe;
static unsigned char type = MAIN2DRMD;
static unsigned char cmd = SET_LEAVE;
static unsigned char len = 18;
static unsigned char start = 1;
static unsigned char current = 1;
static unsigned char next = 2;
static unsigned char end = 28;
static unsigned char door = 1;
static unsigned char doorErr = 0;
static unsigned char chgSt = 0;
static unsigned char emeId = 0;
static unsigned char mirror = 0;
static unsigned char skipId[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char sum = 0;
static unsigned char xorSum = 0;
static unsigned char tail = 0xff;
/*
dst:组包后buf
type:数据包类型
cmd:数据包命令字
buf:数据内容
bufLen:数据长度
*/
static int target(unsigned char *dst)
{
    int i, j;
    
    if (NULL == dst)
    {
        perror("target");
        return -1;
    }
    
    i = 0;
    len = 0;
    dst[i++] = head;
    dst[i++] = type;
    dst[i++] = cmd;
    dst[i++] = 0;
    dst[i++] = start;
    dst[i++] = current;
    dst[i++] = next;
    dst[i++] = end;
    dst[i++] = door;
    dst[i++] = doorErr;
    dst[i++] = emeId;
    dst[i++] = mirror;
    dst[i++] = chgSt;
    dst[i++] = skipId[0];
    dst[i++] = skipId[1];
    dst[i++] = skipId[2];
    dst[i++] = skipId[3];
    dst[i++] = skipId[4];
    dst[i++] = skipId[5];
    dst[i++] = skipId[6];
    dst[i++] = skipId[7];
    len = i - 1;
    dst[3] = len;
    
    for (j = 1, sum = 0; j <= len; j++)
    {
        sum += dst[j];
    }
    
    dst[i++] = sum;
    
    for (j = 1, xorSum = 0; j <= len; j++)
    {
        xorSum ^= dst[j];
    }
    
    dst[i++] = xorSum;
    dst[i++] = tail;
    return i;
}
//设置IP
int setCmd(int val)
{
    if (cmd == SET_IP_CONFLICT_ON)
    {
        if (val == SET_IP_CONFLICT_OFF)
            cmd = val;
    }
    else
        cmd = val;
        
    return 0;
}

unsigned char getDisplayCmd(void)
{
    return cmd;
}

int setMirror(int val)
{
    mirror = val;
}

int setStartStation(int val)
{
    start = val;
}
int setCurrentStation(int val)
{
    current = val;
}
int setNextStation(int val)
{
    next = val;
}
int setEndStation(int val)
{
    end = val;
}
int setDoor(int val)
{
    door = val;
}

int setDoorErr(int val)
{
    doorErr = val;
}

int setChgSt(int val)
{
    chgSt = val;
}

int setEmeId(int val)
{
    emeId = val;
}
int setSkipBuf(unsigned char *buf, int len)
{
    int i;
    
    if (8 != len)
    {
        perror("len is error");
        return -1;
    }
    
    for (i = 0; i < len; i++)
    {
        skipId[i] = buf[i];
    }
    
    return 1;
}
int sendDrmdPacket()
{
    int ret, i;
    unsigned char buf[512];
    ret = target(buf);
    
    if (ret > 0)
    {
        DrmdWriteData(buf, ret);
        printf("send to drmd:\n");
        
        for (i = 0; i < ret; i++)
        {
            printf(" %02x", buf[i]);
        }
        
        printf("\n");
        //printf("send:cmd=%d\n",buf[2]);
    }
}

int sendDrmdCyclePacket()
{
    int ret;
    int i, j;
    int sum = 0, len = 0, xorSum = 0;
    unsigned char buf[512];
    //ret = target(buf);
    {
        i = 0;
        buf[i++] = 0xfe;
        buf[i++] = MAIN2DRMD;
        
        if (get_osd_CycleMode() > 0)
            buf[i++] = SET_CYCLE_TEST_ON;
        else
            buf[i++] = SET_CYCLE_TEST_OFF;
            
        buf[i++] = 0;
        buf[i++] = get_osd_CycleTime();
        buf[i++] = get_osd_CycleIndex();
        len = i - 1;
        buf[3] = i - 1;
        
        for (j = 1, sum = 0; j <= len; j++)
        {
            sum += buf[j];
        }
        
        buf[i++] = sum;
        
        for (j = 1, xorSum = 0; j <= len; j++)
        {
            xorSum ^= buf[j];
        }
        
        buf[i++] = xorSum;
        buf[i++] = 0xff;
    }
    
    if (i > 0)
    {
        DrmdWriteData(buf, i);
        printf("send:cycle cmd=%d\n", buf[2]);
    }
}

int getPacket(unsigned char *buf, int length)
{
    int ret = -1;
    
    if (NULL == buf)
    {
        perror("target");
        return -1;
    }
    
    if (length > 4)
    {
        if (buf[0] == head && buf[length - 1] == tail && buf[3] == length - 7)
        {
            int i = 0;
            unsigned char l_sum = 0, l_xorSum = 0, l_len = buf[3];
            
            for (i = 1; i < l_len + 4; i++)
            {
                l_sum += buf[i];
                l_xorSum ^= buf[i];
            }
            
            if (l_sum == buf[length - 3] && l_xorSum == buf[length - 2])
            {
                ret = 1;
            }
            else
            {
                printf("recvsum:%02x sum:%02x\n", buf[length - 3], l_sum);
                printf("recvxor:%02x xor:%02x\n", buf[length - 2], l_xorSum);
            }
        }
    }
    
    return ret;
}