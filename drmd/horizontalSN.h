#ifndef HORIZONTALSN_H
#define HORIZONTALSN_H

#include <QObject>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QColor>

class HORIZONTALSN : public QLabel
{
    Q_OBJECT
public:
    explicit HORIZONTALSN(QLabel *parent = 0);
    HORIZONTALSN(int angle);
    void setPaintPixel(int pixelCh, int pixelEn);
    void setPaintText(QString textCh,QString textEn);
    void setPaintBold(bool BoldCh,bool BoldEn);
    void setPaintOffset(int offset);
    void setTextColor(QColor color);

    int angle;
    int paintOffset;
    int paintPixelCh;
    int paintPixelEn;

    bool paintBoldCh;
    bool paintBoldEn;

    QString paintTextCh;
    QString paintTextEn;
    QColor paintColor;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);

};

#endif // MYCLASS_H
