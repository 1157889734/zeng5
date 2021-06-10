/********************************************************************************
** Form generated from reading UI file 'scrolltext.ui'
**
** Created: Mon Jan 29 19:25:36 2018
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCROLLTEXT_H
#define UI_SCROLLTEXT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_scrolltext
{
public:
    QLabel *labelTextDisp;
    QLabel *labelPlay;

    void setupUi(QWidget *scrolltext)
    {
        if (scrolltext->objectName().isEmpty())
            scrolltext->setObjectName(QString::fromUtf8("scrolltext"));
        scrolltext->resize(1280, 720);
        labelTextDisp = new QLabel(scrolltext);
        labelTextDisp->setObjectName(QString::fromUtf8("labelTextDisp"));
        labelTextDisp->setGeometry(QRect(-100, 380, 1380, 40));
        labelPlay = new QLabel(scrolltext);
        labelPlay->setObjectName(QString::fromUtf8("labelPlay"));
        labelPlay->setGeometry(QRect(0, 150, 1280, 372));

        retranslateUi(scrolltext);

        QMetaObject::connectSlotsByName(scrolltext);
    } // setupUi

    void retranslateUi(QWidget *scrolltext)
    {
        scrolltext->setWindowTitle(QApplication::translate("scrolltext", "Form", 0, QApplication::UnicodeUTF8));
        labelTextDisp->setText(QApplication::translate("scrolltext", "TextLabel", 0, QApplication::UnicodeUTF8));
        labelPlay->setText(QApplication::translate("scrolltext", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class scrolltext: public Ui_scrolltext {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCROLLTEXT_H
