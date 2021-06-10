#include "myudpforrcvcontrol.h"
#include <stdio.h>
#include <unistd.h>
#include "drmd_ui.h"
MyUdpforRcvControl::MyUdpforRcvControl(QObject *parent) :
    QObject(parent)
{
    startThread(&this->drmdRecvThreadFd,this->drmdRecvProc,this);//drmd
}
void MyUdpforRcvControl::start_thread_run(void)
{

    startThread(&this->drmdRecvThreadFd,this->drmdRecvProc,this);//drmd
}
void MyUdpforRcvControl::thread_run(void)
{
    while(0)
    {
        //drmdRecvProc(this);
    }
}

MyUdpforRcvControl::~MyUdpforRcvControl()
{
}

int MyUdpforRcvControl::startThread(pthread_t *fd,void* (*fun)(void*),void *arg)
{
    int ret;
    ret = pthread_create(fd,NULL,fun,arg);
    if(ret<0)
    {
        perror("pthread_create");
        return -1;
    }
    return 1;
}

int MyUdpforRcvControl::drmdCmdProc(unsigned char *buf,int len,class MyUdpforRcvControl  *arg)
{
    if(NULL==buf&&len<4)
    {
        printf("drmdCmdProc buf,len is error\n");
        return -1;
    }
    unsigned char cmd = buf[2];
    if((cmd==SET_CYCLE_TEST_ON)||(cmd==SET_CYCLE_TEST_OFF))
    {
        arg->setColorTest(cmd,buf[4],buf[5]);
        return 1;
    }
    unsigned char start = buf[4];
    unsigned char current = buf[5];
    unsigned char next = buf[6];
    unsigned char end = buf[7];
    bool door = (buf[8]>0)?true:false;
    unsigned char doorErr = buf[9];
    unsigned char emeId = buf[10];
    bool mirrorFlag = (buf[11]>0)?true:false;
    int i;

    int skipLen=8;

    for(i=0;i<skipLen;i++)
    {
        this->skipBuf[i] = buf[13+i];
    }

    arg->setMirror(mirrorFlag);
    arg->setSkip(this->skipBuf,skipLen);

    drmd_st= cmd;
    switch(cmd)
    {
        case SET_LEAVE:
            arg->running_trigger_signal(SET_LEAVE,start,current,next,end,door);            
            break;
        case SET_PRE:
            arg->running_trigger_signal(SET_PRE,start,current,next,end,door);
            break;
        case SET_ARRIVED:
            arg->running_trigger_signal(SET_ARRIVED,start,current,next,end,door);
            break;

        case SET_DOOR_ERROR:
            arg->running_trigger_signal(SET_DOOR_ERROR,start,current,next,end,door);
            break;

        case SET_DOOR_OK:
            arg->running_trigger_signal(SET_DOOR_OK,start,current,next,end,door);
            break;

        case SET_OPEN:
            arg->setDisplayType(SET_OPEN);
            break;
        case SET_CLOSE:
            arg->setDisplayType(SET_CLOSE);
            break;
        case SET_EME:
            //setDisplay(SET_EME);
            arg->Emerg_signal(emeId);
            break;
        case SET_NOSIGNAL:
            arg->setDisplayType(SET_NOSIGNAL);
            break;
        case SET_CLOSE_LCD:
            arg->setDisplayType(SET_CLOSE_LCD);
            break;
        case SET_UPDATE_ING:
            arg->setDisplayType(SET_UPDATE_ING);
            break;
        case SET_UPDATE_OK:
            arg->setDisplayType(SET_UPDATE_OK);
            break;
        case SET_UPDATE_ERROR:
            arg->setDisplayType(SET_UPDATE_ERROR);
            break;

        case SET_IP_CONFLICT_ON:
            arg->setDisplayType(SET_IP_CONFLICT_ON);
            break;

        case SET_IP_CONFLICT_OFF:
            arg->setDisplayType(SET_IP_CONFLICT_OFF);
        break;

        default:
            break;
    }
    return 1;
}

void* MyUdpforRcvControl::drmdRecvProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    int rest=-1;
    int l_recvOk;
    class MyUdpforRcvControl  *my = (class MyUdpforRcvControl  *)arg;

    while(rest<0)
    {
        rest = DrmdInit();
        usleep(1000);
    }
    //my->thread_run();
    while(1)
    {
        ret = DrmdReadData(buf,1024);
        if(ret>0)
        {
            l_recvOk = getPacket(buf,ret);
            if(l_recvOk>0)
            {
                //printf("DrmdReadData:cmd=%d\n",buf[2]);
                my->drmdCmdProc(buf,ret,my);
                DrmdWriteData(buf,ret);
            }
            else
            {
                printf("DrmdReadData is error data cmd=%d\n",buf[2]);
            }
        }
        usleep(1000);
    }
}

void MyUdpforRcvControl::mainSendProc()
{
    int ret;
//    static int cnt=0;
        
    unsigned char buf[100];
    unsigned char dat[100];
    dat[0] = VERSION_MAX;
    dat[1] = VERSION_MIN;
    dat[2] = drmd_st;
    ret = target(buf,DRMD2MAIN,SEND_DRMD_STATUS,dat,3);
    if(ret>0)
    {
        DrmdWriteData(buf,ret);
    }
}
