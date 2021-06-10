#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "myudpforrcvcontrol.h"
#include "mythread/mythread.h"
#include <QMainWindow>
#include <QLabel>
#include <QFont>
#include <QPen>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QWidget>
#include "myqthread.h"
#include "datadefine.h"
#include "drmd_ui.h"
#include "unistd.h"
#include "param.h"
#include "malloc.h"
#include "vertical_ch.h"
#include "vertical_en.h"
#include "horizontalSN.h"
#include "stdio.h"

namespace Ui {
class MainWindow;
}

#define DEFINE_STATION_NUM 50

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void InitEnv();
    void readInitFile();
    void labelInit(QLabel &label, label_info_t info_t, int type, QString str, int widgetNum, QString fontStr, bool blod);
    void publicInit(public_info_t *ppublic);
    void allInit(all_info_t *pallInfo);
    void preInit(part_info_t *ppartInfo);
    void arrInit(part_info_t *ppartInfo);
    void initDipaly(public_info_t *ppublic, all_info_t *pallInfo, part_info_t *ppartInfo);
    void allProc(int start, int current, int next, int end, bool door, int cmd,int index);
    void preProc(int start,int current,int next,int end,bool door,int cmd,int index);
    void arrProc(int start,int current,int next,int end,bool door,int cmd,int index);
    bool checkStation(int start,int current,int next,int end,int station1,int station2);
    void setFont(QLabel &label, QString name, bool bold, int weight, int size, int r, int g, int b);
    void chgToWidget(int index);
    void ipConflictProc(int flag);
    void readLabelAttr(label_info_t *ps,QSettings *set, QString path,int labelType,QString language,int index,bool debug);

    /*------Ui Init parameter------*/
    /***images path***/
    QString imageDir = QString("/root/media/images/");
    QString welcomePath = QString(imageDir+"welcome.png");

    QString allBgPath = QString(imageDir+"ui_all/BG/bg.png");
    QString allSitePath = QString(imageDir+"ui_all/SITE/");
    QString allTrackPath = QString(imageDir+"ui_all/TRACK/");
    QString allChgPath = QString(imageDir+"ui_all/CHANGE/line/");

    /******1. 公共控件&变量参数******/
    int widgetIndex;
    QWidget widget[10];
    QLabel  welcomeLable;
    QLabel  updateLable;

    bool nextChgFlag;           // 下一站是否为换乘站

    //---标题栏共同属性控件
    QLabel allLogo;             //  LOGO
    QLabel preLogo;
    QLabel arrLogo;

    QLabel dateLabel;           // 日期时间
    QLabel weekChLabel;
    QLabel weekSplitLabel;
    QLabel weekEnLabel;
    QLabel timeLabel;

    QLabel preDateLabel;
    QLabel preWeekChLabel;
    QLabel preWeekSplitLabel;
    QLabel preWeekEnLabel;
    QLabel preTimeLabel;

    QLabel arrDateLabel;
    QLabel arrWeekChLabel;
    QLabel arrWeekSplitLabel;
    QLabel arrWeekEnLabel;
    QLabel arrTimeLabel;

    QLabel allMNTextCh;         //  下一站文本
    QLabel allMNTextEn;
    QLabel preMNTextCh;
    QLabel preMNTextEn;
    QLabel arrMCTextCh;         //  当前站文本
    QLabel arrMCTextEn;

    QLabel allMNextSNCh;        //  下一站站名
    QLabel allMNextSNEn;
    QLabel preMNextSNCh;
    QLabel preMNextSNEn;
    QLabel arrMNextSNCh;
    QLabel arrMNextSNEn;

    QLabel allMRundir;          //  内外环图片
    QLabel preMRundir;
    QLabel arrMRundir;

    /******2. all ui ******/
    QLabel allBgLabel;

    QLabel *allSNPassCh[50];    // 站名控件
    QLabel *allSNNextCh[50];
    QLabel *allSNPassEn[50];
    QLabel *allSNPassEn2[50];
    QLabel *allSNNextEn[50];
    QLabel *allSNNextEn2[50];

    QLabel *allSite[50];
    QLabel *allTrack[50];
    QLabel *allJump[50];
    QLabel *allStart[50];
    QLabel *allEnd[50];
    QLabel *allChg[100];

    QPixmap *allSitePixmap[50];
    QPixmap *allSiteChgPixmap[50];
    QPixmap *allTrackUPixmap[50];
    QPixmap *allTrackDPixmap[50];
    QPixmap *allTrackLPixmap[50];
    QPixmap *allTrackRPixmap[50];

    QLabel allNotRunImage;
    QLabel allNotRunSN;
    QLabel allRunDirLeft;
    QLabel allRunDirRight;
    QMovie *allRDLeftMovie;
    QMovie *allRDRightMovie;

    QLabel allTrackFlash;
    QMovie *allTrackMovie;
    int allTrackFlashIndex;

    QLabel allSiteFlash;
    QMovie *allSiteMovie;
    int allSiteFlashIndex;


    /*###2 pre arrive ui */
    QString preBgPath = QString(imageDir+"ui_pre/BG/partBg.png");
    QLabel preBgLabel;

    QLabel precurrentCh;          // station SN
    QLabel precurrentEn;
    QLabel prenextCh;
    QLabel prenextEn;

    QString preSitePath = QString(imageDir+"ui_pre/SITE/");
    QString preTrackPath = QString(imageDir+"ui_pre/TRACK/");

    QString preChgPath = QString(imageDir+"ui_pre/CHANGE/line/");
    QLabel preChg[2];                         // 换乘数字图标

    QLabel preMNextTextCh;                 //  标题栏参数
    QLabel preMNextTextEn;

    QLabel preTrackFlash;
    QMovie *preTrackMovie;

    QLabel preOpen[2];                     // 预到开门提示
    QLabel preClose[2];                    // 预到关门提示

    QLabel preNotRunImage;
    QLabel preNotRunSN;

    bool preNextChgFlag;                   // 下一站是否为换乘站

    /*###3 arrive ui init*/
    QLabel arrBgLabel;

    QLabel arrMNextTextCh;                 //  标题栏参数
    QLabel arrMNextTextEn;
    QLabel arrMDstTextCh;
    QLabel arrMDstTextEn;
    QLabel arrMDstSNCh;
    QLabel arrMDstSNEn;

    QLabel arrOpenTextCh[2];
    QLabel arrOpenTextEn[2];
    QLabel arrOpenFlash;                     // 开门提示
    QMovie *arrOpenMovie;
    QLabel arrOpenChgImg;
    QLabel arrOpenChgLine;

    QLabel arrStationNameCh;
    QLabel arrStationNameEn;

    QLabel arrCloseFlash;                     // 关门提示
    QMovie *arrCloseMovie;
    QLabel arrCloseChgImg;
    QLabel arrCloseChgLine;
    QLabel arrCloseTextL;
    QLabel arrCloseTextR;

    /*drmd info ui */
    QLabel ipLabel;
    QLabel ipConflictLabel;
    QLabel macLabel;
    QLabel versionLabel;
    QLabel carIdLabel;
    QLabel devIdLabel;

    /*color test */
    QLabel holeDisp;
    QLabel grayScale16[16];
    QLabel grayScale32[32];
    QLabel grayScale64[64];
    unsigned char cycleMode;
    unsigned char cycleTime;
    unsigned char cycleIndex;
    unsigned char colorDispIndex;

    /*public parameter*/
    int updateFlag = 0;
    int doorIndex = 0;
    int startId = 1;
    int currentId = 1;
    int nextId = 2;
    int endId = 32;
    int StationMaxNumber=DEFINE_STATION_NUM;
    int gRunDir=1;
    unsigned char run_st;
    int movieIndex=0;
    int movieFrameNum=0;
    bool doorFlag = false;
    bool doorCycleFlag = false;
    bool preFlag = false;
    bool arrivedFlag = false;
    bool mirrorFlag = false;
    BYTE skipStation[10];

    int arrToLeaveFlag=0;
    int leaveSt; // 离站状态，应用与多个离站界面循环切换
    int leave_chg_time; // 离站界面之间切换时间
    int arrOpenFlag=0;  // 到站开门标志

