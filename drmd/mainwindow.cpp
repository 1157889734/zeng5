#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPalette>
#include <QTime>
#include <QTimer>
#include <QPainter>
#include <QSettings>
#include <QTabBar>
#include <QMovie>
#include <QProcess>
#include <QFileInfo>

#include "drmd_ui.h"
#include "unistd.h"

#define PRODUCION           // 公版程序宏定义

#define WELCOME 0           // 界面索引宏定义
#define INFODISP 1
#define COLORDISP 2
#define ALL 3
#define PRE 4
#define ARR 5
#define WIDGET_NUM 6

#define LEAVE_ST_OFF 0      // 显示界面状态宏定义
#define LEAVE_ST_ALL 1
#define LEAVE_ST_PRE 2
#define LEAVE_ST_ARR 3

#define ST_OUT 0            // 站点运行状态宏定义
#define ST_SKIP 1
#define ST_PASS 2
#define ST_CURRENT 3
#define ST_NEXT 4
#define ST_NEXTN 5
#define ST_NONE 6

#define LABEL_CHEN_TYPE 0   // 读取配置类型宏定义// example: for xCh xEn yCh yEn ...
#define LABEL_OTHER_TYPE 1

#define LABEL_TYPE_IMAGE 0  // 控件类型宏定义
#define LABEL_TYPE_IMAGE_FLASH 1
#define LABEL_TYPE_TEXT 2

#define NOT_RUN_ID 16

extern public_info_t gPublicInfo;
extern all_info_t gAllInfo;
extern part_info_t gPartInfo;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow";
    qDebug() << "------Version:" << VERSION_MAX << "." << VERSION_MIN << "------";
    /*background clear*/
    ui->setupUi(this);
    QPalette pal;
    pal.setBrush(this->backgroundRole(), QBrush(QColor(0, 0, 0, 0)));
    this->setPalette(pal);
    this->setAutoFillBackground(true);
    //    widget_info_t *ppublic;
    //    all_info_t *pallInfo;
    //    ppublic = (widget_info_t *)malloc(sizeof(widget_info_t));
    //    pallInfo = (all_info_t *)malloc(sizeof(all_info_t));
    readInitFile();   // read config file
    initDipaly(&gPublicInfo, &gAllInfo, &gPartInfo);     // init display ui
    InitEnv();        // init trigger signal
    //    free(ppublic);
    //    free(pallInfo);
    qDebug() << "MainWindow end";
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readLabelAttr(label_info_t *ps, QSettings *set, QString path, int labelType, QString language, int index, bool debug)
{
    QString str, ch, en;
    int i;

    if (labelType == LABEL_CHEN_TYPE)
    {
        str = QString("%1%2%3").arg(path).arg("/x").arg(language);
        ps->rectUp.x = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/y").arg(language);
        ps->rectUp.y = set->value(str).toInt();
        //        str =QString("%1%2%3").arg(path).arg("/x1").arg(language);
        //        ps->rectDown.x = set->value(str).toInt();
        //        str =QString("%1%2%3").arg(path).arg("/y1").arg(language);
        //        ps->rectDown.y = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/w").arg(language);
        ps->rectUp.w = set->value(str).toInt();
        //        ps->rectDown.w = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/h").arg(language);
        ps->rectUp.h = set->value(str).toInt();
        //        ps->rectDown.h = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/r").arg(language);
        ps->rgba.r = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/g").arg(language);
        ps->rgba.g = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/b").arg(language);
        ps->rgba.b = set->value(str).toInt();
        str = QString("%1%2%3").arg(path).arg("/px").arg(language);
        ps->fontSize = set->value(str).toInt();
        ch = "Ch";
        en = "En";
        i = index;
#if 1

        if (debug == true)
        {
            if (language.compare(ch) == 0)
            {
                str = QString("%1").arg(ps->rectUp.x);
                qDebug() << i + 1 << "ch.x:" << str;
                str = QString("%1").arg(ps->rectUp.y);
                qDebug() << i + 1 << "ch.y:" << str;
                //                str = QString("%1").arg(ps->rectDown.x);
                //                qDebug()<<i+1<<"ch.x1:"<<str;
                //                str = QString("%1").arg(ps->rectDown.y);
                //                qDebug()<<i+1<<"ch.y1:"<<str;
                str = QString("%1").arg(ps->rectUp.w);
                qDebug() << i + 1 << "ch.w:" << str;
                str = QString("%1").arg(ps->rectUp.h);
                qDebug() << i + 1 << "ch.h:" << str;
                str = QString("%1").arg(ps->rgba.r);
                qDebug() << i + 1 << "ch.r:" << str;
                str = QString("%1").arg(ps->rgba.g);
                qDebug() << i + 1 << "ch.g:" << str;
                str = QString("%1").arg(ps->rgba.b);
                qDebug() << i + 1 << "ch.b:" << str;
                str = QString("%1").arg(ps->fontSize);
                qDebug() << i + 1 << "ch.px:" << str;
            }

            if (language.compare(en) == 0)
            {
                str = QString("%1").arg(ps->rectUp.x);
                qDebug() << i + 1 << "en.x:" << str;
                str = QString("%1").arg(ps->rectUp.y);
                qDebug() << i + 1 << "en.y:" << str;
                //                str = QString("%1").arg(ps->rectDown.x);
                //                qDebug()<<i+1<<"en.x1:"<<str;
                //                str = QString("%1").arg(ps->rectDown.y);
                //                qDebug()<<i+1<<"en.y1:"<<str;
                str = QString("%1").arg(ps->rectUp.w);
                qDebug() << i + 1 << "en.w:" << str;
                str = QString("%1").arg(ps->rectUp.h);
                qDebug() << i + 1 << "en.h:" << str;
                str = QString("%1").arg(ps->rgba.r);
                qDebug() << i + 1 << "en.r:" << str;
                str = QString("%1").arg(ps->rgba.g);
                qDebug() << i + 1 << "en.g:" << str;
                str = QString("%1").arg(ps->rgba.b);
                qDebug() << i + 1 << "en.b:" << str;
                str = QString("%1").arg(ps->fontSize);
                qDebug() << i + 1 << "en.px:" << str;
            }
        }

#endif
    }
    else
    {
        str = QString("%1%2").arg(path).arg("/x");
        ps->rectUp.x = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/y");
        ps->rectUp.y = set->value(str).toInt();
        //        str =QString("%1%2").arg(path).arg("/x1");
        //        ps->rectDown.x = set->value(str).toInt();
        //        str =QString("%1%2").arg(path).arg("/y1");
        //        ps->rectDown.y = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/w");
        ps->rectUp.w = set->value(str).toInt();
        //        ps->rectDown.w = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/h");
        ps->rectUp.h = set->value(str).toInt();
        //        ps->rectDown.h = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/r");
        ps->rgba.r = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/g");
        ps->rgba.g = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/b");
        ps->rgba.b = set->value(str).toInt();
        str = QString("%1%2").arg(path).arg("/px");
        ps->fontSize = set->value(str).toInt();
#if 1

        if (debug == true)
        {
            str = QString("%1").arg(ps->rectUp.x);
            qDebug() << "x:" << str;
            str = QString("%1").arg(ps->rectUp.y);
            qDebug() << "y:" << str;
            //            str = QString("%1").arg(ps->rectDown.x);
            //            qDebug()<<"x1:"<<str;
            //            str = QString("%1").arg(ps->rectDown.y);
            //            qDebug()<<"y1:"<<str;
            str = QString("%1").arg(ps->rectUp.w);
            qDebug() << "w:" << str;
            str = QString("%1").arg(ps->rectUp.h);
            qDebug() << "h:" << str;
            str = QString("%1").arg(ps->rgba.r);
            qDebug() << "r:" << str;
            str = QString("%1").arg(ps->rgba.g);
            qDebug() << "g:" << str;
            str = QString("%1").arg(ps->rgba.b);
            qDebug() << "b:" << str;
            str = QString("%1").arg(ps->fontSize);
            qDebug() << "px:" << str;
        }

#endif
    }
}

void MainWindow::readInitFile()
{
    printf("readInitFile\n");
    /*****************(1)netcfg.ini*********************************************************/
    QString netcfgPath = "/etc/net.conf";
    QSettings settings_netcfg(netcfgPath, QSettings::IniFormat);
    settings_netcfg.setIniCodec("UTF8");
    //QString cfgPath = "netconfig/";
    QString str_net;
    //QImage image;
    str_net = settings_netcfg.value("netconfig/IPADDR").toString();    /*---ip配置---*/
    str_net = str_net.trimmed();
    qDebug() << "IP:" << str_net;
    mytimer.ipStr = str_net;
    QStringList list = str_net.split(".");
    ip4 = list[3].toInt();
    str_net = settings_netcfg.value("netconfig/MAC").toString();
    str_net = str_net.trimmed();
    qDebug() << "MAC:" << str_net;
    mytimer.macStr = str_net;
    macStr = str_net;
}

/*控件初始化函数*/
void MainWindow::labelInit(QLabel &label, label_info_t info_t, int type, QString str, int widgetNum, QString fontStr, bool blod)
{
    QImage image;
    QPixmap p;
    label.setParent(&widget[widgetNum]);
    label.setGeometry(info_t.rectUp.x, info_t.rectUp.y, info_t.rectUp.w, info_t.rectUp.h);

    if (type == LABEL_TYPE_IMAGE)
    {
        if (str.compare("NULL") != 0)
        {
            image.load(str);
            p = QPixmap::fromImage(image.scaled(info_t.rectUp.w, info_t.rectUp.h, Qt::IgnoreAspectRatio));
            label.setPixmap(p);
        }
    }
    else if (type == LABEL_TYPE_TEXT)
    {
        setFont(label, fontStr, blod, QFont::Normal, info_t.fontSize, info_t.rgba.r, info_t.rgba.g, info_t.rgba.b);
        //        label.setText(str);
    }

    label.show();
}

