#ifndef MYUDPFORSENDSTATUS_H
#define MYUDPFORSENDSTATUS_H

#include <QtNetwork>
#include <QApplication>
#include <QObject>
//#include "CBase.h"
#include "datadefine.h"
//���ⷢ��״̬��Ϣ
class MyUdpforSendStatus : public QObject
{
    Q_OBJECT
public:
    explicit MyUdpforSendStatus(QObject *parent = 0);
    ~MyUdpforSendStatus();
    void initSocket(char* mcaddr,unsigned int mcport);

    void sendPakages(WORD nstatus);
    int drmdDevID = 0;
public slots:
private:
        QUdpSocket * udpSocket;
        unsigned long  FSocket;

        char mc_addr[20];
        unsigned int mc_port;
};

#endif // MYUDPFORSENDSTATUS_H

