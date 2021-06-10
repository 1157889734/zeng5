#include "ptu.h"
#include<QDebug>
ptu::ptu(QThread *parent) :
    QThread(parent)
{
    qDebug()<<"ptu start";

    tcpSocket1 = new QTcpSocket();
    //tcpSocket1->bind(tcplPort1);
    connect(tcpSocket1,SIGNAL(connected()),this,SLOT(connected()));
    connect(tcpSocket1,SIGNAL(readyRead()),this,SLOT(tcpreadyRead()));
    connect(tcpSocket1,SIGNAL(disconnected()), this, SLOT(disconnected()));

    udpSocket = new QUdpSocket();
    QHostAddress HostAddress = QHostAddress("255.255.255.255");
    udpSocket->bind(HostAddress,portRx);
    udpSocket->open(QIODevice::ReadWrite);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(udpreadyRead()));
    connect(this,SIGNAL(timeout1s()),this,SLOT(TimeoutSLOT()));
    //init();
    qDebug()<<"ptu end";
    //threadFunc = &TimeoutSLOT;
    this->start();
}
void ptu::init()
{
    qDebug()<<"connectToHost";
    udpOnConnect = false;
    tcpOnTime = 0;
    tcpSocket1->disconnectFromHost();
    tcpSocket1->connectToHost(tcpServerIp1,tcpServerPort1,QTcpSocket::ReadWrite);

}
void ptu::connected()
{
    qDebug()<<"connected";
    tcpOnConnect = true;
    udpOnConnect = false;
}
void ptu::disconnected()
{
    qDebug()<<"disconnected";
    tcpOnConnect = false;
}
void ptu::tcpreadyRead()
{
    QByteArray arr=tcpSocket1->readAll();
    if(arr.length()>0)
    {
        tcpOnTime = 0;
        ptuTcpRecvProc(arr);
        //���յ������������ݻظ���ͬ����
        //qDebug()<<arr;
        //tcpSocket1->write((char *)arr.data(),(quint64)arr.length());
    }
}
//�ֽڵ���
void convert(char *buf,int len)
{
    char temp;
    for(int i=0;i<len/2;i++)
    {
        temp = buf[i];
        buf[i] = buf[len-1-i];
        buf[len-1-i] = temp;
    }
}

#define TYPE_BIG 1
#define TYPE_LITTLE 2
unsigned int U8toU32(char ll,char lh,char hl,char hh)
{
    unsigned int ret = 0;
    ret &= (unsigned int)ll;
    ret &= (((unsigned int)lh)<<8);
    ret &= (((unsigned int)hl)<<16);
    ret &= (((unsigned int)hh)<<24);
    return ret;
}
unsigned short U8toU16(char l,char h)
{
    unsigned short ret = 0;
    ret &= (unsigned short)l;
    ret &= (((unsigned short)h)<<8);
    return ret;
}
void U64toU8(char *buf,quint64 val,int type)
{
    if(TYPE_BIG==type)
    {
        for(int i=0;i<8;i++)
        {
            buf[i] = (val>>((7-i)*8))&0xff;
        }
    }
    else if(TYPE_LITTLE==type)
    {
        for(int i=0;i<8;i++)
        {
            buf[i] = (val>>((i)*8))&0xff;
        }
    }
}
#include <sys/vfs.h>
int GetStorageInfo(char * MountPoint,  //SD�����һ������
                                 quint64 *Capacity,  //  ��Ҫ��ȡ�Ŀռ��С
                                 int type) //��ȡʲô���͵Ŀռ�
{
  struct statfs statFS; //ϵͳstat�Ľṹ��
  quint64 usedBytes = 0;
  quint64 freeBytes = 0;
  quint64 totalBytes = 0;
  quint64 endSpace = 0;

  if (statfs(MountPoint, &statFS) == -1){   //��ȡ������״̬
  printf("statfs failed for path->[%s]\n", MountPoint);
  return(-1);
  }

  totalBytes = (quint64)statFS.f_blocks * (quint64)statFS.f_frsize; //��ϸ�ķ����������� ���ֽ�Ϊ��λ
  freeBytes = (quint64)statFS.f_bfree * (quint64)statFS.f_frsize; //��ϸ��ʣ��ռ����������ֽ�Ϊ��λ
  usedBytes = (quint64)(totalBytes - freeBytes); //��ϸ��ʹ�ÿռ����������ֽ�Ϊ��λ

  switch( type )
  {
  case 1:
  endSpace = totalBytes; //��BΪ��λ��������
  break;

  case 2:
  endSpace = usedBytes; //��BΪ��λ��ʹ�ÿռ�
  break;

  case 3:
  endSpace = freeBytes; //��BΪ��λ��ʣ��ռ�
  break;

  default:
  return ( -1 );
  }
  *Capacity = endSpace; //�������˵�˰�
  return 0;
}
#define GET_STATE 0x0100  //      ��ȡ�豸״̬
#define GET_SD_STATE 0x0200 //    ��ȡSD��״̬
#define GET_VERSION 0x0300  //    ��ȡ�汾
#define DEV_CTRL 0x0400   //      �豸����
#define GET_CTRL_TYPE 0x0500 //   ��ȡ��������
#define SET_CTRL_TYPE 0x0600  //  ���ÿ�������
#define GET_BACK_LIGHT 0x0700  // ��ȡ��������
#define SET_BACK_LIGHT 0x0800  // ���ñ�������
#define GET_CONTRAST 0x0900   // ��ȡ�Աȶ�
#define SET_CONTRAST 0x0A00  // ���öԱȶ�
#define SET_IP 0x0B00   //  ����IP
#define GET_MAC 0x0C00  //  ��ȡMAC��ַ
#define HEARTBEAT 0x9900  //����

