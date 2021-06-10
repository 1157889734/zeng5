#include <QDebug>
#include "horizontalSN.h"

HORIZONTALSN::HORIZONTALSN(QLabel *parent) :
    QLabel(parent)
{
}

HORIZONTALSN::HORIZONTALSN(int a)
{
    angle = a;

    paintBoldCh = false;
    paintBoldEn = false;
    paintOffset = 0;

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,0,0,0));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}


void HORIZONTALSN::paintEvent(QPaintEvent *)
{
    QFont font = this->font();
//    QString str;
    QPainter painter(this);
//    QChar qc;
//    QFontMetrics fm=painter.fontMetrics();

    painter.setPen(paintColor.rgb());


    //计算字宽
    int w = (paintPixelCh*paintTextCh.length()) + (paintPixelEn/2*paintTextEn.length());

    //draw text
    font.setFamily("SourceHanSansCN");
    font.setPixelSize(paintPixelCh);
    font.setBold(paintBoldCh);

    painter.setFont(font);
    int x = (this->width()-w)/2;
//    qDebug()<<"x:"<<x<<"pixel ch"<<paintPixelCh<<"pixel en"<<paintPixelEn;
//    qDebug()<<"paintTextCh.length():"<<paintTextCh.length()<<"paintTextEn.length()"<<paintTextEn.length();

    painter.translate(x, font.pixelSize());
    painter.drawText(0,0, paintTextCh);
    painter.translate(font.pixelSize()*paintTextCh.length(),0);

    font.setFamily("Roboto");
    font.setPixelSize(paintPixelEn);
    font.setBold(paintBoldEn);

    painter.setFont(font);
    painter.drawText(0,0, paintTextEn);

    painter.end();
}

void HORIZONTALSN::setPaintText(QString textCh,QString textEn)
{
    paintTextCh = textCh;
    paintTextEn = textEn;
    this->repaint();
}

void HORIZONTALSN::setPaintPixel(int pixelCh,int pixelEn)
{
    paintPixelCh = pixelCh;
    paintPixelEn = pixelEn;
}

void HORIZONTALSN::setPaintBold(bool BoldCh,bool BoldEn)
{
    paintBoldCh = BoldCh;
    paintBoldEn = BoldEn;
}

void HORIZONTALSN::setPaintOffset(int offset)
{
    paintOffset = offset;
}

void HORIZONTALSN::setTextColor(QColor color)
{
    paintColor = color;
}

