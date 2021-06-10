#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "myudpforsendstatus.h"

MyUdpforSendStatus::MyUdpforSendStatus(QObject *parent) :
QObject(parent)
{
    memset(mc_addr,0,sizeof(mc_addr));
    mc_port=0;
}


MyUdpforSendStatus::~MyUdpforSendStatus()
{
	
}

void MyUdpforSendStatus::initSocket(char* mcaddr,unsigned int mcport)
{
	unsigned long  FSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	unsigned char TTL = 16;
	bool Loop = true;
	setsockopt(FSocket, IPPROTO_IP, IP_MULTICAST_TTL, (const char *) &TTL,sizeof(unsigned char));
	setsockopt(FSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (const char *) &Loop,sizeof(unsigned char));
	udpSocket = new QUdpSocket();
	udpSocket->setSocketDescriptor(FSocket);
	strcpy(mc_addr,mcaddr);
	mc_port=mcport;
}

void MyUdpforSendStatus::sendPakages(WORD nstatus)
{
#if 1
	if(strlen(mc_addr)>0 && mc_port!=0)
	{ // 发送状态。按照协议格式
        BYTE buf[40];
        memset(buf,0,sizeof(buf));
        buf[0]=drmdDevID;
        buf[1]=nstatus;
        buf[2]=50;
        buf[3]=1;
        buf[4]=0;
        buf[5]=0;
        buf[6]=0;
        buf[7]=0;
		
        int nsentcnt=udpSocket->writeDatagram((char*)buf, 8, QHostAddress(mc_addr),mc_port);
		//qDebug()<<QString("send status len=%1").arg(nsentcnt);
		nsentcnt=0;
    }
#endif
}
