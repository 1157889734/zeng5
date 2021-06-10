#ifndef MYUDPFORRCVCONTROL_H
#define MYUDPFORRCVCONTROL_H

#include <QtNetwork>
#include <QApplication>
#include <QObject>
#include "datadefine.h"
class MyUdpforRcvControl;
//接收控制数据，来自VVS的行车信息和控制信息
class MyUdpforRcvControl : public QObject
{
    Q_OBJECT
public:
    explicit MyUdpforRcvControl(QObject *parent = 0);
    ~MyUdpforRcvControl();
    void start_thread_run(void);
    void thread_run(void);

    int drmd_st=0;

signals:
    void running_trigger_signal(BYTE type,BYTE n1,BYTE n2,BYTE n3,BYTE n4,bool door);
    void Emerg_signal(BYTE n1);
    void setDisplayType(BYTE type);
    void setSkip(BYTE *buf,int len);
    void setMirror(bool flag);
    void setColorTest(BYTE mode, BYTE time, BYTE index);
public slots:
    void mainSendProc();
    
    
private:
    pthread_t drmdRecvThreadFd;
    static void* drmdRecvProc(void *arg);
    int startThread(pthread_t *fd,void* (*fun)(void*),void *arg);
    int drmdCmdProc(unsigned char *buf,int len,class MyUdpforRcvControl  *arg);
    unsigned char skipBuf[10];
};

#endif // MYUDPFORRCVCONTROL_H
