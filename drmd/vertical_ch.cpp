#include "vertical_ch.h"
#include <QDebug>

VERTICALCH::VERTICALCH(QLabel *parent) :
    QLabel(parent)
{
}

VERTICALCH::VERTICALCH(int a)
{
    angle = a;
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,0,0,0));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}


void VERTICALCH::paintEvent(QPaintEvent *)
{
    QFont font;

    font = this->font();
    QPainter painter(this);

    painter.setPen(paintColor.rgb());
    painter.setFont(font);

    QChar qc;
    QString str=this->text();
    painter.translate(0, font.pixelSize()+paintOffset);
    for(int i=0;i<str.length();i++)
    {
        qc = str.at(i);
        if(i!=0)
            painter.translate(0, font.pixelSize());
        painter.drawText(0,0, qc);
    }
    painter.end();
}

void VERTICALCH::setPaintOffset(int offset)
{
    paintOffset = offset;
}

void VERTICALCH::setTextColor(QColor color)
{
    paintColor = color;
}