void ptu::ptuTcpRecvProc(QByteArray packet)
{
    unsigned char temp = 0;
    unsigned char const head = 0x7e;
    unsigned char const end = 0xfe;
    unsigned char recvEnd = 0xfe;
    unsigned char checkflag = 1;

    unsigned char check = 0;
    unsigned char recvCheck = 0;
    int packetLen = 0;
    int dataLen = 0;
    unsigned short cmd = 0;
    int const len =  packet.length();
    struct ptuTcpRecvPacket Ppacket;
    for(int i=0;i+13<=len;)
    {
        temp = (unsigned char)packet.at(i);
        if(head==temp)
        {
            cmd = 0;
            cmd += (short)packet.at(i+1);
            cmd += (((short)packet.at(i+2))<<8);
            convert((char *)&cmd,2);
            packetLen = 0;
            packetLen += ((int)packet.at(i+3));
            packetLen += (((int)packet.at(i+4))<<8);
            packetLen += (((int)packet.at(i+5))<<16);
            packetLen += (((int)packet.at(i+6))<<24);
            convert((char *)&packetLen,4);
            dataLen = 0;
            dataLen += ((int)packet.at(i+7));
            dataLen += (((int)packet.at(i+8))<<8);
            dataLen += (((int)packet.at(i+9))<<16);
            dataLen += (((int)packet.at(i+10))<<24);
            convert((char *)&dataLen,4);
            if(packetLen>=13 && (packetLen+i)<=len && dataLen+13==packetLen)
            {
                check = 0;
                for(int j=1;j<packetLen-2;j++)
                {
                    checkflag = (j%2==1)?1:3;
                    check += checkflag*(unsigned char)packet.at(i+j);
                }
                check /= 11;
                recvCheck = (unsigned char)packet.at(i+packetLen-2);
                recvEnd = (unsigned char)packet.at(i+packetLen-1);
                if(check==recvCheck && end==recvEnd)
                {
                    Ppacket.cmd = cmd;
                    Ppacket.packetLen = packetLen;
                    Ppacket.dataLen = dataLen;
                    for(int j=0;j<dataLen;j++)
                    {
                        Ppacket.buff[j] = (unsigned char)packet.at(i+11+j);
                    }
                    i += packetLen;
                    ptuTcpPacketProc(Ppacket);
                    continue;
                }
                else
                {
                    qDebug()<<"error:recvEnd="<<recvEnd;
                    qDebug()<<"error:recvCheck="<<recvCheck;
                    qDebug()<<"error:check"<<check;
                }
            }
        }
        i++;
    }
}

