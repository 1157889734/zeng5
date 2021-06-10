#ifndef PTU_H
#define PTU_H

#include <QObject>
#include <QtNetwork>
#include <QAbstractSocket>
#include <QTime>
#include <QThread>
struct ptuTcpRecvPacket
{
    unsigned char head;
    unsigned short cmd;
    int packetLen;
    int dataLen;
    unsigned char buff[1024];
};
struct ParamST
{
    char devCtrlCmd = 0;
    char ctrlType = 1;
    char backlight = 50;
    char contrast = 50;
    char devType = 1;
    quint64 usedBytes = 0;
    quint64 freeBytes = 0;
    quint64 totalBytes = 0;
    char version[4] = {1,2,3,4};
    char ipAddr[4] = {192,168,101,29};
    char macAddr[8] = {1,2,3,4,5,6,7,8};
};
class ptu : public QThread
{
    Q_OBJECT
public:
    explicit ptu(QThread *parent = 0);
    void init();
    void run();
    struct ParamST param;
private:
    QTcpSocket *tcpSocket1;
    QHostAddress LocalIp1 = QHostAddress("192.168.101.29");
    QHostAddress tcpServerIp1 = QHostAddress("192.168.101.160");
    quint16 tcplPort1 = 9990;
    quint16 tcpServerPort1 = 9990;
    unsigned int tcpOnTime = 0;
    bool tcpOnConnect = false;

    unsigned int udpOnTime = 0;
    bool udpOnConnect = false;
    quint16 udpServerPort1 = 8889;
    QUdpSocket *udpSocket;
    quint16 portTx = 8889;
    quint16 portRx = 8889;
    //QHostAddress udpIpTx = tcpServerIp1;
    //QHostAddress udpIpRx = tcpServerIp1;

//    QTimer Timer1;
//    int Timer1Ms = 2000;



signals:
    void timeout1s();
    void timeout25ms();
    void timeout50ms();
    void timeout100ms();
    void timeout150ms();
    void timeout200ms();
    void timeout500ms();
public slots:
    void connected();
    void disconnected();
    void tcpreadyRead();
    void TimeoutSLOT();

    void udpreadyRead();
    void ptuTcpSendData(short cmd,char *buff,unsigned int len);
    void ptuTcpRecvProc(QByteArray packet);
    void ptuTcpPacketProc(const struct ptuTcpRecvPacket Ppacket);
};

#endif // PTU_H
