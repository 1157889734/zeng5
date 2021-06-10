//#include <QtGui/QApplication>
#include <QApplication>
#include "mainwindow.h"
#include "QTextCodec"
#include <QWSServer>
#include <QDebug>
//#include <QFont>
extern "C"{
#include "include.h"
}
#include <sys/stat.h>
static int dirInit()
{
   int ret;
   ret = access("/root/socket",F_OK);
   if(ret<0)
   {
       mkdir("/root/socket",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   }
   return 1;
}
#include<QMetaType>
int main(int argc, char *argv[])
{
    qDebug()<<"main";

    //Q_DECLARE_METATYPE(BYTE);
    qRegisterMetaType<BYTE> ("BYTE");//
    //fbTest();
    dirInit();
	QApplication a(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8"));
    QWSServer::setCursorVisible(false);
    //QTextCodec *code = QTextCodec::codecForName("gbk");
    //QTextCodec::setCodecForLocale(code);
    QWSServer::setBackground(QColor(0,0,0,0));

    MainWindow *w = new MainWindow();
    qDebug()<<"main end";
    w->showFullScreen();
//    w.hide();    
    return a.exec();
}