void ptu::ptuTcpPacketProc(const struct ptuTcpRecvPacket Ppacket)
{
    unsigned short cmd;
    cmd = Ppacket.cmd;
    if(GET_STATE==cmd)
    {
        qDebug()<<"GET_STATE";
        char onState[1];
        onState[0] = 1;
        ptuTcpSendData(cmd,onState,1);
    }
    else if(GET_SD_STATE==cmd)
    {
        qDebug()<<"GET_SD_STATE";
        quint64 usedBytes = 0;
        quint64 freeBytes = 0;
        quint64 totalBytes = 0;
        char *path = "/";
        quint64 Capacity = 0;
        int type = 1;

        Capacity = 0;
        type = 1;
        GetStorageInfo(path,&Capacity,type);
        totalBytes = Capacity;
        param.totalBytes = totalBytes;

        Capacity = 0;
        type = 2;
        GetStorageInfo(path,&Capacity,type);
        usedBytes = Capacity;
        param.usedBytes = usedBytes;

        Capacity = 0;
        type = 3;
        GetStorageInfo(path,&Capacity,type);
        freeBytes = Capacity;
        param.freeBytes = freeBytes;

        quint64 buff[2];
        buff[0] = totalBytes; //SD�����ڴ�
        convert((char *)&buff[0],8);
        buff[1] = freeBytes;//SD��ʣ���ڴ�
        convert((char *)&buff[1],8);
        ptuTcpSendData(cmd,(char *)buff,16);
    }
    else if(GET_VERSION==cmd)
    {
        qDebug()<<"GET_VERSION";
        char devType = 0;
        devType = Ppacket.buff[0];;
        if(1==devType)
        {
            char buff[5];
            buff[0] = param.devType;
            buff[1] = param.version[1];
            buff[2] = param.version[2];
            buff[3] = param.version[3];
            buff[4] = param.version[4];
            ptuTcpSendData(cmd,buff,5);
        }
    }
    else if(DEV_CTRL==cmd)
    {
        qDebug()<<"DEV_CTRL";
        param.devCtrlCmd = Ppacket.buff[0];
        char recvOk = 0;
        ptuTcpSendData(cmd,&recvOk,1);
    }
    else if(GET_CTRL_TYPE==cmd)
    {
        qDebug()<<"GET_CTRL_TYPE";
        ptuTcpSendData(cmd,&param.ctrlType,1);
    }
    else if(SET_CTRL_TYPE==cmd)
    {
        qDebug()<<"SET_CTRL_TYPE";
        param.ctrlType = Ppacket.buff[0];
        char recvOk = 0;
        ptuTcpSendData(cmd,&recvOk,1);
    }
    else if(GET_BACK_LIGHT==cmd)
    {
        qDebug()<<"GET_BACK_LIGHT";
        ptuTcpSendData(cmd,&param.backlight,1);
    }
    else if(SET_BACK_LIGHT==cmd)
    {
        qDebug()<<"SET_BACK_LIGHT";
        param.backlight = Ppacket.buff[0];
        char recvOk = 0;
        ptuTcpSendData(cmd,&recvOk,1);
    }
    else if(GET_CONTRAST==cmd)
    {
        qDebug()<<"GET_CONTRAST";
        ptuTcpSendData(cmd,&param.contrast,1);

    }
    else if(SET_CONTRAST==cmd)
    {
        qDebug()<<"SET_CONTRAST";
        param.contrast = Ppacket.buff[0];
        char recvOk = 0;
        ptuTcpSendData(cmd,&recvOk,1);
    }
    else if(SET_IP==cmd)
    {
        qDebug()<<"SET_IP";
        for(int i=0;i<4;i++)
        {
            param.ipAddr[i] = Ppacket.buff[i];
        }
        char recvOk = 0;
        ptuTcpSendData(cmd,&recvOk,1);

    }
    else if(GET_MAC==cmd)
    {
        qDebug()<<"GET_MAC";
        ptuTcpSendData(cmd,param.macAddr,8);
    }
    else if(HEARTBEAT==cmd)
    {
    }
    else
    {
        qDebug()<<"error:recvFlag:cmd="<<(unsigned short)cmd;
    }
}
void ptu::udpreadyRead()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size(),NULL,NULL);
        if(datagram.length()>=6)
        {
            if(datagram.at(0)==0x01\
                &&datagram.at(1)==0x02\
                &&datagram.at(2)==0x03\
                &&datagram.at(3)==0x04\
                &&datagram.at(4)==0x05\
                &&datagram.at(5)==0x06)
            {
                qDebug()<<"udp recv ok";
                udpOnConnect = true;
                udpOnTime = 0;
            }
            //�ط����յ�������
            //udpSocket->writeDatagram(datagram.data(),datagram.size(),LocalIp1,portTx);
        }
    }
}
void ptu::ptuTcpSendData(short cmd,char *buff,unsigned int len)
{
    QByteArray arr;
    arr.clear();
    arr.append((char)0x7e);//��ͷ

    //����
    arr.append(((char *)&cmd)[1]);
    arr.append(((char *)&cmd)[0]);

    //����
    unsigned int packetlen=13+len;
    arr.append(((char *)&packetlen)[3]);
    arr.append(((char *)&packetlen)[2]);
    arr.append(((char *)&packetlen)[1]);
    arr.append(((char *)&packetlen)[0]);

    //���ݳ���
    arr.append(((char *)&len)[3]);
    arr.append(((char *)&len)[2]);
    arr.append(((char *)&len)[1]);
    arr.append(((char *)&len)[0]);

    //�˴������������
    for(int i=0;i<len;i++)
    {
        arr.append(buff[i]);
    }
    //���У����
    unsigned char check=0;
    unsigned char checkflag=1;
    for(int i=1;i<packetlen-2;i++)
    {
        checkflag = (i%2==1)?1:3;
        check += checkflag*(unsigned char)arr.at(i);
    }
    check /= 11;
    arr.append((char)check);
    arr.append((char)0xfe);//��β
    tcpSocket1->write((char *)arr.data(),(quint64)arr.length());
}