private:
    Ui::MainWindow *ui;

    QTimer *psysTimer;
    //    QMovie *movie;
    //    QMovie *movieAllDoor;
    //    QMovie *moviePreDoor;
    //    QMovie *movieAllNext;
    //    QMovie *moviePreNext;

    QStringList weeklist;
    QStringList weekchlist;

    int stationNum;                        // 当前的最大站号
    int ip4;
    QString macStr;

    QList<QString>stationChName;
    QList<QString>stationEnName;
    QList<QString>stationEnName2;
    QList<int>stationEnDispLines;

    QList<QString>changeLineCh;
    QList<QString>changeLineEn;

protected:
signals:

private slots:
    void running_trigger(BYTE type,BYTE start,BYTE current,BYTE next,BYTE end,bool door);
    void sysproc();
    void movieChgProc();
    void setWelcome();
    void setInfo();
    void setColorTest(BYTE mode, BYTE time, BYTE index);
    void setUpdate(int update);
    void MainDisplayType(BYTE);
    void setSkip(BYTE *buf,int len);
    void setMirror(bool mirror);
    void TimeoutProc();
    void imageInit();


private:
    class MyUdpforRcvControl mycontroludp;
    class myqthread mytimer;
    class myqthread mytimer2;
    class myqthread mytimer3;
};

#endif // MAINWINDOW_H
