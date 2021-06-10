#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class Mythread : public QThread
{
    Q_OBJECT
public:
    Mythread();
    void stop();

private:
    volatile bool stopped;
protected:
    void run();

signals:
    void textTrigger();
    void stationTrigger();
};

#endif // MYTHREAD_H