void MainWindow::publicInit(public_info_t *ppublic)
{
    int i;
    QString str;
    bool debug = false;
    QString displayPath = "/root/config/display.ini";
    QSettings settings_disp(displayPath, QSettings::IniFormat);
    settings_disp.setIniCodec("UTF8");
    qDebug() << "------------------------------publicInit------------------------------";
    /*******中英文站名文本******/
    StationMaxNumber = DEFINE_STATION_NUM;                              //站名文本
    stationNum = settings_disp.value("StationNum/num").toInt();
    StationMaxNumber = stationNum;                                      // 站总数

    for (int i = 0; i < stationNum; i++) // 获取站名
    {
        str.clear();
        str = QString("%1%2").arg("StationNameCh/").arg(i + 1, 2, 10, QLatin1Char('0'));
        str = settings_disp.value(str).toString();
        str = str.trimmed();
        stationChName.append(str);
        //qDebug()<<"station:"<<i<<" Ch: "<<str;
    }

    int dispLines = 0;

    for (int i = 0; i < stationNum; i++)
    {
        str.clear();
        str = QString("%1%2").arg("SNEnDispLines/").arg(i + 1, 2, 10, QLatin1Char('0'));
        dispLines = settings_disp.value(str).toInt();
        stationEnDispLines.append(dispLines);
        //qDebug()<<"station:"<<i<<"dispLines:"<<dispLines;
    }

    for (int i = 0; i < stationNum; i++)
    {
        str.clear();
        str = QString("%1%2").arg("StationNameEn/").arg(i + 1, 2, 10, QLatin1Char('0'));
        str = settings_disp.value(str).toString();
        str = str.trimmed();
        stationEnName.append(str);
        //qDebug()<<"station:"<<i<<" En: "<<str;
    }

    for (int i = 0; i < stationNum; i++)
    {
        str.clear();
        str = QString("%1%2").arg("StationNameEn2/").arg(i + 1, 2, 10, QLatin1Char('0'));
        str = settings_disp.value(str).toString();
        str = str.trimmed();
        stationEnName2.append(str);
        //qDebug()<<"station:"<<i<<" En2: "<<str;
    }

    qDebug() << "------Widget------";
    readLabelAttr(&ppublic->widget, &settings_disp, "Widget", LABEL_OTHER_TYPE, "NULL", 0, debug);

    for (i = 0; i < WIDGET_NUM; i++)
    {
        widget[i].setParent(ui->centralWidget);
        widget[i].setGeometry(ppublic->widget.rectUp.x, ppublic->widget.rectUp.y, ppublic->widget.rectUp.w, ppublic->widget.rectUp.h);
    }

    qDebug() << "------Welcome------";
    readLabelAttr(&ppublic->welcomeLable, &settings_disp, "Welcome", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(welcomeLable, ppublic->welcomeLable, LABEL_TYPE_IMAGE, welcomePath, WELCOME, "NULL", false);
    qDebug() << "------Update------";
    readLabelAttr(&ppublic->updateLable, &settings_disp, "Update", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(updateLable, ppublic->updateLable, LABEL_TYPE_TEXT, "NULL", WELCOME, "Oswald", false);
    updateLable.setAlignment(Qt::AlignCenter);
    qDebug() << "------Background------";
    readLabelAttr(&ppublic->background, &settings_disp, "Background", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(allBgLabel, ppublic->background, LABEL_TYPE_IMAGE, imageDir + "ui_all/BG/bg.png", ALL, "NULL", false);
    labelInit(preBgLabel, ppublic->background, LABEL_TYPE_IMAGE, imageDir + "ui_pre/BG/bg.png", PRE, "NULL", false);
    labelInit(arrBgLabel, ppublic->background, LABEL_TYPE_IMAGE, imageDir + "ui_arr/BG/bg.png", ARR, "NULL", false);
    qDebug() << "------SNColor------";
    readLabelAttr(&ppublic->SNOutRangeColor, &settings_disp, "SNOutRangeColor", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->SNPassColor, &settings_disp, "SNPassColor", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->SNCurrentColor, &settings_disp, "SNCurrentColor", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->SNNextColor, &settings_disp, "SNNextColor", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->SNNextNColor, &settings_disp, "SNNextNColor", LABEL_OTHER_TYPE, "Ch", 0, debug);
    //"******菜单栏信息******"
    qDebug() << "------LOGO------";
    readLabelAttr(&ppublic->logo, &settings_disp, "LOGO", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(allLogo, ppublic->logo, LABEL_TYPE_IMAGE, imageDir + "menu/logo.png", ALL, "NULL", false);
    labelInit(preLogo, ppublic->logo, LABEL_TYPE_IMAGE, imageDir + "menu/logo.png", PRE, "NULL", false);
    labelInit(arrLogo, ppublic->logo, LABEL_TYPE_IMAGE, imageDir + "menu/logo.png", ARR, "NULL", false);
    qDebug() << "------DateTime------";
    debug = false;
    readLabelAttr(&ppublic->dateLabel, &settings_disp, "Date", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->weekChLabel, &settings_disp, "WeekCh", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->weekEnLabel, &settings_disp, "WeekEn", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->timeLabel, &settings_disp, "Time", LABEL_OTHER_TYPE, "Ch", 0, debug);
    //readLabelAttr(&ppublic->weekSplitLabel,&settings_disp,"WeekSplit",LABEL_OTHER_TYPE,"NULL",0,debug);
    labelInit(dateLabel, ppublic->dateLabel, LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
    dateLabel.setAlignment(Qt::AlignLeft);
    labelInit(weekChLabel, ppublic->weekChLabel, LABEL_TYPE_TEXT, "NULL", ALL, "SourceHanSansCN", false);
    weekChLabel.setAlignment(Qt::AlignLeft);
    labelInit(weekEnLabel, ppublic->weekEnLabel, LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
    weekEnLabel.setAlignment(Qt::AlignLeft);
    labelInit(timeLabel, ppublic->timeLabel, LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
    timeLabel.setAlignment(Qt::AlignLeft);
    labelInit(preDateLabel, ppublic->dateLabel, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", false);
    preDateLabel.setAlignment(Qt::AlignLeft);
    labelInit(preWeekChLabel, ppublic->weekChLabel, LABEL_TYPE_TEXT, "NULL", PRE, "SourceHanSansCN", false);
    preWeekChLabel.setAlignment(Qt::AlignLeft);
    labelInit(preWeekEnLabel, ppublic->weekEnLabel, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", false);
    preWeekEnLabel.setAlignment(Qt::AlignLeft);
    labelInit(preTimeLabel, ppublic->timeLabel, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", false);
    preTimeLabel.setAlignment(Qt::AlignLeft);
    labelInit(arrDateLabel, ppublic->dateLabel, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    arrDateLabel.setAlignment(Qt::AlignLeft);
    labelInit(arrWeekChLabel, ppublic->weekChLabel, LABEL_TYPE_TEXT, "NULL", ARR, "SourceHanSansCN", false);
    arrWeekChLabel.setAlignment(Qt::AlignLeft);
    labelInit(arrWeekEnLabel, ppublic->weekEnLabel, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    arrWeekEnLabel.setAlignment(Qt::AlignLeft);
    labelInit(arrTimeLabel, ppublic->timeLabel, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    arrTimeLabel.setAlignment(Qt::AlignLeft);
    qDebug() << "------MNextText------";
    readLabelAttr(&ppublic->MNextTextCh, &settings_disp, "MNextText", LABEL_CHEN_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->MNextTextEn, &settings_disp, "MNextText", LABEL_CHEN_TYPE, "En", 0, debug);
    labelInit(allMNTextCh, ppublic->MNextTextCh, LABEL_TYPE_TEXT, "NULL", ALL, "SourceHanSansCN", false);
    labelInit(allMNTextEn, ppublic->MNextTextEn, LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
    labelInit(preMNTextCh, ppublic->MNextTextCh, LABEL_TYPE_TEXT, "NULL", PRE, "SourceHanSansCN", false);
    labelInit(preMNTextEn, ppublic->MNextTextEn, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", false);
    allMNTextCh.setText("下一站");
    allMNTextEn.setText("Next Station");
    preMNTextCh.setText("下一站");
    preMNTextEn.setText("Next Station");
    qDebug() << "------MCurrentText------";
    readLabelAttr(&ppublic->MCurrentTextCh, &settings_disp, "MCurrentText", LABEL_CHEN_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->MCurrentTextEn, &settings_disp, "MCurrentText", LABEL_CHEN_TYPE, "En", 0, debug);
    labelInit(arrMCTextCh, ppublic->MCurrentTextCh, LABEL_TYPE_TEXT, "NULL", ARR, "SourceHanSansCN", false);
    labelInit(arrMCTextEn, ppublic->MCurrentTextEn, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    arrMCTextCh.setText("当前站");
    arrMCTextEn.setText("This Station");
    qDebug() << "------MNextSN------";
    readLabelAttr(&ppublic->MNextSNCh, &settings_disp, "MNextSN", LABEL_CHEN_TYPE, "Ch", 0, debug);
    readLabelAttr(&ppublic->MNextSNEn, &settings_disp, "MNextSN", LABEL_CHEN_TYPE, "En", 0, debug);
    labelInit(allMNextSNCh, ppublic->MNextSNCh, LABEL_TYPE_TEXT, "NULL", ALL, "SourceHanSansCN", false);
    labelInit(allMNextSNEn, ppublic->MNextSNEn, LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
    labelInit(preMNextSNCh, ppublic->MNextSNCh, LABEL_TYPE_TEXT, "NULL", PRE, "SourceHanSansCN", false);
    labelInit(preMNextSNEn, ppublic->MNextSNEn, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", false);
    labelInit(arrMNextSNCh, ppublic->MNextSNCh, LABEL_TYPE_TEXT, "NULL", ARR, "SourceHanSansCN", false);
    labelInit(arrMNextSNEn, ppublic->MNextSNEn, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    qDebug() << "------MRundir------";
    readLabelAttr(&ppublic->MRundir, &settings_disp, "MDIR", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(allMRundir, ppublic->MRundir, LABEL_TYPE_IMAGE, imageDir + "menu/runIn.png", ALL, "NULL", false);
    labelInit(preMRundir, ppublic->MRundir, LABEL_TYPE_IMAGE, imageDir + "menu/runIn.png", PRE, "NULL", false);
    labelInit(arrMRundir, ppublic->MRundir, LABEL_TYPE_IMAGE, imageDir + "menu/runIn.png", ARR, "NULL", false);
}

void MainWindow::allInit(all_info_t *pallInfo)
{
    QImage image;
    QString str;
    bool debug = false;
    QString displayPath = "/root/config/display.ini";
    QSettings settings_disp(displayPath, QSettings::IniFormat);
    settings_disp.setIniCodec("UTF8");
    qDebug() << "------------------------------allInit------------------------------";
    qDebug() << "---Allchg---";
    int chgnum = 0;
    int chgIndex = 0;

    for (int i = 0; i < stationNum; i++) // 换乘线路信息
    {
        str = QString("%1%2%3").arg("AllStationChg_").arg(i + 1, 2, 10, QLatin1Char('0')).arg("/chgnum");
        chgnum = settings_disp.value(str).toInt();
        pallInfo->stationChgNum[i] = chgnum;

        if (chgnum > 0)
        {
            for (int j = 0; j < chgnum; j++)
            {
                str = QString("%1%2%3%4").arg("AllStationChg_").arg(i + 1, 2, 10, QLatin1Char('0')).arg("/xchg_").arg(j + 1, 2, 10, QLatin1Char('0'));
                pallInfo->chg[chgIndex].rect.x = settings_disp.value(str).toInt();
                str = QString("%1%2%3%4").arg("AllStationChg_").arg(i + 1, 2, 10, QLatin1Char('0')).arg("/ychg_").arg(j + 1, 2, 10, QLatin1Char('0'));
                pallInfo->chg[chgIndex].rect.y = settings_disp.value(str).toInt();
                str = QString("%1%2%3").arg("AllStationChg_").arg(i + 1, 2, 10, QLatin1Char('0')).arg("/w");
                pallInfo->chg[chgIndex].rect.w = settings_disp.value(str).toInt();
                str = QString("%1%2%3").arg("AllStationChg_").arg(i + 1, 2, 10, QLatin1Char('0')).arg("/h");
                pallInfo->chg[chgIndex].rect.h = settings_disp.value(str).toInt();
                str = QString("%1%2%3%4").arg("AllStationChg_").arg(i + 1, 2, 10, QLatin1Char('0')).arg("/linechg_").arg(j + 1, 2, 10, QLatin1Char('0'));
                pallInfo->chg[chgIndex].lineNo = settings_disp.value(str).toInt();
                pallInfo->chg[chgIndex].stationId = i + 1;
                chgIndex++;
            }
        }
    }

    pallInfo->chg[chgIndex].stationId = 0;
    label_info_t chgLabelInfo;

    for (int i = 0; i < 100; i++)
    {
        if (pallInfo->chg[i].stationId != 0)
        {
            allChg[i] = new QLabel();

            if (pallInfo->chg[i].stationId <= 16)
                str = QString("%1%2%3").arg(imageDir + "ui_all/CHANGE/line/next").arg(pallInfo->chg[i].lineNo, 2, 10, QLatin1Char('0')).arg("_up.png");
            else
                str = QString("%1%2%3").arg(imageDir + "ui_all/CHANGE/line/next").arg(pallInfo->chg[i].lineNo, 2, 10, QLatin1Char('0')).arg("_down.png");

            chgLabelInfo.rectUp.x = pallInfo->chg[i].rect.x;
            chgLabelInfo.rectUp.y = pallInfo->chg[i].rect.y;
            chgLabelInfo.rectUp.w = pallInfo->chg[i].rect.w;
            chgLabelInfo.rectUp.h = pallInfo->chg[i].rect.h;
            labelInit(*allChg[i], chgLabelInfo, LABEL_TYPE_IMAGE, str, ALL, "NULL", false);
        }
        else
        {
            break;
        }
    }

    qDebug() << "---AllTrack---";

    for (int i = 0; i < stationNum; i++)
    {
        str = QString("%1%2").arg("AllTrack_").arg(i + 1, 2, 10, QLatin1Char('0'));
        readLabelAttr(&pallInfo->track[i], &settings_disp, str, LABEL_OTHER_TYPE, "Ch", i, debug);
        allTrack[i] = new QLabel();

        if ((1 + i) < 16)
            str = imageDir + "ui_all/TRACK/upNormal.png";
        else if ((1 + i) == 16)
            str = imageDir + "ui_all/TRACK/rightNormal.png";
        else if ((1 + i) < 32)
            str = imageDir + "ui_all/TRACK/downNormal.png";
        else if ((1 + i) == 32)
            str = imageDir + "ui_all/TRACK/leftNormal.png";

        labelInit(*allTrack[i], pallInfo->track[i], LABEL_TYPE_IMAGE, str, ALL, "NULL", false);
    }

    readLabelAttr(&pallInfo->trackFlash[0], &settings_disp, "AllTrackLeftFlash", LABEL_OTHER_TYPE, "Ch", 0, debug);
    readLabelAttr(&pallInfo->trackFlash[1], &settings_disp, "AllTrackRightFlash", LABEL_OTHER_TYPE, "Ch", 0, debug);
    allTrackFlash.setParent(&widget[ALL]);
    allTrackMovie = new QMovie();
    qDebug() << "---AllSite---";

    for (int i = 0; i < stationNum; i++)
    {
        str = QString("%1%2").arg("AllSite_").arg(i + 1, 2, 10, QLatin1Char('0'));
        readLabelAttr(&pallInfo->site[i], &settings_disp, str, LABEL_OTHER_TYPE, "Ch", i, debug);
        allSite[i] = new QLabel();
        labelInit(*allSite[i], pallInfo->site[i], LABEL_TYPE_IMAGE, imageDir + "ui_all/SITE/nextn.png", ALL, "NULL", false);
    }

    readLabelAttr(&pallInfo->siteFlash, &settings_disp, "AllSiteFlash", LABEL_OTHER_TYPE, "Ch", 0, debug);
    allSiteFlash.setParent(&widget[ALL]);
    allSiteMovie = new QMovie();
    qDebug() << "---AllSN---";

    for (int i = 0; i < stationNum; i++)
    {
        if ((i + 1) == NOT_RUN_ID)
            continue;

        if ((i + 1) <= 16)
            str = "AllSN_UP";
        else
            str = "AllSN_DOWN";

        readLabelAttr(&pallInfo->chSN[i], &settings_disp, str, LABEL_CHEN_TYPE, "Ch", i, debug);
        readLabelAttr(&pallInfo->enSN[i], &settings_disp, str, LABEL_CHEN_TYPE, "En", i, debug);
        pallInfo->chSN[i].rectUp.x = pallInfo->site[i].rectUp.x - (pallInfo->chSN[i].rectUp.w - pallInfo->site[i].rectUp.w) / 2;
        pallInfo->enSN[i].rectUp.x = pallInfo->chSN[i].rectUp.x;
        allSNPassCh[i] = new QLabel();
        labelInit(*allSNPassCh[i], pallInfo->chSN[i], LABEL_TYPE_TEXT, "NULL", ALL, "SourceHanSansCN", false);
        allSNPassCh[i]->setText(stationChName[i]);
        allSNPassCh[i]->setAlignment(Qt::AlignCenter);
        allSNPassCh[i]->hide();
        allSNNextCh[i] = new QLabel();
        labelInit(*allSNNextCh[i], pallInfo->chSN[i], LABEL_TYPE_TEXT, "NULL", ALL, "SourceHanSansCN", false);
        allSNNextCh[i]->setText(stationChName[i]);
        allSNNextCh[i]->setAlignment(Qt::AlignCenter);
        allSNPassEn2[i] = new QLabel();
        allSNNextEn2[i] = new QLabel();
        allSNNextEn2[i]->hide();

        if ((stationEnDispLines[i] == 2) || (stationEnDispLines[i] == 20))
        {
            if ((i + 1) <= 16)
                pallInfo->enSN[i].rectUp.y = 105;
            else
                pallInfo->enSN[i].rectUp.y = 265;

            labelInit(*allSNPassEn2[i], pallInfo->enSN[i], LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
            allSNPassEn2[i]->setText(stationEnName2[i]);
            allSNPassEn2[i]->setAlignment(Qt::AlignCenter);
            allSNPassEn2[i]->hide();
            labelInit(*allSNNextEn2[i], pallInfo->enSN[i], LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
            allSNNextEn2[i]->setText(stationEnName2[i]);
            allSNNextEn2[i]->setAlignment(Qt::AlignCenter);
            allSNNextEn2[i]->show();

            if ((i + 1) <= 16)
                pallInfo->enSN[i].rectUp.y = 90;
            else
                pallInfo->enSN[i].rectUp.y = 250;
        }

        allSNPassEn[i] = new QLabel();
        labelInit(*allSNPassEn[i], pallInfo->enSN[i], LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
        allSNPassEn[i]->setText(stationEnName[i]);
        allSNPassEn[i]->setAlignment(Qt::AlignCenter);
        allSNPassEn[i]->hide();
        allSNNextEn[i] = new QLabel();
        labelInit(*allSNNextEn[i], pallInfo->enSN[i], LABEL_TYPE_TEXT, "NULL", ALL, "Oswald-Medium", false);
        allSNNextEn[i]->setText(stationEnName[i]);
        allSNNextEn[i]->setAlignment(Qt::AlignCenter);
    }

    qDebug() << "---AllJump---";                        //越站图标
    image.load(allSitePath + "jump.png");
    str = allSitePath + "jump.png";

    for (int i = 0; i < stationNum; i++)
    {
        allJump[i] = new QLabel();
        allJump[i]->setParent(&widget[ALL]);
        allJump[i]->setGeometry(pallInfo->site[i].rectUp.x - (image.width() - pallInfo->site[i].rectUp.w) / 2, pallInfo->site[i].rectUp.y - (image.height() - pallInfo->site[i].rectUp.h) / 2, image.width(), image.height());
        allJump[i]->setPixmap(str);
        allJump[i]->hide();
    }

    qDebug() << "---AllStartEnd---";                    //起始终点站图标
    image.load(allSitePath + "end.png");
    str = allSitePath + "end.png";

    for (int i = 0; i < stationNum; i++)
    {
        str = allSitePath + "start.png";
        allStart[i] = new QLabel();
        allStart[i]->setParent(&widget[ALL]);
        allStart[i]->setGeometry(pallInfo->site[i].rectUp.x - (image.width() - pallInfo->site[i].rectUp.w) / 2, pallInfo->site[i].rectUp.y - (image.height() - pallInfo->site[i].rectUp.h) / 2, image.width(), image.height());
        allStart[i]->setPixmap(str);
        allStart[i]->hide();
        str = allSitePath + "end.png";
        allEnd[i] = new QLabel();
        allEnd[i]->setParent(&widget[ALL]);
        allEnd[i]->setGeometry(pallInfo->site[i].rectUp.x - (image.width() - pallInfo->site[i].rectUp.w) / 2, pallInfo->site[i].rectUp.y - (image.height() - pallInfo->site[i].rectUp.h) / 2, image.width(), image.height());
        allEnd[i]->setPixmap(str);
        allEnd[i]->hide();
    }

    qDebug() << "---AllNotRunning---";                        //未开通图标
    readLabelAttr(&pallInfo->notRunImage, &settings_disp, "AllNotRunImage", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(allNotRunImage, pallInfo->notRunImage, LABEL_TYPE_IMAGE, imageDir + "ui_all/OTHER/notRunning.png", ALL, "NULL", false);
    readLabelAttr(&pallInfo->notRunSN, &settings_disp, "AllNotRunSN", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(allNotRunSN, pallInfo->notRunSN, LABEL_TYPE_IMAGE, imageDir + "ui_all/OTHER/notRunning16.png", ALL, "NULL", false);
    qDebug() << "---AllRunDirInOut---";                        //内外环方向动图
    readLabelAttr(&pallInfo->runDirLeft, &settings_disp, "AllRunDirLeft", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(allRunDirLeft, pallInfo->runDirLeft, LABEL_TYPE_IMAGE, "NULL", ALL, "NULL", false);
    allRDLeftMovie = new QMovie();
    readLabelAttr(&pallInfo->runDirRight, &settings_disp, "AllRunDirRight", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(allRunDirRight, pallInfo->runDirRight, LABEL_TYPE_IMAGE, "NULL", ALL, "NULL", false);
    allRDRightMovie = new QMovie();
    //    image.load(imageDir+"ui_all/CHANGE/site/next.png");
    //    pallInfo->siteChgFlashFrameNum = image.height()/pallInfo->site[0].rectUp.h;
}

void MainWindow::preInit(part_info_t *ppartInfo)
{
    int i = 0;
    QString str;
    bool debug = false;
    QString displayPath = "/root/config/display.ini";
    QSettings settings_disp(displayPath, QSettings::IniFormat);
    settings_disp.setIniCodec("UTF8");
    qDebug() << "------------------------------preInit------------------------------";
    qDebug() << "---Pre PartSiteTrack---";
    readLabelAttr(&ppartInfo->trackFlash, &settings_disp, "PartSiteTrack", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(preTrackFlash, ppartInfo->trackFlash, LABEL_TYPE_IMAGE, "NULL", PRE, "NULL", false);
    preTrackMovie = new QMovie();
    qDebug() << "------Pre Chg------";
    readLabelAttr(&ppartInfo->chg[0], &settings_disp, "Chg1", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(preChg[0], ppartInfo->chg[0], LABEL_TYPE_IMAGE, "NULL", PRE, "NULL", false);
    readLabelAttr(&ppartInfo->chg[1], &settings_disp, "Chg2", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(preChg[1], ppartInfo->chg[1], LABEL_TYPE_IMAGE, "NULL", PRE, "NULL", false);
    qDebug() << "---PartSN---";
    str = QString("%1%2").arg("PartSN_").arg(i + 1, 2, 10, QLatin1Char('0'));
    readLabelAttr(&ppartInfo->currentSNch, &settings_disp, "PartSN_01", LABEL_CHEN_TYPE, "Ch", i, debug);
    readLabelAttr(&ppartInfo->currentSNen, &settings_disp, "PartSN_01", LABEL_CHEN_TYPE, "En", i, debug);
    labelInit(precurrentCh, ppartInfo->currentSNch, LABEL_TYPE_TEXT, "NULL", PRE, "SourceHanSansCN", true);
    labelInit(precurrentEn, ppartInfo->currentSNen, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", true);
    precurrentCh.setAlignment(Qt::AlignLeft);
    precurrentEn.setAlignment(Qt::AlignLeft);
    readLabelAttr(&ppartInfo->nextSNch, &settings_disp, "PartSN_02", LABEL_CHEN_TYPE, "Ch", i, debug);
    readLabelAttr(&ppartInfo->nextSNen, &settings_disp, "PartSN_02", LABEL_CHEN_TYPE, "En", i, debug);
    labelInit(prenextCh, ppartInfo->nextSNch, LABEL_TYPE_TEXT, "NULL", PRE, "SourceHanSansCN", true);
    labelInit(prenextEn, ppartInfo->nextSNen, LABEL_TYPE_TEXT, "NULL", PRE, "Oswald-Medium", true);
    prenextCh.setAlignment(Qt::AlignLeft);
    prenextEn.setAlignment(Qt::AlignLeft);
    qDebug() << "------Pre Door------";
    readLabelAttr(&ppartInfo->doorOpenImg[0], &settings_disp, "OpenDoor1", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(preOpen[0], ppartInfo->doorOpenImg[0], LABEL_TYPE_IMAGE, imageDir + "ui_pre/DOOR/preOpen.png", PRE, "NULL", false);
    readLabelAttr(&ppartInfo->doorOpenImg[1], &settings_disp, "OpenDoor2", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(preOpen[1], ppartInfo->doorOpenImg[1], LABEL_TYPE_IMAGE, imageDir + "ui_pre/DOOR/preOpen.png", PRE, "NULL", false);
    readLabelAttr(&ppartInfo->doorCloseImg[0], &settings_disp, "CloseDoor1", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(preClose[0], ppartInfo->doorCloseImg[0], LABEL_TYPE_IMAGE, imageDir + "ui_pre/DOOR/preClose.png", PRE, "NULL", false);
    readLabelAttr(&ppartInfo->doorCloseImg[1], &settings_disp, "CloseDoor2", LABEL_OTHER_TYPE, "NULL", 0, debug);
    labelInit(preClose[1], ppartInfo->doorCloseImg[1], LABEL_TYPE_IMAGE, imageDir + "ui_pre/DOOR/preClose.png", PRE, "NULL", false);

    qDebug() << "---PreNotRunning---";                        //未开通图标
    readLabelAttr(&ppartInfo->notRunImage, &settings_disp, "PreNotRunImage", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(preNotRunImage, ppartInfo->notRunImage, LABEL_TYPE_IMAGE, imageDir + "ui_pre/SITE/notRunning16.png", PRE, "NULL", false);
    readLabelAttr(&ppartInfo->notRunSN, &settings_disp, "PreNotRunSN", LABEL_OTHER_TYPE, "Ch", 0, debug);
    labelInit(preNotRunSN, ppartInfo->notRunSN, LABEL_TYPE_IMAGE, imageDir + "ui_pre/SITE/notRunning.png", PRE, "NULL", false);

}

void MainWindow::arrInit(part_info_t *ppartInfo)
{
    int i = 0;
    bool debug = false;
    QString displayPath = "/root/config/display.ini";
    QSettings settings_disp(displayPath, QSettings::IniFormat);
    settings_disp.setIniCodec("UTF8");
    qDebug() << "------------------------------arrInit------------------------------";
    qDebug() << "---Arr Open---";
    readLabelAttr(&ppartInfo->ArrOpenDoor, &settings_disp, "ArriveOpenDoor", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrOpenFlash, ppartInfo->ArrOpenDoor, LABEL_TYPE_IMAGE, "NULL", ARR, "NULL", false);
    arrOpenMovie = new QMovie();
    readLabelAttr(&ppartInfo->ArrOpenLeftTextCh, &settings_disp, "ArriveOpenTextL", LABEL_CHEN_TYPE, "Ch", i, debug);
    readLabelAttr(&ppartInfo->ArrOpenLeftTextEn, &settings_disp, "ArriveOpenTextL", LABEL_CHEN_TYPE, "En", i, debug);
    labelInit(arrOpenTextCh[0], ppartInfo->ArrOpenLeftTextCh, LABEL_TYPE_TEXT, "NULL", ARR, "SourceHanSansCN", false);
    labelInit(arrOpenTextEn[0], ppartInfo->ArrOpenLeftTextEn, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    readLabelAttr(&ppartInfo->ArrOpenRightTextCh, &settings_disp, "ArriveOpenTextR", LABEL_CHEN_TYPE, "Ch", i, debug);
    readLabelAttr(&ppartInfo->ArrOpenRightTextEn, &settings_disp, "ArriveOpenTextR", LABEL_CHEN_TYPE, "En", i, debug);
    labelInit(arrOpenTextCh[1], ppartInfo->ArrOpenRightTextCh, LABEL_TYPE_TEXT, "NULL", ARR, "SourceHanSansCN", false);
    labelInit(arrOpenTextEn[1], ppartInfo->ArrOpenRightTextEn, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    arrOpenTextCh[0].setText("本侧开门");
    arrOpenTextEn[0].setText("Doors open on this side");
    arrOpenTextCh[1].setText("本侧开门");
    arrOpenTextEn[1].setText("Doors open on this side");
    qDebug() << "---Arr SN---";
    readLabelAttr(&ppartInfo->ArriveChSN, &settings_disp, "ArriveOpenSN", LABEL_CHEN_TYPE, "Ch", i, debug);
    readLabelAttr(&ppartInfo->ArriveEnSN, &settings_disp, "ArriveOpenSN", LABEL_CHEN_TYPE, "En", i, debug);
    labelInit(arrStationNameCh, ppartInfo->ArriveChSN, LABEL_TYPE_TEXT, "NULL", ARR, "SourceHanSansCN", false);
    labelInit(arrStationNameEn, ppartInfo->ArriveEnSN, LABEL_TYPE_TEXT, "NULL", ARR, "Oswald-Medium", false);
    arrStationNameCh.setAlignment(Qt::AlignCenter);
    arrStationNameEn.setAlignment(Qt::AlignCenter);
    qDebug() << "---Arr Chg---";
    readLabelAttr(&ppartInfo->ArrOpenChgTextImg, &settings_disp, "ArriveChgText", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrOpenChgImg, ppartInfo->ArrOpenChgTextImg, LABEL_TYPE_IMAGE, imageDir + "ui_arr/CHANGE/line/text.png", ARR, "NULL", false);
    readLabelAttr(&ppartInfo->ArrOpenChgLine, &settings_disp, "ArriveChgLine", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrOpenChgLine, ppartInfo->ArrOpenChgLine, LABEL_TYPE_IMAGE, "NULL", ARR, "NULL", false);
    readLabelAttr(&ppartInfo->ArrCloseChgTextImg, &settings_disp, "ArriveChgText2", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrCloseChgImg, ppartInfo->ArrCloseChgTextImg, LABEL_TYPE_IMAGE, imageDir + "ui_arr/CHANGE/line/text.png", ARR, "NULL", false);
    readLabelAttr(&ppartInfo->ArrCloseChgLine, &settings_disp, "ArriveChgLine2", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrCloseChgLine, ppartInfo->ArrCloseChgLine, LABEL_TYPE_IMAGE, "NULL", ARR, "NULL", false);
    qDebug() << "---Arr Close---";
    readLabelAttr(&ppartInfo->ArrCloseDoorImg, &settings_disp, "ArriveCloseImage", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrCloseFlash, ppartInfo->ArrCloseDoorImg, LABEL_TYPE_IMAGE, "NULL", ARR, "NULL", false);

    qDebug() << "---Arr Close Text---";
    readLabelAttr(&ppartInfo->ArrCloseTextL, &settings_disp, "ArriveCloseTextL", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrCloseTextL, ppartInfo->ArrCloseTextL, LABEL_TYPE_IMAGE, imageDir + "ui_arr/DOOR/closeText.png", ARR, "NULL", false);
    readLabelAttr(&ppartInfo->ArrCloseTextR, &settings_disp, "ArriveCloseTextR", LABEL_OTHER_TYPE, "NULL", i, debug);
    labelInit(arrCloseTextR, ppartInfo->ArrCloseTextR, LABEL_TYPE_IMAGE, imageDir + "ui_arr/DOOR/closeText.png", ARR, "NULL", false);

    arrCloseMovie = new QMovie();
}

/*init display*/
void MainWindow::initDipaly(public_info_t *ppublic, all_info_t *pallInfo, part_info_t *ppartInfo)
{
    int i;
    QString str;
    QString displayPath = "/root/config/display.ini";
    QSettings settings_disp(displayPath, QSettings::IniFormat);
    settings_disp.setIniCodec("UTF8");
    /*###### 1.公共控件参数初始化 #######################################################################################################*/
    publicInit(ppublic);
    /*###### 2.ALL界面控件参数初始化 #######################################################################################################*/
    allInit(pallInfo);
    /*###### 3. 局部界面控件初始化 #######################################################################################################*/
    preInit(ppartInfo);
    /*###### 4.到站界面控件参数初始化 #######################################################################################################*/
    arrInit(ppartInfo);
    /*###### 5.启动界面显示参数初始化 #######################################################################################################*/
    versionLabel.setParent(&widget[INFODISP]);
    versionLabel.setGeometry(200, 40, 500, 50);
    carIdLabel.setParent(&widget[INFODISP]);
    carIdLabel.setGeometry(200, 100, 500, 50);
    devIdLabel.setParent(&widget[INFODISP]);
    devIdLabel.setGeometry(500, 100, 500, 50);
    ipLabel.setParent(&widget[INFODISP]);
    ipLabel.setGeometry(200, 160, 500, 50);
    ipConflictLabel.setParent(&widget[INFODISP]);
    ipConflictLabel.setGeometry(300, 160, 500, 50);
    macLabel.setParent(&widget[INFODISP]);
    macLabel.setGeometry(500, 160, 500, 50);
    setFont(versionLabel, "Oswald", false, QFont::Normal, 34, 200, 200, 200);
    versionLabel.setAlignment(Qt::AlignLeft);
    setFont(carIdLabel, "Oswald", false, QFont::Normal, 34, 200, 200, 200);
    carIdLabel.setAlignment(Qt::AlignLeft);
    setFont(devIdLabel, "Oswald", false, QFont::Normal, 34, 200, 200, 200);
    devIdLabel.setAlignment(Qt::AlignLeft);
    setFont(ipLabel, "Oswald", false, QFont::Normal, 34, 200, 200, 200);
    ipLabel.setAlignment(Qt::AlignLeft);
    setFont(ipConflictLabel, "Oswald", false, QFont::Normal, 34, 200, 200, 200);
    ipConflictLabel.setAlignment(Qt::AlignLeft);
    setFont(macLabel, "Oswald", false, QFont::Normal, 34, 200, 200, 200);
    macLabel.setAlignment(Qt::AlignLeft);
    //    QString str;
    str = "Ip: " + mytimer.ipStr;
    ipLabel.setText(str);
    str = QString("%1%2%3%4").arg("version: ").arg(VERSION_MAX).arg(".").arg(VERSION_MIN);
    versionLabel.setText(str);
    str = "Mac: " + macStr;
    macLabel.setText(str);
    int carid = (ip4 - 1) / 10 + 1;
    int carno = (ip4 - 1) % 10 + 1;
    QString caridStr = QString("%1%2%3%4").arg("Car: ").arg(carid).arg("        Num: ").arg(carno);
    carIdLabel.setText(caridStr);
    int dev = (carid - 1) * 8 + carno;
    QString devidStr = QString("%1%2").arg("Dev: ").arg(dev);
    devIdLabel.setText(devidStr);
    setInfo();
    /*###### 6.颜色显示参数初始化 #######################################################################################################*/
    holeDisp.setParent(&widget[COLORDISP]);
    holeDisp.setGeometry(0, 0, 1920, 360);
    holeDisp.setAutoFillBackground(true);

    for (i = 0; i < 16; i++)
    {
        grayScale16[i].setParent(&widget[COLORDISP]);
        grayScale16[i].setGeometry((1920 / 16)*i, 0, 1920 / 16, 360);
        grayScale16[i].setAutoFillBackground(true);
    }
}

void MainWindow::InitEnv()
{
    qDebug() << "InitEnv";
    /*(1)init Socket*/
    connect(&mycontroludp, SIGNAL(running_trigger_signal(BYTE, BYTE, BYTE, BYTE, BYTE, bool)), this, SLOT(running_trigger(BYTE, BYTE, BYTE, BYTE, BYTE, bool)));
    connect(&mycontroludp, SIGNAL(setDisplayType(BYTE)), this, SLOT(MainDisplayType(BYTE)));
    connect(&this->mycontroludp, SIGNAL(setSkip(BYTE *, int)), this, SLOT(setSkip(BYTE *, int)));
    connect(&this->mycontroludp, SIGNAL(setMirror(bool)), this, SLOT(setMirror(bool)));
    connect(&mycontroludp, SIGNAL(setColorTest(BYTE , BYTE , BYTE )), this, SLOT(setColorTest(BYTE , BYTE , BYTE )));
#if 1
    psysTimer = new QTimer(this);
    connect(psysTimer, SIGNAL(timeout()), this, SLOT(sysproc()));
    psysTimer->start(1000);
#endif
    connect(&mytimer, SIGNAL(timeOut20ms()), this, SLOT(TimeoutProc()));
    connect(&mytimer, SIGNAL(timeOut1000ms()), &mycontroludp, SLOT(mainSendProc()));
    mytimer.stopFlag = 0;
    mytimer.startFlag = 0;
    connect(&mytimer2, SIGNAL(timeOut20ms()), this, SLOT(TimeoutProc()));
    connect(&mytimer2, SIGNAL(timeOut1000ms()), &mycontroludp, SLOT(mainSendProc()));
    mytimer2.stopFlag = 0;
    mytimer2.startFlag = 0;
    connect(&mytimer3, SIGNAL(timeOut20ms()), this, SLOT(TimeoutProc()));
    connect(&mytimer3, SIGNAL(timeOut1000ms()), &mycontroludp, SLOT(mainSendProc()));
    mytimer3.stopFlag = 0;
    mytimer3.startFlag = 0;
    weeklist << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
    weekchlist << "星期一" << "星期二" << "星期三" << "星期四" << "星期五" << "星期六" << "星期日";
    run_st = SET_IDLE;
    leaveSt = LEAVE_ST_OFF;
    arrToLeaveFlag = 0;
    imageInit();
}

void MainWindow::imageInit()
{
    //int y;
    //局部界面初始化动效
    //    for(int i=0;i<gPartInfo.arrOpenFrameNum;i++)  // 到站开门
    //    {
    //        arrOpenPixmap[i]=new QPixmap;
    //        y = i*gPartInfo.ArrOpenDoor.rectUp.h;
    //        *arrOpenPixmap[i]=QPixmap("/root/media/images/ui_pre/DOOR/arriveOpen.png").copy(0,y,gPartInfo.ArrOpenDoor.rectUp.w,gPartInfo.ArrOpenDoor.rectUp.h);
    //    }
    qDebug() << "==>image init finish";
}

void MainWindow::chgToWidget(int index)
{
    for (int i = 0; i < WIDGET_NUM; i++)
    {
        if (i == index)
        {
            widgetIndex = i;
            widget[i].show();
        }
        else
            widget[i].hide();
    }
}


void MainWindow::setFont(QLabel &label, QString name, bool bold, int weight, int size, int r, int g, int b)
{
    //瀛椾綋璁剧疆
    QFont font;
    QColor color;//璺崇珯
    QPalette palette;
    font.setPixelSize(size);
    font.setBold(bold);
    font.setFamily(name);
    //    font.setWeight(weight);
    color.setRgb(r, g, b);
    palette.setColor(QPalette::WindowText, color);
    label.setPalette(palette);
    //    label.setAlignment(Qt::AlignCenter);
    //    label.setWordWrap(true);
    //    label.setLineWidth(1);
    label.setFont(font);
}

bool MainWindow::checkStation(int start, int current, int next, int end, int station1, int station2)
{
    if (start == end)
    {
        qDebug() << "------------station false------------";
        return false;
    }
    else if (start >= station1 && start <= current && current <= next && next <= end && end <= station2)
    {
        return true;
    }
    else if (start <= station2 && start >= current && current >= next && next >= end && end >= station1)
    {
        return true;
    }
    else
    {
        qDebug() << "------------station false------------";
        return false;
    }
}


void MainWindow::setUpdate(int update)
{
    chgToWidget(WELCOME);
    updateLable.show();
    welcomeLable.hide();

    if (update)
        updateLable.setText("正在升级程序，请等待！");
    else
        updateLable.setText("升级成功");
}

void MainWindow::setInfo()
{
    chgToWidget(INFODISP);
}

void MainWindow::setColorTest(BYTE mode, BYTE time, BYTE index)
{
    cycleMode = mode;

    if (time > 0)
        cycleTime = time;

    if ((index == 0) || (index == 0xff))
        cycleIndex = 0xff;
    else
        cycleIndex = index - 1;

    if (mode == SET_CYCLE_TEST_ON)
    {
        chgToWidget(COLORDISP);
    }
    else
        setWelcome();

    qDebug() << "color cycle test";
    qDebug() << "mode:" << cycleMode;
    qDebug() << "time:" << cycleTime;
    qDebug() << "index:" << cycleIndex;
}


void MainWindow::setWelcome()
{
    chgToWidget(WELCOME);
    welcomeLable.show();
    //    qDebug()<<"chg welcome";
    updateLable.hide();
    //   allProc(1,2,3,32,doorFlag,SET_LEAVE,movieIndex);
}

void MainWindow::setSkip(BYTE *buf, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        skipStation[i] = buf[i];
    }
}

void MainWindow::setMirror(bool mirror)
{
    mirrorFlag = mirror;
    qDebug() << "mirrorFlag:" << mirrorFlag;
    //    mirrorFlag = false;
}

void MainWindow::ipConflictProc(int flag)
{
    if (flag)
    {
        chgToWidget(WELCOME);
        updateLable.show();
        welcomeLable.hide();
        QString str = "IP: " + mytimer.ipStr + " Conflict";
        updateLable.clear();
        updateLable.setText(str);
    }
    else
    {
        setWelcome();
    }
}

void MainWindow::TimeoutProc()
{
    static int opentoleave = 0;
    static int frameIndex = 0;
    QPixmap p;

    if (leaveSt == LEAVE_ST_ARR) // 到站
    {
        arrToLeaveFlag = 1;
        opentoleave = 1;
        return;
    }

    leave_chg_time++;

    if (leave_chg_time % 6 != 1) // 20ms 中断一次，n?ms更新动画
        return;

    if (leaveSt == LEAVE_ST_ALL)
    {
        if (arrToLeaveFlag)
        {
            if (opentoleave)
            {
                arrOpenMovie->stop();       // door flash
                arrOpenFlash.setMovie(arrOpenMovie);
                arrOpenMovie->setFileName(imageDir + "ui_arr/DOOR/openToClose.gif");
                arrOpenMovie->setSpeed(100);
                arrOpenMovie->start();
                opentoleave = 0;
                frameIndex = 0;
                arrStationNameCh.hide();
                arrStationNameEn.hide();

                arrCloseTextL.hide();
                arrCloseTextR.hide();

                arrOpenTextCh[0].hide();
                arrOpenTextCh[1].hide();
                arrOpenTextEn[0].hide();
                arrOpenTextEn[1].hide();

                arrOpenChgImg.hide();
                arrOpenChgLine.hide();
                arrCloseChgImg.hide();
                arrCloseChgLine.hide();
            }

            //            qDebug()<<"c:"<<frameIndex;
            if (frameIndex >= arrOpenMovie->frameCount() - 1) //关门动画帧完毕
            {
                qDebug() << "openToClose flash play over";
                arrToLeaveFlag = 0;
                allProc(startId, currentId, nextId, endId, doorFlag, SET_LEAVE, movieIndex);
            }
            else
                frameIndex = arrOpenMovie->currentFrameNumber();

            return;
        }
    }
    else if (leaveSt == LEAVE_ST_PRE)
    {
    }
    else
    {
    }

    if (leaveSt && (leave_chg_time > 495) && (widgetIndex >= ALL))
    {
        if (currentId == nextId)
        {
            leave_chg_time = 0;
        }
        else if (leaveSt == LEAVE_ST_PRE)
        {
            leaveSt = LEAVE_ST_ALL;
            leave_chg_time = 0;
            allProc(startId, currentId, nextId, endId, doorFlag, SET_LEAVE, movieIndex);
            qDebug() << "time to chg to all";
        }
        else if ((leaveSt == LEAVE_ST_ALL) && (leave_chg_time > 750))
        {
            leaveSt = LEAVE_ST_PRE;
            leave_chg_time = 0;
            preProc(startId, currentId, nextId, endId, doorFlag, SET_LEAVE, movieIndex);
            qDebug() << "time to chg to pre";
        }
        else {}
    }

    //    preProc(startId,currentId,nextId,endId,doorFlag,SET_LEAVE,movieIndex);
}

void MainWindow::sysproc()
{
    static unsigned int info_time = 0;
    static unsigned int colorDispTime = 0;
    QString wstr, dstr, temp;
    wstr = QDateTime::currentDateTime().toString("dddd");
    static int life1 = 0, life2 = 0, life3 = 0;
    static int cnt1 = 0, cnt2 = 0, cnt3 = 0;

    if (++info_time < 4)
        return;
    else
    {
        if (info_time >= 5)
            info_time = 5;
        else
        {
            if (run_st == SET_IDLE)
                setWelcome();

            cnt1 = 0;
            mytimer.startFlag = 1;
            mytimer.start();
        }
    }

    if (widgetIndex == COLORDISP)
    {
        if ((colorDispTime % cycleTime == 0) || (cycleIndex != 0xff))
        {
            QColor color;
            QPalette palette;
            holeDisp.hide();

            for (int i = 0; i < 16; i++)
                grayScale16[i].hide();

            for (int i = 0; i < 32; i++)
                grayScale32[i].hide();

            for (int i = 0; i < 64; i++)
                grayScale64[i].hide();

            if (cycleIndex != 0xff)
                colorDispIndex = cycleIndex;

            switch (colorDispIndex % 7)
            {
                case 0:
                    color.setRgb(255, 0, 0);
                    palette.setColor(QPalette::Background, color);
                    holeDisp.setPalette(palette);
                    holeDisp.show();
                    //qDebug()<<"---red disp---";
                    break;

                case 1:
                    color.setRgb(0, 255, 0);
                    palette.setColor(QPalette::Background, color);
                    holeDisp.setPalette(palette);
                    holeDisp.show();
                    //qDebug()<<"---green disp---";
                    break;

                case 2:
                    color.setRgb(0, 0, 255);
                    palette.setColor(QPalette::Background, color);
                    holeDisp.setPalette(palette);
                    holeDisp.show();
                    //qDebug()<<"---blue disp---";
                    break;

                case 3:
                    color.setRgb(0, 0, 0);
                    palette.setColor(QPalette::Background, color);
                    holeDisp.setPalette(palette);
                    holeDisp.show();
                    //qDebug()<<"---black disp---";
                    break;

                case 4:
                    color.setRgb(128, 128, 128);
                    palette.setColor(QPalette::Background, color);
                    holeDisp.setPalette(palette);
                    holeDisp.show();
                    //qDebug()<<"---gray m disp---";
                    break;

                case 5:
                    color.setRgb(255, 255, 255);
                    palette.setColor(QPalette::Background, color);
                    holeDisp.setPalette(palette);
                    holeDisp.show();
                    //qDebug()<<"---white disp---";
                    break;

                case 6:
                    for (int i = 0; i < 16; i++)
                    {
                        color.setRgb(i * 256 / 16, i * 256 / 16, i * 256 / 16);
                        palette.setColor(QPalette::Background, color);
                        grayScale16[i].setPalette(palette);
                        grayScale16[i].show();
                    }

                    //qDebug()<<"---gray scale16 disp---";
                    break;

                case 7:
                    for (int i = 0; i < 32; i++)
                    {
                        color.setRgb(i * 256 / 32, i * 256 / 32, i * 256 / 32);
                        palette.setColor(QPalette::Background, color);
                        grayScale32[i].setPalette(palette);
                        grayScale32[i].show();
                    }

                    qDebug() << "---gray scale32 disp---";
                    break;

                case 8:
                    for (int i = 0; i < 64; i++)
                    {
                        color.setRgb(i * 256 / 64, i * 256 / 64, i * 256 / 64);
                        palette.setColor(QPalette::Background, color);
                        grayScale64[i].setPalette(palette);
                        grayScale64[i].show();
                    }

                    qDebug() << "---gray scale64 disp---";
                    break;

                default:
                    break;
            }

            colorDispIndex++;
        }

        colorDispTime++;
    }

    for (int i = 0; i < weeklist.size(); i++)
    {
        temp = weeklist.at(i);

        if (QString::compare(wstr, temp) == 0)
        {
            dstr = weekchlist.at(i);
            break;
        }
    }

    QString h = QDateTime::currentDateTime().toString("hh");
    QString m = QDateTime::currentDateTime().toString("mm");
    //    QString stime = h.mid(0,1)+h.mid(1,1)+" : "+m.mid(0,1)+" "+m.mid(1,1);
    wstr = "/" + QDateTime::currentDateTime().toString("ddd");
    dateLabel.setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    timeLabel.setText(h + ":" + m);
    weekChLabel.setText(dstr);
    weekEnLabel.setText(wstr);
    preDateLabel.setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    preTimeLabel.setText(h + ":" + m);
    preWeekChLabel.setText(dstr);
    preWeekEnLabel.setText(wstr);
    arrDateLabel.setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    arrTimeLabel.setText(h + ":" + m);
    arrWeekChLabel.setText(dstr);
    arrWeekEnLabel.setText(wstr);

    //定时器生命信号处理
    if (life1 != mytimer.life)
        cnt1 = 0;

    if (life2 != mytimer2.life)
        cnt2 = 0;

    if (life3 != mytimer3.life)
        cnt3 = 0;

    life1 = mytimer.life;
    life2 = mytimer2.life;
    life3 = mytimer3.life;

    if ((++cnt1 > 5) && (mytimer.startFlag > 0))
    {
        mytimer.startFlag = 0;
        mytimer.stopFlag = 1;
        cnt2 = 0;
        mytimer2.startFlag = 2;
        mytimer2.start();
        qDebug() << "drmd==>mytimer stop && mytimer2 start";
    }

    if ((++cnt2 > 5) && (mytimer2.startFlag > 0))
    {
        mytimer2.startFlag = 0;
        mytimer2.stopFlag = 1;
        cnt3 = 0;
        mytimer3.startFlag = 3;
        mytimer3.start();
        qDebug() << "drmd==>mytimer2 stop && mytimer3 start";
    }

    if ((++cnt3 > 5) && (mytimer3.startFlag > 0))
    {
        mytimer3.startFlag = 0;
        mytimer3.stopFlag = 1;
        qDebug() << "drmd==>mytimer3 stop";
        this->close();
    }
}

void MainWindow::movieChgProc()
{
}


void MainWindow::MainDisplayType(BYTE type)
{
    run_st = type;

    if (widgetIndex == COLORDISP)
        return;

    switch (type)
    {
        case SET_OPEN:
            break;

        case SET_CLOSE:
            break;

        //case SET_EME:
        //    break;
        case SET_NOSIGNAL:
            setWelcome();
            break;

        case SET_CLOSE_LCD:
            break;

        case SET_UPDATE_ING:
            updateFlag = 1;
            setUpdate(1);
            break;

        case SET_UPDATE_OK:
            updateFlag = 0;
            setUpdate(0);
            break;

        case SET_UPDATE_ERROR:
            break;

        case SET_IP_CONFLICT_ON:
            ipConflictProc(1);
            break;

        case SET_IP_CONFLICT_OFF:
            ipConflictProc(0);
            break;

        default:
            break;
    }
}


void MainWindow::running_trigger(BYTE type, BYTE start, BYTE current, BYTE next, BYTE end, bool door)
{
    //    if((false==checkStation(start,current,next,end,1,DEFINE_STATION_NUM))||(updateFlag))
    //    {
    //        return;
    //    }
    if (widgetIndex == COLORDISP)
        return;

    movieIndex = 1;
    startId = start;
    endId = end;
    currentId = current;
    nextId = next;
    doorFlag = door;
    qDebug() << "running_trigger station:" << startId << currentId << nextId << endId << "cmd type" << type;
    run_st = type;

    switch (type)
    {
        case SET_LEAVE:
            allProc(start, current, next, end, door, SET_LEAVE, movieIndex);
            //            preProc(start,current,next,end,door,SET_PRE,movieIndex);
            leaveSt = LEAVE_ST_ALL;
            leave_chg_time = 0;
            break;

        case SET_PRE:
            leaveSt = LEAVE_ST_PRE;
            preProc(start, current, next, end, door, SET_PRE, movieIndex);
            break;

        case SET_ARRIVED:
            leaveSt = LEAVE_ST_ARR;
            arrProc(start, current, next, end, door, SET_ARRIVED, movieIndex);
            break;

        case SET_DOOR_ERROR:
            break;

        case SET_DOOR_OK:
            break;

        default:
            break;
    }
}

void MainWindow::allProc(int start, int current, int next, int end, bool door, int cmd, int index)
{
    QString filename;
    QString str;
    QString site_str;
    QString track_str;
    int runDir = 0;
    int i; // 站名索引
    int ST_StationName[StationMaxNumber]; // 各个站状态
    int ST_Site[StationMaxNumber];
    int ST_Track[StationMaxNumber];
    int grayNum;

    //    if(checkStation(start,current,next,end,1,StationMaxNumber)==false)
    //        return;

    if ((current > StationMaxNumber) || (next > StationMaxNumber)
        || ((end != 98) && (end != 99) && (end > StationMaxNumber)))
        return;

    if (arrToLeaveFlag == 1)
        return;

    qDebug() << "---allProc---";

    //内外环判断  1上行内环 0 下行外环
    if (end == 98)
    {
        runDir = 1;
        gRunDir = runDir;
    }
    else if (end == 99)
    {
        runDir = 0;
        gRunDir = runDir;
    }
    else if (end <= 32)
    {
        // 上行内环
        if ((current < next) && (next < end))   //1 c<n<e
        {
            runDir = 1;
            gRunDir = runDir;
        }
        else if ((end < current) && (current < next)) //2 e<c<n
        {
            runDir = 1;
            gRunDir = runDir;
        }
        else if ((next < end) && (end < current)) //3 n<e<c
        {
            runDir = 1;
            gRunDir = runDir;
        }
        // 下行外环
        else if ((current > next) && (next > end)) //4 c>n>e
        {
            runDir = 0;
            gRunDir = runDir;
        }
        else if ((end > current) && (current > next)) //5 e>c>n
        {
            runDir = 0;
            gRunDir = runDir;
        }
        else if ((next > end) && (end > current)) //6 n>e>c
        {
            runDir = 0;
            gRunDir = runDir;
        }
        else
        {
            if ((current == next) || (end == next)) // 环线无法判断上下行，所以不处理
                runDir = gRunDir;
        }
    }
    else
    {
        return;
    }

    if (runDir)
        allMRundir.setPixmap(imageDir + "menu/runIn.png");
    else
        allMRundir.setPixmap(imageDir + "menu/runOut.png");

    allMNextSNCh.setText(stationChName[next - 1]);
    allMNextSNCh.adjustSize();
    allMNextSNCh.setGeometry(gPublicInfo.MNextSNCh.rectUp.x, gPublicInfo.MNextSNCh.rectUp.y, allMNextSNCh.width(), gPublicInfo.MNextSNCh.rectUp.h);

    if (stationEnDispLines[next - 1] == 2)
        allMNextSNEn.setText(stationEnName[next - 1] + stationEnName2[next - 1]);
    else if (stationEnDispLines[next - 1] == 20)
        allMNextSNEn.setText(stationEnName[next - 1] + " " + stationEnName2[next - 1]);
    else
        allMNextSNEn.setText(stationEnName[next - 1]);

    allMNextSNEn.adjustSize();
    allMNextSNEn.setGeometry(gPublicInfo.MNextSNCh.rectUp.x + allMNextSNCh.width() + 10, gPublicInfo.MNextSNEn.rectUp.y, allMNextSNEn.width(), gPublicInfo.MNextSNEn.rectUp.h);
    //1. 动图显示处理 Flash disp
    allSiteMovie->stop();       // site flash
    allSiteFlash.setGeometry(gAllInfo.site[next - 1].rectUp.x - (gAllInfo.siteFlash.rectUp.w - gAllInfo.site[next - 1].rectUp.w) / 2, gAllInfo.site[next - 1].rectUp.y - (gAllInfo.siteFlash.rectUp.h - gAllInfo.site[next - 1].rectUp.h) / 2, gAllInfo.siteFlash.rectUp.w, gAllInfo.siteFlash.rectUp.h);
    allSiteFlash.setMovie(allSiteMovie);
    allSiteMovie->setFileName(allSitePath + "next.gif");
    allSiteMovie->setSpeed(100);
    allSiteMovie->start();

    if (current == next)
    {
        allSiteFlash.hide();
    }
    else
    {
        allSiteFlash.show();
    }

    int trackId = 0;            // track flash

    if (runDir) // 上行 内环
    {
        if (next == 1)
            trackId = StationMaxNumber - 1;
        else
            trackId = next - 2;

        if (trackId < 15)
            filename = QString("%1%2").arg(allTrackPath).arg("upIn.gif");
        else if (trackId == 15)
            filename = QString("%1%2").arg(allTrackPath).arg("rightIn.gif");
        else if (trackId < 31)
            filename = QString("%1%2").arg(allTrackPath).arg("downIn.gif");
        else if (trackId == 31)
            filename = QString("%1%2").arg(allTrackPath).arg("leftIn.gif");
        else {}
    }
    else
    {
        trackId = next - 1;

        if (trackId < 15)
            filename = QString("%1%2").arg(allTrackPath).arg("upOut.gif");
        else if (trackId == 15)
            filename = QString("%1%2").arg(allTrackPath).arg("rightOut.gif");
        else if (trackId < 31)
            filename = QString("%1%2").arg(allTrackPath).arg("downOut.gif");
        else if (trackId == 31)
            filename = QString("%1%2").arg(allTrackPath).arg("leftOut.gif");
        else {}
    }

    allTrackMovie->stop();       // track flash

    if (trackId == 31)
        allTrackFlash.setGeometry(gAllInfo.trackFlash[0].rectUp.x, gAllInfo.trackFlash[0].rectUp.y, gAllInfo.trackFlash[0].rectUp.w, gAllInfo.trackFlash[0].rectUp.h);
    else if (trackId == 15)
        allTrackFlash.setGeometry(gAllInfo.trackFlash[1].rectUp.x, gAllInfo.trackFlash[1].rectUp.y, gAllInfo.trackFlash[1].rectUp.w, gAllInfo.trackFlash[1].rectUp.h);
    else
        allTrackFlash.setGeometry(gAllInfo.track[trackId].rectUp.x, gAllInfo.track[trackId].rectUp.y, gAllInfo.track[trackId].rectUp.w, gAllInfo.track[trackId].rectUp.h);

    allTrackFlash.setMovie(allTrackMovie);
    allTrackMovie->setFileName(filename);
    allTrackMovie->setSpeed(100);
    allTrackMovie->start();
    allRDLeftMovie->stop();       // rundir flash
    allRunDirLeft.setMovie(allRDLeftMovie);

    if (runDir)
        filename = imageDir + "ui_all/OTHER/runDirIn.gif";
    else
        filename = imageDir + "ui_all/OTHER/runDirOut.gif";

    allRDLeftMovie->setFileName(filename);
    allRDLeftMovie->setSpeed(100);
    allRDLeftMovie->start();
    allRDRightMovie->stop();
    allRunDirRight.setMovie(allRDRightMovie);
    allRDRightMovie->setFileName(filename);
    allRDRightMovie->setSpeed(100);
    allRDRightMovie->start();

    for (int i = 0; i < stationNum; i++)
    {
        allStart[i]->hide();
        allEnd[i]->hide();
    }

    if (end <= stationNum)
        allEnd[end - 1]->show();

    // 1. 状态处理
    if (runDir) // 1.1 上行 内环 逻辑判断处理  ***********************************
    {
        for (i = 0; i < StationMaxNumber; i++) // 站名
        {
            ST_StationName[i] = ST_NEXTN;
            ST_Site[i] = ST_NEXTN;

            if (i > 0)
                ST_Track[i - 1] = ST_NEXTN;
            else
                ST_Track[StationMaxNumber - 1] = ST_NEXTN;

            if (end == 98) // (1)环线运行
            {
                if (current <= next)
                {
                    //qDebug()<<"all==> end==98 current<next";
                    if (((i + 1) > current) && ((i + 1) < next))
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;

                        if (i > 0)
                            ST_Track[i - 1] = ST_SKIP;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_SKIP;
                    }
                    else
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;

                        if (i > 0)
                            ST_Track[i - 1] = ST_NEXTN;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                    }
                }
                else  // 当前站大于下一站时
                {
                    //qDebug()<<"all==> end==98 current>next";
                    if (((i + 1) > next) && ((i + 1) < current))
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;

                        if (i > 0)
                            ST_Track[i - 1] = ST_NEXTN;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                    }
                    else
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;

                        if (i > 0)
                            ST_Track[i - 1] = ST_SKIP;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_SKIP;
                    }
                }
            }
            else    // (2) 有终点站
            {
                if ((current < next) && (next < end))   //(2.1) c<n<e
                {
                    //qDebug()<<"all==> (2.1) c<n<e";
                    if (((i + 1) > current) && ((i + 1) < next))
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;

                        if (i > 0)
                            ST_Track[i - 1] = ST_SKIP;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_SKIP;
                    }
                    else if (((i + 1) > next) && ((i + 1) <= end))
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;

                        if (i > 0)
                            ST_Track[i - 1] = ST_NEXTN;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                    }
                    else
                    {
                        ST_StationName[i] = ST_PASS;
                        ST_Site[i] = ST_PASS;

                        if (i > 0)
                            ST_Track[i - 1] = ST_PASS;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_PASS;
                    }
                }
                else if ((end < current) && (current < next)) //(2.2) e<c<n
                {
                    //qDebug()<<"all==> (2.2) e<c<n";
                    if (((i + 1) > current) && ((i + 1) < next))
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;

                        if (i > 0)
                            ST_Track[i - 1] = ST_SKIP;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_SKIP;
                    }
                    else if (((i + 1) > end) && ((i + 1) < current))
                    {
                        ST_StationName[i] = ST_PASS;
                        ST_Site[i] = ST_PASS;

                        if (i > 0)
                            ST_Track[i - 1] = ST_PASS;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_PASS;
                    }
                    else
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;

                        if (i > 0)
                            ST_Track[i - 1] = ST_NEXTN;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                    }
                }
                else if ((next < end) && (end < current)) //(2.3) n<e<c
                {
                    //qDebug()<<"all==> (2.3) n<e<c";
                    if (((i + 1) > end) && ((i + 1) < current))
                    {
                        ST_StationName[i] = ST_PASS;
                        ST_Site[i] = ST_PASS;

                        if (i > 0)
                            ST_Track[i - 1] = ST_PASS;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_PASS;
                    }
                    else if (((i + 1) > next) && ((i + 1) <= end))
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;

                        if (i > 0)
                            ST_Track[i - 1] = ST_NEXTN;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                    }
                    else
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;

                        if (i > 0)
                            ST_Track[i - 1] = ST_SKIP;
                        else
                            ST_Track[StationMaxNumber - 1] = ST_SKIP;
                    }
                }
                else     //(2.4) n==e || n==c
                {
                    //qDebug()<<"all==> (2.4) n==e || n==c";
                    if (next == end)
                    {
                        if (next >= current) // n>=c
                        {
                            if (((i + 1) > current) && ((i + 1) < next))
                            {
                                ST_StationName[i] = ST_SKIP;
                                ST_Site[i] = ST_SKIP;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_SKIP;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_SKIP;
                            }
                            else
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_PASS;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_PASS;
                            }
                        }
                        else    // n<c
                        {
                            if (((i + 1) > next) && ((i + 1) < current))
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_PASS;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_PASS;
                            }
                            else
                            {
                                ST_StationName[i] = ST_SKIP;
                                ST_Site[i] = ST_SKIP;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_SKIP;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_SKIP;
                            }
                        }
                    }
                    else if (next == current)
                    {
                        if (next < end)
                        {
                            if (((i + 1) > next) && ((i + 1) <= end))
                            {
                                ST_StationName[i] = ST_NEXTN;
                                ST_Site[i] = ST_NEXTN;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_NEXTN;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                            }
                            else
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_PASS;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_PASS;
                            }
                        }
                        else
                        {
                            if (((i + 1) > end) && ((i + 1) < next))
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_PASS;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_PASS;
                            }
                            else
                            {
                                ST_StationName[i] = ST_NEXTN;
                                ST_Site[i] = ST_NEXTN;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_NEXTN;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_NEXTN;
                            }
                        }
                    }
                    else {}
                }
            }
        }

        if (end == 98) // 环线运行
        {
            grayNum = 5;

            for (i = 1; i <= grayNum; i++) // 当前5个站前PASS
            {
                if (current == next)
                {
                    if (current >= i + 1)
                    {
                        if (i < grayNum)
                            ST_Site[current - i - 1] = ST_PASS;

                        ST_Track[current - i - 1] = ST_PASS;
                        //qDebug()<<"In 5 x1 : i="<<i;
                    }
                    else if (current < i + 1)
                    {
                        if (i < grayNum)
                            ST_Site[StationMaxNumber + current - i - 1] = ST_PASS;

                        ST_Track[StationMaxNumber + current - i - 1] = ST_PASS;
                        //qDebug()<<"In 5 x2 : i="<<i;
                    }
                }
                else if (current >= i + 1)
                {
                    if (current > next)
                    {
                        if (current - i >= next)
                        {
                            if (i < grayNum)
                                ST_Site[current - i - 1] = ST_PASS;

                            ST_Track[current - i - 1] = ST_PASS;
                            //qDebug()<<"In 5 x3 : i="<<i;
                        }
                    }
                    else
                    {
                        if (i < grayNum)
                            ST_Site[current - i - 1] = ST_PASS;

                        ST_Track[current - i - 1] = ST_PASS;
                        //qDebug()<<"In 5 x4 : i="<<i;
                    }
                }
                else if (current < i + 1)
                {
                    if ((next > current) && (StationMaxNumber + current - i >= next))
                    {
                        if (i < grayNum)
                            ST_Site[StationMaxNumber + current - i - 1] = ST_PASS;

                        ST_Track[StationMaxNumber + current - i - 1] = ST_PASS;
                        //qDebug()<<"In 5 x5: i="<<i;
                    }
                }
                else {}
            }
        }

        for (i = 0; i < StationMaxNumber; i++) // 当前站和下一站
        {
            if ((i + 1) == next)
            {
                ST_StationName[i] = ST_NEXT;
                ST_Site[i] = ST_NEXT;

                if (i > 0)
                    ST_Track[i - 1] = ST_NEXT;
                else
                    ST_Track[StationMaxNumber - 1] = ST_NEXT;
            }

            if ((i + 1) == current)
            {
                ST_StationName[i] = ST_CURRENT;
                ST_Site[i] = ST_CURRENT;

                if (i > 0)
                    ST_Track[i - 1] = ST_PASS;
                else
                    ST_Track[StationMaxNumber - 1] = ST_PASS;
            }
            else {}
        }

        qDebug() << "up end";
    }
    else  // 1.2 下行 逻辑判断处理 ***************************************
    {
        for (i = 0; i < StationMaxNumber; i++) // 站名
        {
            ST_StationName[i] = ST_NEXTN;
            ST_Site[i] = ST_NEXTN;
            ST_Track[i] = ST_NEXTN;

            if (end == 99) // (1)环线运行
            {
                if (current < next)
                {
                    //qDebug()<<"all==> end==99 current<next";
                    if (((i + 1) > current) && ((i + 1) < next))
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;
                        ST_Track[i] = ST_NEXTN;
                    }
                    else
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;
                        ST_Track[i] = ST_SKIP;
                    }
                }
                else  // 当前站大于下一站时
                {
                    //qDebug()<<"all==> end==99 current>next";
                    if (((i + 1) > next) && ((i + 1) < current))
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;
                        ST_Track[i] = ST_SKIP;
                    }
                    else
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;
                        ST_Track[i] = ST_NEXTN;
                    }
                }
            }
            else    // (2) 有终点站
            {
                if ((current > next) && (next > end))   //(2.5) c>n>e
                {
                    //qDebug()<<"all==> (2.5) c>n>e";
                    if (((i + 1) > next) && ((i + 1) < current))
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;
                        ST_Track[i] = ST_SKIP;
                    }
                    else if (((i + 1) < next) && ((i + 1) >= end))
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;
                        ST_Track[i] = ST_NEXTN;
                    }
                    else
                    {
                        ST_StationName[i] = ST_PASS;
                        ST_Site[i] = ST_PASS;
                        ST_Track[i] = ST_PASS;
                    }
                }
                else if ((end > current) && (current > next)) //(2.6) e>c>n
                {
                    //qDebug()<<"all==> (2.6) e>c>n";
                    if (((i + 1) > next) && ((i + 1) < current))
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;
                        ST_Track[i] = ST_SKIP;
                    }
                    else if (((i + 1) < end) && ((i + 1) > current))
                    {
                        ST_StationName[i] = ST_PASS;
                        ST_Site[i] = ST_PASS;
                        ST_Track[i] = ST_PASS;
                    }
                    else
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;
                        ST_Track[i] = ST_NEXTN;
                    }
                }
                else if ((next > end) && (end > current)) //(2.7) n>e>c
                {
                    //qDebug()<<"all==> (2.7) n>e>c";
                    if (((i + 1) < end) && ((i + 1) > current))
                    {
                        ST_StationName[i] = ST_PASS;
                        ST_Site[i] = ST_PASS;
                        ST_Track[i] = ST_PASS;
                        //qDebug()<<"i="<<i;
                    }
                    else if (((i + 1) < next) && ((i + 1) >= end))
                    {
                        ST_StationName[i] = ST_NEXTN;
                        ST_Site[i] = ST_NEXTN;
                        ST_Track[i] = ST_NEXTN;
                    }
                    else
                    {
                        ST_StationName[i] = ST_SKIP;
                        ST_Site[i] = ST_SKIP;
                        ST_Track[i] = ST_SKIP;
                    }
                }
                else                                    //(2.8) n==e || n==c
                {
                    //qDebug()<<"all==> (2.8) n==e || n==c";
                    if (next == end)
                    {
                        if (next > current) // c<n
                        {
                            if (((i + 1) > current) && ((i + 1) < next))
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_PASS;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_PASS;
                            }
                            else
                            {
                                ST_StationName[i] = ST_SKIP;
                                ST_Site[i] = ST_SKIP;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_SKIP;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_SKIP;
                            }
                        }
                        else    // c>=n
                        {
                            if (((i + 1) > next) && ((i + 1) < current))
                            {
                                ST_StationName[i] = ST_SKIP;
                                ST_Site[i] = ST_SKIP;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_SKIP;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_SKIP;
                            }
                            else
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;

                                if (i > 0)
                                    ST_Track[i - 1] = ST_PASS;
                                else
                                    ST_Track[StationMaxNumber - 1] = ST_PASS;
                            }
                        }
                    }
                    else if (next == current)
                    {
                        if (next >= end)
                        {
                            if (((i + 1) < next) && ((i + 1) >= end))
                            {
                                ST_StationName[i] = ST_NEXTN;
                                ST_Site[i] = ST_NEXTN;
                                ST_Track[i] = ST_NEXTN;
                            }
                            else
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;
                                ST_Track[i] = ST_PASS;
                            }
                        }
                        else
                        {
                            if (((i + 1) < end) && ((i + 1) > next))
                            {
                                ST_StationName[i] = ST_PASS;
                                ST_Site[i] = ST_PASS;
                                ST_Track[i] = ST_PASS;
                            }
                            else
                            {
                                ST_StationName[i] = ST_NEXTN;
                                ST_Site[i] = ST_NEXTN;
                                ST_Track[i] = ST_NEXTN;
                            }
                        }
                    }
                    else {}
                }
            }
        }

        if (end == 99) // 环线运行
        {
            grayNum = 5;

            for (i = 1; i <= grayNum; i++) // 当前5个站前PASS
            {
                if (current == next)
                {
                    if (current < StationMaxNumber - i + 1)
                    {
                        if (i < grayNum)
                            ST_Site[current + i - 1] = ST_PASS;

                        ST_Track[current + i - 2] = ST_PASS;
                    }
                    else if (current >= StationMaxNumber - i + 1)
                    {
                        if (i < grayNum)
                            ST_Site[current - StationMaxNumber + i - 1] = ST_PASS;

                        if (current == StationMaxNumber - i + 1)
                            ST_Track[StationMaxNumber - 1] = ST_PASS;
                        else
                            ST_Track[current - StationMaxNumber + i - 2] = ST_PASS;
                    }
                    else {}
                }
                else if (current < StationMaxNumber - i + 1)
                {
                    //qDebug()<<"Out 1111111111111111 : i="<<i;
                    if (current > next)
                    {
                        if (i < grayNum)
                            ST_Site[current + i - 1] = ST_PASS;

                        ST_Track[current + i - 2] = ST_PASS;
                        //qDebug()<<"Out 5 x1 : i="<<i;
                    }
                    else
                    {
                        if (current + i <= next)
                        {
                            if (i < grayNum)
                                ST_Site[current + i - 1] = ST_PASS;

                            ST_Track[current + i - 2] = ST_PASS;
                            //qDebug()<<"Out 5 x2 : i="<<i;
                        }
                    }
                }
                else if (current >= StationMaxNumber - i + 1)
                {
                    //qDebug()<<"Out 22222222222222 : i="<<i;
                    if ((next < current) && (i + current - StationMaxNumber <= next))
                    {
                        if (i < grayNum)
                            ST_Site[current - StationMaxNumber + i - 1] = ST_PASS;

                        if (current == StationMaxNumber - i + 1)
                            ST_Track[StationMaxNumber - 1] = ST_PASS;
                        else
                            ST_Track[current - StationMaxNumber + i - 2] = ST_PASS;

                        //qDebug()<<"Out 5 x3 : i="<<i;
                    }

                    if ((next > current) && (current + i <= next))
                    {
                        if (i < grayNum)
                            ST_Site[current - StationMaxNumber + i - 1] = ST_PASS;

                        if (current == StationMaxNumber - i + 1)
                            ST_Track[StationMaxNumber - 1] = ST_PASS;
                        else
                            ST_Track[current - StationMaxNumber + i - 2] = ST_PASS;

                        //qDebug()<<"Out 5 x4 : i="<<i;
                    }
                }
                else {}
            }
        }

        for (i = 0; i < StationMaxNumber; i++) // 当前站和下一站
        {
            if ((i + 1) == next)
            {
                ST_StationName[i] = ST_NEXT;
                ST_Site[i] = ST_NEXT;
                ST_Track[i] = ST_NEXT;
            }

            if ((i + 1) == current)
            {
                ST_StationName[i] = ST_CURRENT;
                ST_Site[i] = ST_CURRENT;
                ST_Track[i] = ST_PASS;
            }
            else {}
        }

        qDebug() << "up down";
    }

    // 越站判断
    for (i = 0; i < StationMaxNumber; i++)
    {
        if (runDir) // 上行 内环
        {
            if ((skipStation[i / 8] & (1 << (i % 8))) > 0)
            {
                //ST_StationName[i] = ST_SKIP;
                ST_Site[i] = ST_SKIP;

                if (i > 0)
                    ST_Track[i - 1] = ST_SKIP;
                else
                    ST_Track[StationMaxNumber - 1] = ST_SKIP;
            }
        }
        else    // 下行 外环
        {
            if ((skipStation[i / 8] & (1 << (i % 8))) > 0)
            {
                //ST_StationName[i] = ST_SKIP;
                ST_Site[i] = ST_SKIP;
                ST_Track[i] = ST_SKIP;
            }
        }
    }

    qDebug() << "---all disp---";

    // 2. 显示处理
    for (i = 0; i < StationMaxNumber; i++)
    {
        allTrack[i]->show();

        switch (ST_Track[i]) // 轨道图标
        {
            case ST_OUT:
                track_str = "Pass.png";
                break;

            case ST_PASS:
                track_str = "Pass.png";
                break;

            case ST_CURRENT:
                track_str = "Pass.png";
                break;

            case ST_NEXT:
                track_str = "Normal.png";
                break;

            case ST_NEXTN:
                track_str = "Normal.png";
                break;

            case ST_SKIP:
                track_str = "Normal.png";
                break;

            default:
                break;
        }

        if ((i < 15))
            str = allTrackPath + "up" + track_str;
        else if (i == 15)
            str = allTrackPath + "right" + track_str;
        else if (i < 31)
            str = allTrackPath + "down" + track_str;
        else if (i == 31)
            str = allTrackPath + "left" + track_str;
        else {}

        allTrack[i]->setPixmap(str);
        allSite[i]->show();
        allJump[i]->hide();

        switch (ST_Site[i]) // 站点图标
        {
            case ST_OUT:
                site_str = allSitePath + "pass.png";
                allSite[i]->setPixmap(site_str);
                break;

            case ST_PASS:
                site_str = allSitePath + "pass.png";
                allSite[i]->setPixmap(site_str);
                break;

            case ST_CURRENT:
                site_str = allSitePath + "current.png";
                allSite[i]->setPixmap(site_str);
                break;

            case ST_NEXT:
                site_str = allSitePath + "next.png";
                allSite[i]->setPixmap(site_str);
                break;

            case ST_NEXTN:
                site_str = allSitePath + "nextn.png";
                allSite[i]->setPixmap(site_str);
                break;

            case ST_SKIP:
                site_str = allSitePath + "nextn.png";
                allSite[i]->setPixmap(site_str);
                break;

            default:
                break;
        }
    }

#if 0

    for (i = 0; i < StationMaxNumber; i++) // 越站
    {
        switch (ST_StationName[i]) // 站名
        {
        }

        switch (ST_Track[i]) // 轨道图标
        {
            case ST_SKIP:
                break;

            default:
                break;
        }

        switch (ST_Site[i]) // 站点图标
        {
            case ST_SKIP:
                break;

            default:
                break;
        }
    }

#endif
    chgToWidget(ALL);
}


void MainWindow::preProc(int start, int current, int next, int end, bool door, int cmd, int index)
{
    QString str;

    //    if(checkStation(start,current,next,end,1,StationMaxNumber)==false)
    //        return;
    if ((current > StationMaxNumber) || (next > StationMaxNumber)
        || ((end != 98) && (end != 99) && (end > StationMaxNumber)))
        return;

    qDebug() << "---preProc---";

    // 1. 状态处理

    // 2. 显示处理
    if (gRunDir)
        preMRundir.setPixmap(imageDir + "menu/runIn.png");
    else
        preMRundir.setPixmap(imageDir + "menu/runOut.png");

    preMNextSNCh.setText(stationChName[next - 1]);
    preMNextSNCh.adjustSize();
    preMNextSNCh.setGeometry(gPublicInfo.MNextSNCh.rectUp.x, gPublicInfo.MNextSNCh.rectUp.y, preMNextSNCh.width(), gPublicInfo.MNextSNCh.rectUp.h);

    if (stationEnDispLines[next - 1] == 2)
        preMNextSNEn.setText(stationEnName[next - 1] + stationEnName2[next - 1]);
    else if (stationEnDispLines[next - 1] == 20)
        preMNextSNEn.setText(stationEnName[next - 1] + " " + stationEnName2[next - 1]);
    else
        preMNextSNEn.setText(stationEnName[next - 1]);

    preMNextSNEn.adjustSize();
    preMNextSNEn.setGeometry(gPublicInfo.MNextSNCh.rectUp.x + preMNextSNCh.width() + 10, gPublicInfo.MNextSNEn.rectUp.y, preMNextSNEn.width(), gPublicInfo.MNextSNEn.rectUp.h);

    // 站名显示
    if (stationEnDispLines[current - 1] == 2)
        precurrentEn.setText(stationEnName[current - 1] + stationEnName2[current - 1]);
    else if (stationEnDispLines[current - 1] == 20)
        precurrentEn.setText(stationEnName[current - 1] + " " + stationEnName2[current - 1]);
    else
        precurrentEn.setText(stationEnName[current - 1]);

    precurrentCh.setText(stationChName[current - 1]);

    if(current == NOT_RUN_ID)
    {
        precurrentCh.hide();
        precurrentEn.hide();

        preNotRunImage.show();
        preNotRunSN.show();
    }
    else
    {
        precurrentCh.show();
        precurrentEn.show();
        preNotRunImage.hide();
        preNotRunSN.hide();
    }

    if (stationEnDispLines[next - 1] == 2)
        prenextEn.setText(stationEnName[next - 1] + stationEnName2[next - 1]);
    else if (stationEnDispLines[next - 1] == 20)
        prenextEn.setText(stationEnName[next - 1] + " " + stationEnName2[next - 1]);
    else
        prenextEn.setText(stationEnName[next - 1]);

    prenextCh.setText(stationChName[next - 1]);
    preTrackMovie->stop();       // site flash
    preTrackFlash.setMovie(preTrackMovie);
    preTrackMovie->setFileName(preTrackPath + "track.gif");
    preTrackMovie->setSpeed(100);
    preTrackMovie->start();

    if (door == true)           // 开关门显示
    {
        preOpen[0].show();
        preOpen[1].show();
        preClose[0].hide();
        preClose[1].hide();
    }
    else
    {
        preOpen[0].hide();
        preOpen[1].hide();
        preClose[0].show();
        preClose[1].show();
    }

    // 换乘显示
    preChg[0].hide();
    preChg[1].hide();

    for (int i = 0; i < 100; i++)
    {
        if (gAllInfo.chg[i].stationId == current)
        {
            str = QString("%1%2%3").arg(imageDir + "ui_pre/CHANGE/line/").arg(gAllInfo.chg[i].lineNo, 2, 10, QLatin1Char('0')).arg(".png");
            preChg[0].setPixmap(str);
            preChg[0].show();
            qDebug() << "current chg no:" << gAllInfo.chg[i].lineNo;
        }
        else if (gAllInfo.chg[i].stationId == next)
        {
            str = QString("%1%2%3").arg(imageDir + "ui_pre/CHANGE/line/").arg(gAllInfo.chg[i].lineNo, 2, 10, QLatin1Char('0')).arg(".png");
            preChg[1].setPixmap(str);
            preChg[1].show();
            qDebug() << "next chg no:" << gAllInfo.chg[i].lineNo;
        }
        else
        {
            if (gAllInfo.chg[i].stationId == 0)
                break;
        }
    }

    chgToWidget(PRE);
}

void MainWindow::arrProc(int start, int current, int next, int end, bool door, int cmd, int index)
{
    QString str;

    //    if(checkStation(start,current,next,end,1,StationMaxNumber)==false)
    //        return;

    if ((current > StationMaxNumber) || (next > StationMaxNumber)
        || ((end != 98) && (end != 99) && (end > StationMaxNumber)))
        return;

    qDebug() << "---arrProc---";

    if (end == 98)
        gRunDir = 1;
    else if (end == 99)
        gRunDir = 0;

    // 1. 显示处理
    if (gRunDir)
        arrMRundir.setPixmap(imageDir + "menu/runIn.png");
    else
        arrMRundir.setPixmap(imageDir + "menu/runOut.png");

    arrMNextSNCh.setText(stationChName[next - 1]);
    arrMNextSNCh.adjustSize();
    arrMNextSNCh.setGeometry(gPublicInfo.MNextSNCh.rectUp.x, gPublicInfo.MNextSNCh.rectUp.y, arrMNextSNCh.width(), gPublicInfo.MNextSNCh.rectUp.h);

    if (stationEnDispLines[next - 1] == 2)
        arrMNextSNEn.setText(stationEnName[next - 1] + stationEnName2[next - 1]);
    else if (stationEnDispLines[next - 1] == 20)
        arrMNextSNEn.setText(stationEnName[next - 1] + " " + stationEnName2[next - 1]);
    else
        arrMNextSNEn.setText(stationEnName[next - 1]);

    arrMNextSNEn.adjustSize();
    arrMNextSNEn.setGeometry(gPublicInfo.MNextSNCh.rectUp.x + arrMNextSNCh.width() + 10, gPublicInfo.MNextSNEn.rectUp.y, arrMNextSNEn.width(), gPublicInfo.MNextSNEn.rectUp.h);

    // 站名显示
    if (stationEnDispLines[next - 1] == 2)
        arrStationNameEn.setText(stationEnName[next - 1] + stationEnName2[next - 1]);
    else if (stationEnDispLines[next - 1] == 20)
        arrStationNameEn.setText(stationEnName[next - 1] + " " + stationEnName2[next - 1]);
    else
        arrStationNameEn.setText(stationEnName[next - 1]);

    arrStationNameCh.setText(stationChName[next - 1]);
    // 换乘
    int chgBuf[5] = {0, 0, 0, 0, 0};
    int chgBufIndex = 0;

    for (int i = 0; i < 100; i++)
    {
        if (gAllInfo.chg[i].stationId == next)
        {
            nextChgFlag = true;
            chgBuf[chgBufIndex++] = gAllInfo.chg[i].lineNo;
        }
        else
        {
            if (gAllInfo.chg[i].stationId == 0)
                break;
        }
    }

    if (door == true)
    {
        arrStationNameCh.show();
        arrStationNameEn.show();
        arrOpenTextCh[0].show();
        arrOpenTextCh[1].show();
        arrOpenTextEn[0].show();
        arrOpenTextEn[1].show();
        arrOpenFlash.show();
        arrOpenChgImg.hide();
        arrOpenChgLine.hide();
        arrOpenMovie->stop();       // door flash
        arrOpenFlash.setMovie(arrOpenMovie);
        arrOpenMovie->setFileName(imageDir + "ui_arr/DOOR/openCycle.gif");
        arrOpenMovie->setSpeed(100);
        arrOpenMovie->start();
        arrCloseFlash.hide();
        arrCloseChgImg.hide();
        arrCloseChgLine.hide();
        arrCloseTextL.hide();
        arrCloseTextR.hide();

        for (int i = 0; i < 5; i++)
        {
            if (chgBuf[i] > 0)
            {
                str = QString("%1%2%3").arg(imageDir + "ui_arr/CHANGE/line/").arg(chgBuf[i], 2, 10, QLatin1Char('0')).arg(".png");
                arrOpenChgLine.setPixmap(str);
                arrOpenTextCh[1].hide();
                arrOpenTextEn[1].hide();
                arrOpenChgImg.show();
                arrOpenChgLine.show();
            }
        }
    }
    else
    {
        arrStationNameCh.hide();
        arrStationNameEn.hide();
        arrOpenTextCh[0].hide();
        arrOpenTextCh[1].hide();
        arrOpenTextEn[0].hide();
        arrOpenTextEn[1].hide();
        arrOpenChgImg.hide();
        arrOpenChgLine.hide();
        arrOpenFlash.hide();
        arrOpenMovie->stop();       // door flash
        arrCloseFlash.show();
        arrCloseMovie->stop();       // door flash
        arrCloseFlash.setMovie(arrCloseMovie);
        arrCloseMovie->setFileName(imageDir + "ui_arr/DOOR/close.gif");
        arrCloseMovie->setSpeed(100);
        arrCloseMovie->start();
        arrCloseChgImg.hide();
        arrCloseChgLine.hide();

        arrCloseTextL.show();
        arrCloseTextR.show();

        for (int i = 0; i < 5; i++)
        {
            if (chgBuf[i] > 0)
            {
                str = QString("%1%2%3").arg(imageDir + "ui_arr/CHANGE/line/").arg(chgBuf[i], 2, 10, QLatin1Char('0')).arg(".png");
                arrCloseChgLine.setPixmap(str);
                arrCloseChgImg.show();
                arrCloseChgLine.show();
                arrCloseTextR.hide();
            }
        }
    }

    arrToLeaveFlag = 1;
    chgToWidget(ARR);
}

