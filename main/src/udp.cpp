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
#include "udp.h"
//����MAC
//��������
//���öԱȶ�
//���ù���ģʽ
//��ʾ������Ϣ

#define classudp
Udp::Udp()
{
    this->flag = -1;
    this->socketFd = -1;
    this->pAddMsg = NULL;
}
//��
int Udp::myopen(char *recvIp, int recvPort, char *sendIp, int sendPort)
{
    int ret;
    
    if (this->flag > 0)
    {
        return -1;
    }
    
    this->recvPort = recvPort;
    this->socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (this->socketFd < 0)
    {
        perror("socket");
        return -1;
    }
    
    bzero(&this->recvAddr, sizeof(struct sockaddr_in));
    this->recvAddr.sin_family = AF_INET;
    
    if (NULL != recvIp)
    {
        this->recvAddr.sin_addr.s_addr = inet_addr(recvIp);
    }
    else
    {
        this->recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    this->recvAddr.sin_port = htons(this->recvPort);
    ret = bind(this->socketFd , (struct sockaddr *)&this->recvAddr , sizeof(struct sockaddr));
    
    if (ret < 0)
    {
        perror("bind");
        return -2;
    }
    
    this->sendPort = sendPort;
    bzero(&this->sendAddr, sizeof(struct sockaddr_in));
    this->sendAddr.sin_family = AF_INET;
    
    if (NULL != sendIp)
    {
        this->sendAddr.sin_addr.s_addr = inet_addr(sendIp);
    }
    else
    {
        this->sendAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    this->sendAddr.sin_port = htons(this->sendPort);
    this->flag = 1;
    return 1;
}
//��������
int Udp::readData(unsigned char *buf, int len)
{
    int len1;
    
    if (this->flag < 0)
    {
        return -1;
    }
    
    if ((len1 = recvfrom(this->socketFd, buf, len, 0, NULL, NULL)) < 0)
    {
        perror("recvfrom");
        return -2;
    }
    
    return len1;
}
//��������
int Udp::writeData(unsigned char *buf, int len)
{
    int len1;
    
    if (this->flag < 0)
    {
        return -1;
    }
    
    if ((len1 = sendto(this->socketFd, buf, len, 0, (struct sockaddr *)&this->sendAddr, sizeof(struct sockaddr))) < 0)
    {
        perror("sendto");
        return -2;
    }
    
    return len1;
}
//�ر�
int Udp::myclose()
{
    if (this->flag > 0 && this->socketFd > 0)
    {
        close(this->socketFd);
        this->flag = -1;
        this->socketFd = -1;
    }
    
    return 1;
}
int Udp::addMsg(unsigned char *buf, int len) //ִ�лص�����
{
    if (NULL == this->pAddMsg)
    {
        return -1;
    }
    
    this->pAddMsg(buf, len);
    return 1;
}
int Udp::initMsg(int (*fun)(unsigned char *, int)) //���ûص�����
{
    this->pAddMsg = fun;
    return 1;
}