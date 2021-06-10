#ifndef __UDP_H__
#define __UDP_H__
#define __cplusplus 1
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __cplusplus
}
#endif

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
class Udp
{
public:
     Udp();
     int myopen(char *recvIp,int recvPort,char *sendIp,int sendPort);
     int readData(unsigned char *buf,int len);//接收数据
     int writeData(unsigned char *buf,int len);//发送数据
     int myclose();
     int addMsg(unsigned char *buf,int len);//执行回调函数
     int initMsg(int (*fun)(unsigned char*,int));//设置回调函数
private:
    int flag;
    pthread_t recvThreadFd;
    pthread_t sendThreadFd;
    int socketFd;
    int recvPort;
    int sendPort;
    struct sockaddr_in recvAddr;
    int recvAddrLen;
    struct sockaddr_in sendAddr;
    int sendAddrLen;
    int (*pAddMsg)(unsigned char*,int);
};
#endif