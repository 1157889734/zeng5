#ifndef __PARAM_H__
#define __PARAM_H__

#define PACK_ALIGN __attribute__((packed))

#define DF_STATION_NUM 100


typedef struct
{
    int r;
    int g;
    int b;
    int a;
}PACK_ALIGN rgba_t;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
}PACK_ALIGN rect_t;

typedef struct
{
    rect_t rectUp;
//    rect_t rectDown;
    rgba_t rgba;
    int fontSize;
}PACK_ALIGN label_info_t;

typedef struct
{
    int stationId;
    int lineNo;
    rect_t rect;
}PACK_ALIGN chg_t;

typedef struct
{
    label_info_t widget;                        // 窗口参数
    label_info_t welcomeLable;
    label_info_t updateLable;
    label_info_t background;

    label_info_t SNOutRangeColor;               // 站名颜色参数
    label_info_t SNPassColor;
    label_info_t SNCurrentColor;
    label_info_t SNNextColor;
    label_info_t SNNextNColor;

    label_info_t logo;

    label_info_t dateLabel;                     // 时间日期参数
    label_info_t weekChLabel;
    label_info_t weekSplitLabel;
    label_info_t weekEnLabel;
    label_info_t timeLabel;

    label_info_t MCurrentTextCh;
    label_info_t MCurrentTextEn;
    label_info_t MNextTextCh;
    label_info_t MNextTextEn;
    label_info_t MNextSNCh;
    label_info_t MNextSNEn;

    label_info_t MRundir;

    label_info_t MDstTextCh;
    label_info_t MDstTextEn;
    label_info_t MDstSNCh;
    label_info_t MDstSNEn;



}PACK_ALIGN public_info_t;

typedef struct
{
    label_info_t background;
    label_info_t chSN[DF_STATION_NUM];          // 站名文本参数
    label_info_t enSN[DF_STATION_NUM];
    label_info_t site[DF_STATION_NUM];          // 站点图标信息
    label_info_t siteFlash;
    label_info_t notRunImage;
    label_info_t notRunSN;
    label_info_t runDirLeft;
    label_info_t runDirRight;
    label_info_t track[DF_STATION_NUM];         // 轨道图标信息
    label_info_t trackFlash[2];
    label_info_t jump[DF_STATION_NUM];
    chg_t chg[DF_STATION_NUM];                  // 所有换乘信息参数
    int stationChgNum[DF_STATION_NUM];          // 每个站的换乘个数

    int siteFlashFrameNum;
    int siteChgFlashFrameNum;
    int trackUpFlashFrameNum;
    int trackDownFlashFrameNum;
    int trackLeftFlashFrameNum;
    int trackRightFlashFrameNum;

//    int VSNOffsetCh;
//    int VSNOffsetEn;
}PACK_ALIGN all_info_t;

typedef struct
{

//-----------------预到站或者离站局部放大参数---------------------------------------
    label_info_t background;

    int leaveStationNum;                        // 站名个数
    int trackNum;                               // 轨道个数

    label_info_t chSN[10];                      // 站名文本参数
    label_info_t enSN[10];

    label_info_t StationChgM[10];                     // 换乘中间坐标参考参数，多个换乘参考此位置左右移动

    label_info_t trackFlash;
    label_info_t doorOpenImg[2];
    label_info_t doorCloseImg[2];
    label_info_t chg[2];

    label_info_t currentSNch;
    label_info_t currentSNen;

    label_info_t nextSNch;
    label_info_t nextSNen;


//-----------------到站开关门参数---------------------------------------
    label_info_t ArrBackground;
    label_info_t ArriveChSN;                // 站名文本参数
    label_info_t ArriveEnSN;

    label_info_t ArriveCloseChSN;                // 站名文本参数
    label_info_t ArriveCloseEnSN;
    label_info_t ArrCloseSNSplit;

    label_info_t ArriveNextCh;                // 站名文本参数
    label_info_t ArriveNextEn;

    label_info_t ArriveNextTextCh;                // 站名文本参数
    label_info_t ArriveNextTextEn;

    label_info_t ArrOpenDoor;
    label_info_t ArrOpenLeftTextCh;
    label_info_t ArrOpenLeftTextEn;
    label_info_t ArrOpenRightTextCh;
    label_info_t ArrOpenRightTextEn;
    label_info_t ArrOpenChgTextImg;
    label_info_t ArrOpenChgLine;

    label_info_t ArrCloseDoorImg;
    label_info_t ArrCloseChgTextImg;
    label_info_t ArrCloseChgLine;
    label_info_t ArrCloseTextL;
    label_info_t ArrCloseTextR;

    label_info_t notRunImage;
    label_info_t notRunSN;

    int siteFlashFrameNum;
    int siteChgFlashFrameNum;
    int trackFlashFrameNum;
    int openFlashFrameNum;
    int openToCloseFrameNum;
    int trainFlashFrameNum;
    int arrOpenFrameNum;
    int arrOpenCycleFrameNum;
    int arrCloseFrameNum;

}PACK_ALIGN part_info_t;



#endif
