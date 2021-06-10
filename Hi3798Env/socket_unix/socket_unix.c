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
#include "socket_unix.h"

#include "uart485/uart485.h"
#include "hi_osd/hi_osd.h"


static int flag = -1;
static int socketFd = -1;
static int socketSendFd = -1;

//#define RECV_SOCKET "/root/socket/main2osd.socket"
#define RECV_SOCKET "/root/socket/main2osd.socket"
static struct sockaddr_un recvSocketAddr;

#define SEND_SOCKET "/root/socket/osd2main.socket"
static struct sockaddr_un sendSocketAddr;
/*
��ʼ��VVSģ��
*/
int SocketUnixInit()
{
    int ret;
    if(flag > 0)
    {
    	return 1;
    }
    //����socket socketSendFd
    socketFd=socket(PF_LOCAL,SOCK_DGRAM,0);    
    //socketFd = socket(PF_UNIX,SOCK_DGRAM,0);
    if(socketFd<0)
    {
        perror("socket"); 
        shutdown(socketFd,2);
        return -1;
    }

    socketSendFd=socket(PF_LOCAL,SOCK_DGRAM,0);
    if(socketSendFd<0)
    {
        perror("socketSendFd"); 
        shutdown(socketSendFd,2);
        return -1;
    }
    
    unlink(RECV_SOCKET);
    //unlink(SEND_SOCKET);	
    bzero(&recvSocketAddr,sizeof(recvSocketAddr));   
    recvSocketAddr.sun_family=AF_LOCAL;  
    strcpy(recvSocketAddr.sun_path,RECV_SOCKET);
	
    //���ý��ն˿ںš��󶨶˿ں�
    ret = bind(socketFd ,(struct sockaddr *)&recvSocketAddr ,sizeof(recvSocketAddr));
    if(ret<0)
    {
        perror("bind osd"); 
        shutdown(socketFd,2);
        shutdown(socketSendFd,2);
        return -2;
    }
    //���÷���IP���˿ں�
    bzero(&sendSocketAddr,sizeof(sendSocketAddr));   
    sendSocketAddr.sun_family=AF_UNIX;  
    strcpy(sendSocketAddr.sun_path,SEND_SOCKET);
    //��ʼ���ɹ�
    printf("SocketUnixInit is ok\n");
    
    //���������߳�
    //ret = pthread_create(&recvThreadFd,NULL,recvPthread,NULL);
    //���������߳�
    //ret = pthread_create(&sendThreadFd,NULL,sendPthread,NULL);
    
    flag = 1;
    return 1;
}
//��������
int SocketUnixReadData(unsigned char *buf,int len)
{
	int len1,i;
	if(flag < 0)
	{
		return -1;
	}
	if((len1 = recvfrom(socketFd, buf, len, 0, NULL,NULL)) < 0)    
        {    
            perror("recvfrom");    
            return -2;    
        }

        //printf("OSDReadMainData:\n");
        // for(i=0;i<len1;i++){
        //    printf("%02x ",buf[i]);
        //}
        //printf("\n");
        
	return len1;
}
//��������
int SocketUnixWriteData(unsigned char *buf,int len)
{
	int len1;
	if(flag < 0)
	{
		return -1;
	}
	if((len1 = sendto(socketSendFd, buf, len, 0, (struct sockaddr *)&sendSocketAddr,sizeof(sendSocketAddr))) < 0)    
        {    
            perror("sendto");    
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
static int target(unsigned char *dst,int type,int cmd,unsigned char *buf,int bufLen)
{
	int i;
	unsigned char sum;
	unsigned char xorSum;
	if(NULL==dst)
	{
		perror("target");
		return -1;
	}
	dst[0] = 0xFE;
	dst[1] = type;
	dst[2] = cmd;
	dst[3] = bufLen;
	for(i=0;i<bufLen;i++)
	{
		dst[4+i] = buf[i];
	}
	
	for(i=1,sum=0;i<bufLen+4;i++)
	{
		sum += dst[i];
	}
	
	for(i=1,xorSum=0;i<bufLen+4;i++)
	{
		xorSum ^= dst[i];
	}
	dst[bufLen+4] = sum;
	dst[bufLen+5] = xorSum;
	dst[bufLen+6] = 0xFF;
	return bufLen+7;
}

//����IP
int setIp(unsigned char ip1,unsigned char ip2,unsigned char ip3,unsigned char ip4)
{
	int ret;
	unsigned char buf[512];
	unsigned char dat[128];
	dat[0] = ip1;
	dat[1] = ip2;
	dat[2] = ip3;
	dat[3] = ip4;
	ret = target(buf,OSD2MAIN,SET_IP,dat,4);
	if(ret>0)
	{
		SocketUnixWriteData(buf,ret);
		return ret;
	}
	return 0;
}
//����mac
int setMac(unsigned char mac1,unsigned char mac2,unsigned char mac3,unsigned char mac4,unsigned char mac5,unsigned char mac6)
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
	ret = target(buf,OSD2MAIN,SET_MAC,dat,6);
	if(ret>0)
	{
		SocketUnixWriteData(buf,ret);
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
	ret = target(buf,OSD2MAIN,SET_LIGHT,dat,1);
	if(ret>0)
	{
		SocketUnixWriteData(buf,ret);
		return ret;
	}
	return 0;
}

//����pwm
int setPwm(unsigned char pwm)
{
	int ret;
	unsigned char buf[512];
	unsigned char dat[128];
	dat[0] = pwm;
	ret = target(buf,OSD2MAIN,SET_PWM,dat,1);
	if(ret>0)
	{
		SocketUnixWriteData(buf,ret);
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
	ret = target(buf,OSD2MAIN,SET_CONTRAST,dat,1);
	if(ret>0)
	{
		SocketUnixWriteData(buf,ret);
		return ret;
	}
	return 0;
}
//���öԱȶ�
int setLcd(unsigned char on_off)
{
	int ret;
	unsigned char buf[512];
	unsigned char dat[128];
	dat[0] = on_off;
	ret = target(buf,OSD2MAIN,SET_LCD,dat,1);
	if(ret>0)
	{
		SocketUnixWriteData(buf,ret);
		return ret;
	}
	return 0;
}

//����״̬
void sendStatus(void)
{
    int ret;
    unsigned char buf[100];
    static unsigned char dat[20];
    unsigned char cycle[3];

    cycle[0]= get_cycle_mode();
    cycle[1]= get_cycle_time();
    cycle[2]= get_cycle_index();
    
    if(dat[6]>0)
    {
        if((dat[3] == cycle[0])&&(dat[4] == cycle[1])&&(dat[5] == cycle[2]))
            set_cycle_chg(0);
    }
    
    dat[0] = VERSION_MAX;
    dat[1] = VERSION_MIN;
    dat[2] = get_rs485_com();
    dat[3] = get_cycle_mode();
    dat[4] = get_cycle_time();
    dat[5] = get_cycle_index();
    dat[6] = get_cycle_chg();

    //printf("get_rs485_com:%d\n",dat[2] );
    ret = target(buf,OSD2MAIN,SEND_STATUS,dat,7);
    if(ret>0)
    {
        SocketUnixWriteData(buf,ret);
    }
}

// ����485
void send485(unsigned char* pbuf,unsigned char len)
{
    int ret,i;
    unsigned char buf[512];
    ret = target(buf,OSD2MAIN,SET_485,pbuf,len);
    if(ret>0)
    {
        SocketUnixWriteData(buf,ret);
    }

    //printf("------send485 len: %d-----",ret);
    //for(i=0;i<ret;i++)
    //{
     //   printf("%0.2x ",buf[i]);
    //}
    //printf("\n");
}