void ptu::TimeoutSLOT()
{
    //��������
    if(false==tcpOnConnect&&true==udpOnConnect)
    {
        init();
    }
    if(true==tcpOnConnect)
    {
        //����������
        ptuTcpSendData(HEARTBEAT,NULL,0);
        //30sû��ͨ�Ŷ˿�����
        if(tcpOnTime++>5)
        {
            tcpSocket1->disconnectFromHost();
            tcpSocket1->close();
            tcpOnConnect = false;
            tcpOnTime = 0;
            qDebug()<<"tcpSocket1->close";
        }
    }
}
bool isTimeOut(QTime &old_current_time,QTime &current_time,int ms)
{
    current_time = QTime::currentTime();
    if(old_current_time.addMSecs(ms) <= current_time)
    {
        old_current_time = current_time;
        return true;
    }
    else
    {
        return false;
    }
}
void ptu::run()
{
    static int time = 0;
    QTime old_current_time = QTime::currentTime();
    QTime current_time = QTime::currentTime();
    int timems = 1000;
    QTime old_current_time50ms = QTime::currentTime();
    QTime current_time50ms = QTime::currentTime();
    int time50ms = 50;
    int flag50ms = 1;
    while(1)
    {
        if(isTimeOut(old_current_time50ms,current_time50ms,time50ms))
        {
            time++;
            if(0==(time%1*flag50ms))
            {
                timeout50ms();
            }
            if(0==(time%(1*2*flag50ms)))
            {
                timeout100ms();
            }
            if(0==(time%(1*3*flag50ms)))
            {
                timeout150ms();
            }
            if(0==(time%(1*2*2*flag50ms)))
            {
                timeout200ms();
            }
            if(0==(time%(1*2*5*flag50ms)))
            {
                timeout500ms();
            }
            if(0==(time%(1*2*10*flag50ms)))
            {
                timeout1s();
            }
        }
    }
}
