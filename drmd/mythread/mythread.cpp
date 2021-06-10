#include "mythread.h"
#include <QDebug>
#include "include.h"

Mythread::Mythread()
{
    stopped =false;
}

void Mythread::run()
{
    static struct timeval tv,tv_text,tv_station;
    static struct timezone tz;

    stopped =false;
    gettimeofday(&tv_text,&tz);
    gettimeofday(&tv_station,&tz);

    while(!stopped){
        gettimeofday(&tv,&tz);
        if((tv.tv_usec - tv_text.tv_usec >= 10000)
            ||(tv.tv_usec < tv_text.tv_usec))
        {
            tv_text = tv;
            textTrigger();
        }

        if((tv.tv_usec - tv_station.tv_usec >= 20000)
            ||(tv.tv_usec < tv_station.tv_usec))
        {
            tv_station = tv;
            stationTrigger();
        }
    }
    this->stop();
}

void Mythread::stop()
{
    stopped = true;
    qDebug()<<"mythread stop";
}
