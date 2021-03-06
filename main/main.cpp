//#include <strings.h>
//#include <string.h>
#include <iostream>
#include <string>
using namespace std;
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include<time.h>
#include <string.h>
#include "def.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ini_file.h"
#include "osd_proc.h"
#include "drmd_ui.h"
#include "vvs.h"
#include "pisc.h"
#include "ptu.h"
#include "ptu_tcp.h"
#include "udp.h"
#include "param.h"
#include "ipCheck.h"
#include "main.h"

#define DELAY_500MS 500
#define DELAY_1000MS 1000

static pthread_t piscRecvThreadFd;
static pthread_t piscSendThreadFd;
static pthread_t osdRecvThreadFd;
static pthread_t osdSendThreadFd;
static pthread_t ptuTcpRecvThreadFd;
static pthread_t ptuUdpRecvThreadFd;
static pthread_t ptuTcpSendThreadFd;
static pthread_t drmdRecvThreadFd;
static pthread_t drmdSendThreadFd;
static pthread_t testSendThreadFd;
static pthread_t timer300msFd;
static pthread_t ipConflictFd;

int fun(unsigned char *buf, int len)
{
    int i;
    printf("main==>fun %d:", len);
    
    for (i = 0; i < len; i++)
    {
        printf("%0.2x ", buf[i]);
    }
    
    printf("\n");
}

void myUsleep(unsigned long usec)
{
    struct timeval tv;
    tv.tv_sec = usec / 1000000;
    tv.tv_usec = usec % 1000000;
    int err;
    
    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    }
    while (err < 0 && errno == EINTR);
}

static mainDevDataSt mainDevData = {0,};
static mainDevDataSt mainDevDataOld = {0,};
//static PiscRecvSt *piscRecvPacket;


static unsigned char osd_lcd_st = LCD_IDLE;
static unsigned char get_osd_lcd_st(void)
{
    return osd_lcd_st;
}
static void set_osd_lcd_st(unsigned char status)
{
    osd_lcd_st = status;
}

static unsigned char drmd_display_st = 0;
static unsigned char get_drmd_display_st(void)
{
    return drmd_display_st;
}
static void set_drmd_display_st(unsigned char status)
{
    drmd_display_st = status;
}

static int out_line_time = 180;

int get_out_line_time(void)
{
    return out_line_time;
}
void set_out_line_time(int time)
{
    out_line_time = time;
}

static int off_line_time = 600;
int get_off_line_time(void)
{
    return off_line_time;
}

void set_off_line_time(int time)
{
    off_line_time = time;
}

int drmdTimeOutProc()
{
    int netTime;
    int rs485Time;
    int nowTime;
    int nosignal;
    int closeLcd;
    static int send485cnt = 0;
    nowTime = time((time_t *)NULL);
    pthread_rwlock_rdlock(&mainDevData.lock);
    rs485Time = mainDevData.rs485Time;
    netTime = mainDevData.netTime;
    nosignal = mainDevData.nosignal;
    closeLcd = mainDevData.closeLcd;
    pthread_rwlock_unlock(&mainDevData.lock);
    
    if ((netTime + 8 ) <= nowTime) // ????????????485
    {
        if (++send485cnt % 10 == 1)
            set485(RS485_ON);
            
        //if(nosignal<=0){
        //    pthread_rwlock_wrlock(&mainDevData.lock);
        //    mainDevData.nosignal = 1;
        //    pthread_rwlock_unlock(&mainDevData.lock);
        //}
    }
    
    if (((netTime + get_out_line_time()) <= nowTime) && ((rs485Time + get_out_line_time()) <= nowTime) && (nosignal <= 0)) //??????????
    {
        if (get_osd_CycleMode() <= 0)
        {
            pthread_rwlock_wrlock(&mainDevData.lock);
            mainDevData.nosignal = 1;
            pthread_rwlock_unlock(&mainDevData.lock);
            printf("main==>SET_NOSIGNAL\n");
            setCmd(SET_NOSIGNAL);
            sendDrmdPacket();
        }
    }
    
    if (((netTime + get_off_line_time()) <= nowTime) && ((rs485Time + get_off_line_time()) <= nowTime) && (closeLcd <= 0)) //????????
    {
        if (get_osd_CycleMode() <= 0)
        {
            pthread_rwlock_wrlock(&mainDevData.lock);
            mainDevData.rs485Time = nowTime;
            mainDevData.netTime = nowTime;
            //mainDevData.closeLcd = 1;
            pthread_rwlock_unlock(&mainDevData.lock);
            printf("main==>SET_CLOSE_LCD\n");
            setCmd(SET_CLOSE_LCD);
            setLcd(LCD_OFF);
            sendDrmdPacket();
        }
    }
}

int initDevId(unsigned char devId)
{
    pthread_rwlock_wrlock(&mainDevData.lock);
    mainDevData.devId = devId;
    pthread_rwlock_unlock(&mainDevData.lock);
}

#define DATA_NET 0
#define DATA_485 1

#define CHG_STATION 1
#define CHG_RUNDIR 2
#define CHG_DOOR 3
#define CHG_DOOR_ERR 4
#define CHG_LEAVE 5
#define CHG_ARRIVE 6
#define CHG_SYNC 7

void get_pisc_sta(unsigned char *data_buf, int len, int type)
{
    int temp;
    PiscRecvSt *piscRecvPacket = (PiscRecvSt *)data_buf;
    static unsigned int door_side = 0;  // 1:right 2:left 3:both
    static unsigned char cmdBackup = 0;
    static unsigned int drmdStSyncCnt = 0;
    static int startUpFlag = 1;
    static int arrivedFlagOld = 0;
    
    if (get_osd_lcd_st() != LCD_ON)
        setLcd(LCD_ON);
        
    //??????????????, ????????????????
    temp = time((time_t *)NULL);
    pthread_rwlock_wrlock(&mainDevData.lock);
    
    if (type == DATA_NET)
    {
        mainDevData.netTime = temp;
        set485(RS485_OFF);
    }
    
    mainDevData.rs485Time = temp;
    mainDevData.closeLcd = 0;
    mainDevData.nosignal = 0;
    mainDevData.chgFlag = 0;
    mainDevData.leaveFlag = piscRecvPacket->signal_runst & 0x80;
    mainDevData.arrivedFlag = piscRecvPacket->signal_runst & 0x40;
    mainDevData.startId = piscRecvPacket->start;
    mainDevData.currentId = piscRecvPacket->current;
    mainDevData.nextId = piscRecvPacket->next;
    mainDevData.endId = piscRecvPacket->end;
    mainDevData.emeId = piscRecvPacket->emeId;
    mainDevData.pisc_dev_id = piscRecvPacket->frame_head.src_eq.num;
    mainDevData.masterFlag = piscRecvPacket->signal_mk & 0x80;
    mainDevData.key = piscRecvPacket->signal_mk & 0x40;
    mainDevData.nextOpenLeft = piscRecvPacket->signal_rundoor & 0x01;
    mainDevData.nextOpenRight = piscRecvPacket->signal_rundoor & 0x02;
    mainDevData.openleftPulse= piscRecvPacket->signal_rundoor & 0x10;
    mainDevData.openrightPulse= piscRecvPacket->signal_rundoor & 0x20;
    mainDevData.up = piscRecvPacket->signal_rundoor & 0x80;
    mainDevData.down =  piscRecvPacket->signal_rundoor & 0x40;
    mainDevData.closeleft = piscRecvPacket->signal_rundoor & 0x04;
    mainDevData.closeright = piscRecvPacket->signal_rundoor & 0x08;
    
    if ((getDisplayCmd() == SET_CLOSE_LCD) || (getDisplayCmd() == SET_NOSIGNAL)
        || (getDisplayCmd() == SET_IP_CONFLICT_OFF))
    {
        mainDevDataOld.leaveFlag = 0;
        mainDevDataOld.arrivedFlag = 0;
    }
    
    if (getDisplayCmd() != get_drmd_display_st())
    {
        if (++drmdStSyncCnt > 5)
        {
            drmdStSyncCnt = 5;
            mainDevData.chgFlag = 1;
            setChgSt(CHG_SYNC);
            printf("main==>drmd st no update\n");
        }
    }
    else
        drmdStSyncCnt = 0;
        
    if (((mainDevDataOld.up == 0) && (mainDevData.up)) ||
        ((mainDevDataOld.down == 0) && (mainDevData.down)))
    {
        setCmd(SET_LEAVE);
        mainDevData.chgFlag = 1;
        setChgSt(CHG_RUNDIR);
    }
    
    // 1.????????????
    int startId, currentId, nextId, endId;
    startId = mainDevData.startId;
    currentId = mainDevData.currentId;
    nextId = mainDevData.nextId;
    endId = mainDevData.endId;
    
    if (1)
    {
        if ((mainDevDataOld.startId  != mainDevData.startId)
            || (mainDevDataOld.endId != mainDevData.endId))
        {
            mainDevData.chgFlag = 1;
            setChgSt(CHG_STATION);
            setStartStation(startId);
            setCurrentStation(currentId);
            setNextStation(nextId);
            setEndStation(endId);
        }
        
        if (((mainDevDataOld.currentId != mainDevData.currentId) || (mainDevDataOld.nextId != mainDevData.nextId))
            && (mainDevData.leaveFlag || mainDevData.arrivedFlag))
        {
            mainDevData.chgFlag = 1;
            setChgSt(CHG_STATION);
        }
        
        //printf("start:%d current:%d next:%d end:%d\n",startId,currentId,nextId,endId);
    }
    
    if (((mainDevDataOld.up == 0) && (mainDevData.up)) ||
        ((mainDevDataOld.down == 0) && (mainDevData.down)))
    {
        setCmd(SET_LEAVE);
        setStartStation(startId);
        setCurrentStation(currentId);
        setNextStation(nextId);
        setEndStation(endId);
        mainDevData.chgFlag = 1;
        setChgSt(CHG_RUNDIR);
    }
    
    // 2. ????????
    int leaveFlag, leaveFlagOld;
    int leaveTime;
    leaveFlag = mainDevData.leaveFlag;
    leaveFlagOld = mainDevDataOld.leaveFlag;
    
    if (0 == leaveFlagOld && leaveFlag > 0)
    {
        //????????????
        leaveTime = time((time_t *)NULL);
        mainDevData.leaveTime = leaveTime;
        mainDevData.preFlag = 1;  // ??????????????
        mainDevData.chgFlag = 1; // ????????????
        setChgSt(CHG_LEAVE);
        printf("main==>leaveFlag\n");
        startUpFlag = 0;
        setCmd(SET_LEAVE);
        setStartStation(startId);
        setCurrentStation(currentId);
        setNextStation(nextId);
        setEndStation(endId);

        mainDevData.openleft = 0;
        mainDevData.openright = 0;
    }
    
    // 3. ??????????
#if 0
    int preFlag;
    int nowTime;
    preFlag = mainDevData.preFlag;
    leaveTime = mainDevData.leaveTime;
    
    if (preFlag > 0)
    {
        nowTime = time((time_t *)NULL);
        
        if (leaveTime + DRMD_PRE_DELAY_TIME <= nowTime) //  pre delay time
        {
            //??????????
            mainDevData.preFlag = 0;
            mainDevData.chgFlag = 1;
            printf("preFlag\n");
            setCmd(SET_PRE);
        }
    }
    
#endif
    // 4. ????????
    int arrivedFlag = 0;
        
    if (mainDevData.openleftPulse|| mainDevData.openrightPulse)
    {
        arrivedFlag = 1;
    }
    else
    {
        arrivedFlag = 0;
    }

    if ((0 == arrivedFlagOld) && (arrivedFlag > 0))
    {
        mainDevData.preFlag = 0;
        mainDevData.chgFlag = 1;
        setChgSt(CHG_ARRIVE);
        setCmd(SET_ARRIVED);
        startUpFlag = 0;
        printf("main==>arrivedFlag\n");
    }
    
    arrivedFlagOld = arrivedFlag;
    
    // 5. ??????????
    int devId, pisc_id, nextOpenleft, nextOpenright, doorflag; 
    devId = mainDevData.devId;
    pisc_id =  mainDevData.pisc_dev_id;
    
    #if 0
    if(mainDevData.openleftPulse)
    {
        mainDevData.openleft = 1;
    }

    if(mainDevData.openrightPulse)
    {
        mainDevData.openright = 1;
    }

    if((mainDevData.closeleft ) || (mainDevData.closeright))
    {
        mainDevData.openleft = 0;
        mainDevData.openright = 0;
    }
    #else
    if((mainDevData.openleftPulse) || (mainDevData.openrightPulse))
    {
        if(mainDevData.key)
        {
            mainDevData.openPiscId = mainDevData.pisc_dev_id;
        }
        else
        {
            mainDevData.openPiscId = mainDevData.pisc_dev_id % 2 + 1;
        }

        if(mainDevData.openleftPulse)
        {
            mainDevData.openleft = 1;
        }

        if(mainDevData.openrightPulse)
        {
            mainDevData.openright = 1;
        }
    }

    if((mainDevData.closeleft) || (mainDevData.closeright))
    {
        if(mainDevData.key)
        {
            if(((mainDevData.openPiscId == mainDevData.pisc_dev_id) && (mainDevData.closeleft)) ||
                ((mainDevData.openPiscId != mainDevData.pisc_dev_id) && (mainDevData.closeright)))
            {
                    mainDevData.openleft = 0;
            }
            else
            {
                    mainDevData.openright = 0;
            }
        }
        else
        {
            if(((mainDevData.openPiscId == mainDevData.pisc_dev_id) && (mainDevData.closeleft)) ||
                ((mainDevData.openPiscId != mainDevData.pisc_dev_id) && (mainDevData.closeright)))
            {
                    mainDevData.openright = 0;
            }
            else
            {
                    mainDevData.openleft = 0;
            }
        }
        if((mainDevData.openleft ==0) && (mainDevData.openright == 0))
        {
            mainDevData.openPiscId = 0;
        }
    }
    #endif
    
    if(mainDevData.openleft ||  mainDevData.openright)
    {
        mainDevData.nextOpenLeft = 0;
        mainDevData.nextOpenRight = 0;
    }

    if ( mainDevData.nextOpenLeft ||  mainDevData.openleft)
    {
        nextOpenleft = 1;
    }
    else
    {
        nextOpenleft = 0;        
    }
        
    if ( mainDevData.nextOpenRight ||  mainDevData.openright)
    {
        nextOpenright = 1;
    }
    else
    {
        nextOpenright = 0;
    }
    
    doorflag = mainDevData.doorflag;
    
    if (((mainDevData.key > 1) && (pisc_id == 2)) || ((mainDevData.key == 0) && (pisc_id == 1)))
    {
        if ((nextOpenleft == 0) && (nextOpenright > 0))
        {
            nextOpenleft = nextOpenright;
            nextOpenright = 0;
        }
        else
        {
            if ((nextOpenleft > 0) && (nextOpenright == 0))
            {
                nextOpenright = nextOpenleft;
                nextOpenleft = 0;
            }
        }
    }
    
    if (nextOpenleft > 0 && nextOpenright > 0) // ????????
    {
        doorflag = 1;
    }
    else if (nextOpenleft > 0) // ??????
    {
        if(((devId % 2 > 0) && (devId <= 30)) ||((devId % 2 == 0) && (devId > 30))) // ??????
        {
            doorflag = 1;
        }
        else
        {
            doorflag = 0;
        }
    }
    else if (nextOpenright > 0)
    {
        if(((devId % 2 > 0) && (devId <= 30)) ||((devId % 2 == 0) && (devId > 30))) // ??????
        {
            doorflag = 0;
        }
        else
        {
            doorflag = 1;
        }
    }
    else
    {
        doorflag = 0;
    }
    
    mainDevData.doorflag = doorflag;
    setDoor(doorflag);
    
    if ((mainDevData.nextOpenLeft !=  mainDevDataOld.nextOpenLeft) ||
        (mainDevData.nextOpenRight != mainDevDataOld.nextOpenRight))
    {
        if (startUpFlag == 0)
        {
            //setCmd(SET_OPEN);
            mainDevData.chgFlag = 1;
            printf("main==>door st chg\n");
        }
    }
    
    // 6. ????????
    if (((mainDevData.closeleft > 0) && (mainDevDataOld.closeleft == 0))
        || ((mainDevData.closeright > 0) && (mainDevDataOld.closeright == 0)))
    {
        mainDevData.chgFlag = 1; // ????????????
        setChgSt(CHG_DOOR);
        printf("main==>close door trigger\n");
        startUpFlag = 0;
        
        if (getDisplayCmd() != SET_LEAVE)
        {
            setCmd(SET_LEAVE);
            setStartStation(startId);
            setCurrentStation(currentId);
            setNextStation(nextId);
            setEndStation(endId);
        }
    }
    
    // 7. ????????
    // 8. ??????????????
#if 0
    int carId, doorId;
    int doorErr = 0;
    carId = (read_dev_id() - 14) / 10 + 1;
    doorId = (read_dev_id() - 14) % 10 + 1;
    
    if (doorId < 9)
    {
        if (piscRecvPacket->car1_8_door1_8_unlock[carId - 1] &  (0x01 << (doorId - 1)))
            doorErr = 1;
            
        if (piscRecvPacket->car1_8_door_isolation[(carId - 1) * 2] & (0x01 << (doorId - 1)))
            doorErr = 1;
    }
    else
    {
        if ((carId <= 4) && (piscRecvPacket->car1_8_door9_10_unlock[0] & (0x01 << ((carId - 1) * 2 + doorId - 9))))
            doorErr = 1;
            
        if ((carId > 4) && (piscRecvPacket->car1_8_door9_10_unlock[1] & (0x01 << ((carId - 5) * 2 + doorId - 9))))
            doorErr = 1;
            
        if (piscRecvPacket->car1_8_door_isolation[(carId - 1) * 2 + 1] & (0x01 << (doorId - 9)))
            doorErr = 1;
    }
    
    if (doorErr)
    {
        if (mainDevData.doorerror == 0)
        {
            mainDevData.chgFlag = 1;
            setChgSt(CHG_DOOR_ERR);
        }
    }
    else
    {
        if (mainDevData.doorerror == 1)
        {
            mainDevData.chgFlag = 1;
            setChgSt(CHG_DOOR_ERR);
        }
    }
    
    setDoorErr(doorErr);
    mainDevData.doorerror = doorErr;
#endif
    // 9.????????
    unsigned char  skip_buf[8];
    unsigned char  skip_cnt, skip_buf_offset, skip_bit_offset;
    
    for (skip_cnt = 0; skip_cnt < 4; skip_cnt++)
    {
        skip_buf[skip_cnt] = piscRecvPacket->skip1_32[skip_cnt];
    }
    
    for (skip_cnt = 0; skip_cnt < 4; skip_cnt++)
    {
        skip_buf[skip_cnt + 4] = piscRecvPacket->skip33_64[skip_cnt];
    }
    
#if 0
    
    if (((startId <= currentId && currentId <= nextId && nextId <= endId) ||
         (startId >= currentId && currentId >= nextId && nextId >= endId)) &&
        (startId > 0 && currentId > 0 && nextId > 0 && endId > 0))
    {
        if ((nextId > (currentId + 1)) && (startId <= endId)) // up
        {
            for (skip_cnt = currentId + 1; skip_cnt < nextId; skip_cnt++)
            {
                skip_buf_offset = (skip_cnt - 1) / 8;
                skip_bit_offset = (skip_cnt - 1) % 8;
                skip_buf[skip_buf_offset] = skip_buf[skip_buf_offset] | (1 << skip_bit_offset);
            }
        }
        
        if (((nextId + 1) < currentId) && (startId >= endId)) // down
        {
            for (skip_cnt = nextId + 1; skip_cnt < currentId; skip_cnt++)
            {
                skip_buf_offset =   (skip_cnt - 1) / 8;
                skip_bit_offset = (skip_cnt - 1) % 8;
                skip_buf[skip_buf_offset] = skip_buf[skip_buf_offset] | (1 << skip_bit_offset);
            }
        }
    }
    
#endif
    setSkipBuf(skip_buf, 8);
    
    // 10.??????????????????????????????
    if (mainDevData.chgFlag > 0)
    {
        sendDrmdPacket();
        printf("main==>sendDrmdPacket\n");
    }
    
    // 11.????????????????????????
    memcpy(&mainDevDataOld, &mainDevData, sizeof(mainDevDataOld));
    pthread_rwlock_unlock(&mainDevData.lock);
}

void PiscProc(unsigned char *data_buf, int len, int type)
{
    FRAME_HEAD *p_frame_head;
    static int time_init = 0;
    p_frame_head = (FRAME_HEAD *)data_buf;
    
    if ((len < LEN_FRAME_HEAD) || (p_frame_head->dstn_eq.ip[0] != read_dev_id() && p_frame_head->dstn_eq.ip[0] != 0xff )
        || (p_frame_head->frame_start != 0x7E))
    {
        //printf("----PiscProc pakect not for drmd----- \n");
        return;
    }
    
    //printf("---- pakect type :%d \n",p_frame_head->src_eq.type);
    
    switch (p_frame_head->src_eq.type)
    {
        case TYPE_PISC://????
            if ((p_frame_head->cmd == 0x0001) && ((data_buf[LEN_FRAME_HEAD] & 0x80) == 0x80))
                get_pisc_sta(data_buf, len, type); //??????????????????
                
            if (p_frame_head->cmd == 0x000a)
            {
                //printf("----TYPE_PISC time ----- \n");
                //struct rtc_time tm;
                struct tm tm;
                struct timeval tv;
                time_t timep;
                int pisctime;
                pisctime = time((time_t *)NULL);
                
                //pthread_rwlock_rdlock(&mainDevData.lock);
                //if((pisctime>mainDevData.crcTime +600)||(time_init==0))
                if ((pisctime > mainDevData.crcTime + 300)
                    || (mainDevData.year_l   != data_buf[LEN_FRAME_HEAD])
                    || (mainDevData.year_h  != data_buf[LEN_FRAME_HEAD + 1])
                    || (mainDevData.month   != data_buf[LEN_FRAME_HEAD + 2])
                    || (mainDevData.date      != data_buf[LEN_FRAME_HEAD + 3])
                    || (mainDevData.hour      != data_buf[LEN_FRAME_HEAD + 4])
                    || (mainDevData.minute   != data_buf[LEN_FRAME_HEAD + 5]))
                {
                    mainDevData.year_l  = data_buf[LEN_FRAME_HEAD];
                    mainDevData.year_h = data_buf[LEN_FRAME_HEAD + 1];
                    mainDevData.month  = data_buf[LEN_FRAME_HEAD + 2];
                    mainDevData.date     = data_buf[LEN_FRAME_HEAD + 3];
                    mainDevData.hour     = data_buf[LEN_FRAME_HEAD + 4];
                    mainDevData.minute  = data_buf[LEN_FRAME_HEAD + 5];
                    mainDevData.second  = data_buf[LEN_FRAME_HEAD + 6];
                    tm.tm_year = (mainDevData.year_h << 8) + mainDevData.year_l  - 1900;
                    tm.tm_mon = mainDevData.month - 1;
                    tm.tm_mday = mainDevData.date ;
                    tm.tm_hour = mainDevData.hour;
                    tm.tm_min = mainDevData.minute ;
                    tm.tm_sec = mainDevData.second ;
                    timep = mktime(&tm);
                    tv.tv_sec = timep;
                    tv.tv_usec = 0;
                    
                    if (settimeofday(&tv, (struct timezone *) 0) < 0)
                    {
                        perror("settimeofday err \n");
                    }
                    
                    printf("main==>now time:%4d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    mainDevData.crcTime = time((time_t *)NULL);
                    mainDevData.netTime = time((time_t *)NULL);
                    //pthread_rwlock_unlock(&mainDevData.lock);
                }
                
                if (time_init == 0)
                    time_init = 1;
            }
            
            break;
            
        default:
            break;
    }
}

void *piscRecvProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    PiscInit();
    
    while (1)
    {
        ret = PiscReadData(buf, 1024);
        
        if (ret > 0)
        {
            //fun(buf,ret);
            PiscProc(buf, ret, DATA_NET);
        }
        else
            usleep(1);
    }
}


void piscSendAck(void)
{
    unsigned char buf[1024];
    unsigned long long rand_no;
    int len;
    buf[2] = read_light();
    unsigned char i;
    unsigned char tmp_data[200];
    unsigned char check_sum = 0;
    FRAME_HEAD *p_frame_head;
    p_frame_head = (FRAME_HEAD *)tmp_data;
    p_frame_head->frame_start = 0x7e;
    p_frame_head->dstn_eq.vehicle_num_l = 0xff;
    p_frame_head->dstn_eq.vehicle_num_h = 0xff;
    p_frame_head->dstn_eq.type = 0xff;
    p_frame_head->dstn_eq.num = 0xff;
    p_frame_head->dstn_eq.ip[0] = 0xff;
    p_frame_head->dstn_eq.ip[1] = 0xff;
    p_frame_head->dstn_eq.ip[2] = 0xff;
    p_frame_head->dstn_eq.ip[3] = 0xff;
    p_frame_head->src_eq.vehicle_num_l = (read_dev_id() - 1) / 10 + 1;
    p_frame_head->src_eq.vehicle_num_h = 0;
    p_frame_head->src_eq.type = 0x07;
    
    if (read_dev_id() % 10 == 0)
        p_frame_head->src_eq.num = 10;
    else
        p_frame_head->src_eq.num = read_dev_id() % 10; // ip=1~60
        
    p_frame_head->src_eq.ip[0] = read_dev_id();
    p_frame_head->src_eq.ip[1] = 7;
    p_frame_head->src_eq.ip[2] = 16;
    p_frame_head->src_eq.ip[3] = 172;
    p_frame_head->cmd = 0x0001;
    p_frame_head->len = 6;
    tmp_data[LEN_FRAME_HEAD] = get_osd_485st();  // running status 485err=1
    tmp_data[LEN_FRAME_HEAD + 1] = read_light(); // drmd light
    tmp_data[LEN_FRAME_HEAD + 2] = 0x00; // ver
    tmp_data[LEN_FRAME_HEAD + 3] = 0x00; // ver
    tmp_data[LEN_FRAME_HEAD + 4] = VERSION_MAX; // ver
    tmp_data[LEN_FRAME_HEAD + 5] = VERSION_MIN; // ver
    //while(1)
    {
        //rand_no = rand()%15*100000+500000;
        //usleep(rand_no);
        PiscWriteData(tmp_data, LEN_FRAME_HEAD + 6);
        //printf("-----piscSendAck ------- \n");
        //tmp_data[LEN_FRAME_HEAD] = get_osd_485st();  // running status 485err=1
        //tmp_data[LEN_FRAME_HEAD+1] = read_light(); // drmd light
    }
}

void *piscSendProc(void *arg)
{
    unsigned char buf[1024];
    unsigned long long rand_no;
    unsigned int temp_num;
    static unsigned int MyTmCnt500ms = 0;
    unsigned int CNT_500MS = 500;
    unsigned int RandCourt = 0;
    int len;
    sleep(5);
    buf[2] = read_light();
    unsigned char i;
    unsigned char tmp_data[200];
    unsigned char check_sum = 0;
    FRAME_HEAD *p_frame_head;
    p_frame_head = (FRAME_HEAD *)tmp_data;
    p_frame_head->frame_start = 0x7e;
    p_frame_head->dstn_eq.vehicle_num_l = 0xff;
    p_frame_head->dstn_eq.vehicle_num_h = 0xff;
    p_frame_head->dstn_eq.type = 0xff;
    p_frame_head->dstn_eq.num = 0xff;
    p_frame_head->dstn_eq.ip[0] = 255;
    p_frame_head->dstn_eq.ip[1] = 101;
    p_frame_head->dstn_eq.ip[2] = 168;
    p_frame_head->dstn_eq.ip[3] = 192;
    p_frame_head->src_eq.vehicle_num_l = (read_dev_id() - 14) / 10 + 1;
    p_frame_head->src_eq.vehicle_num_h = 0;
    p_frame_head->src_eq.type = 0x07;
    p_frame_head->src_eq.num = read_dev_id() - 13; // ip=14~93
    p_frame_head->src_eq.ip[0] = read_dev_id();
    p_frame_head->src_eq.ip[1] = 101;
    p_frame_head->src_eq.ip[2] = 168;
    p_frame_head->src_eq.ip[3] = 192;
    p_frame_head->cmd = 0x0001;
    p_frame_head->len = 6;
    //tmp_data[LEN_FRAME_HEAD] = get_osd_485st();  // running status 485err=1
    tmp_data[LEN_FRAME_HEAD] = 0;  // running status 485err=1
    tmp_data[LEN_FRAME_HEAD + 1] = read_light(); // drmd light
    tmp_data[LEN_FRAME_HEAD + 2] = 0x00; // ver
    tmp_data[LEN_FRAME_HEAD + 3] = 0x00; // ver
    tmp_data[LEN_FRAME_HEAD + 4] = VERSION_MAX; // ver
    tmp_data[LEN_FRAME_HEAD + 5] = VERSION_MIN; // ver
    temp_num = (p_frame_head->src_eq.vehicle_num_l - 1) * 10 + p_frame_head->src_eq.num + (p_frame_head->src_eq.type << 8);
    srand(temp_num);
    RandCourt = rand() % (CNT_500MS - 1) + 1;
    
    while (1)
    {
        myUsleep(1000);
        MyTmCnt500ms++;
        
        if (MyTmCnt500ms == RandCourt)
        {
            MyTmCnt500ms = 0;
            RandCourt = rand() % (500 - 1) + 1;
            
            if (RandCourt < 100)
                RandCourt += 100;
                
            tmp_data[LEN_FRAME_HEAD] = 0;  // running status 485err=1
            tmp_data[LEN_FRAME_HEAD + 1] = read_light(); // drmd light
            PiscWriteData(tmp_data, LEN_FRAME_HEAD + 6);
        }
        
        if (MyTmCnt500ms >= 500 )
        {
            MyTmCnt500ms = 0;
            printf("main==>>=500 ms MyTmCnt500ms:%d  RandCourt:%d \n", MyTmCnt500ms, RandCourt);
        }
    }
}

void *drmdRecvProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    int l_recvOk;
    int rest = -1;
    
    while (rest < 0)
    {
        rest = DrmdInit();
        usleep(1000);
    }
    
    while (1)
    {
        ret = DrmdReadData(buf, 1024);
        
        if (ret > 0)
        {
            l_recvOk = getPacket(buf, ret);
            
            //fun(buf,ret);
            //DrmdWriteData(buf,ret);
            if ((buf[LOCAL_START_OFFSET] == 0xFE) && (buf[LOCAL_TYPE_OFFSET] == DRMD2MAIN))
            {
                switch (buf[LOCAL_CMD_OFFSET])
                {
                    case DRMD_STATUS:
                        set_drmd_version(buf[LOCAL_DATA_OFFSET], buf[LOCAL_DATA_OFFSET + 1]);
                        set_drmd_display_st(buf[LOCAL_DATA_OFFSET + 2]);
                        break;
                        
                    default:
                        break;
                }
            }
        }
    }
}
void *osdRecvProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    int l_recvOk;
    int rest = -1;
    
    while (rest < 0)
    {
        rest = OsdInit();
        usleep(1000);
    }
    
    while (1)
    {
        ret = OsdReadData(buf, 1024);
        
        if (ret > 0)
        {
            l_recvOk = getPacket(buf, ret);
            
            if (l_recvOk < 0)
            {
                printf("main==>osdRecvErr");
                continue;
            }
            
            //printf("read osd data:\n");
            //fun(buf,ret);
            
            if ((buf[LOCAL_START_OFFSET] == 0xFE) && (buf[LOCAL_TYPE_OFFSET] == OSD2MAIN))
            {
                switch (buf[LOCAL_CMD_OFFSET])
                {
                    case OSD_STATUS:
                        set_osd_version(buf[LOCAL_DATA_OFFSET], buf[LOCAL_DATA_OFFSET + 1]);
                        set_osd_485st(buf[LOCAL_DATA_OFFSET + 2]);
                        
                        if (buf[LOCAL_DATA_OFFSET + 6] > 0)
                        {
                            if ((get_osd_CycleMode() != buf[LOCAL_DATA_OFFSET + 3])
                                || (get_osd_CycleTime() != buf[LOCAL_DATA_OFFSET + 4])
                                || (get_osd_CycleIndex() != buf[LOCAL_DATA_OFFSET + 5]))
                            {
                                if ((get_osd_CycleMode() == buf[LOCAL_DATA_OFFSET + 3]) && (get_osd_CycleMode() == 0))
                                {
                                    set_osd_CycleMode(buf[LOCAL_DATA_OFFSET + 3]);
                                    set_osd_CycleTime(buf[LOCAL_DATA_OFFSET + 4]);
                                    set_osd_CycleIndex(buf[LOCAL_DATA_OFFSET + 5]);
                                }
                                else
                                {
                                    set_osd_CycleMode(buf[LOCAL_DATA_OFFSET + 3]);
                                    set_osd_CycleTime(buf[LOCAL_DATA_OFFSET + 4]);
                                    set_osd_CycleIndex(buf[LOCAL_DATA_OFFSET + 5]);
                                    sendDrmdCyclePacket();
                                }
                            }
                        }
                        
                        break;
                        
                    case SET_IP_OSD:
                        break;
                        
                    case SET_LIGHT:
                        break;
                        
                    case SET_CONTRAST:
                        break;
                        
                    case SET_LCD:
                        set_osd_lcd_st(buf[LOCAL_DATA_OFFSET]);
                        break;
                        
                    case SET_MAC:
                        break;
                        
                    case SET_485:
                        PiscProc(&buf[LOCAL_DATA_OFFSET], buf[LOCAL_LEN_OFFSET], DATA_485);
                        break;
                        
                    default:
                        break;
                }
            }
        }
    }
}

typedef struct
{
    int udpRecvTime;//????UDP????????
    int udpSynConnect;//UDP????????????????????????
    int tcpSynConnect;//TCP????????????
    int tcpDisConnect;//??????????????????????????
    int tcpConnectState;//1=??????????0=????????
    int nowTime;
    int timer1Time;
    pthread_rwlock_t lock;//????????????????
    
} ptuStateSt;
static ptuStateSt ptuState;
static pthread_rwlock_t ptuLock;
int ptuTcpRecvTime = 0;
/*
????????
????????ptuTcp??????????????
????????????????????????
*/
int ptuTimeOutProc()
{
    //????????????????????
    static int connect_old = 0;
    
    if (1)
    {
        //ptu????
        int recvTime;
        time_t nowTime;
        int nosignal;
        int connect;
        connect = getTcpIsConnect();
        
        if (connect > 0)
        {
            if (connect_old != connect)
            {
                nowTime = time((time_t *)NULL);
                pthread_rwlock_wrlock(&ptuLock);
                ptuTcpRecvTime = nowTime;
                pthread_rwlock_unlock(&ptuLock);
                printf("main==>tcp connect nowTime:%d\n", nowTime);
                printf("main==>tcp connect ptuTcpRecvTime:%d\n", ptuTcpRecvTime);
            }
            
            nowTime = time((time_t *)NULL);
            pthread_rwlock_rdlock(&ptuLock);
            recvTime = ptuTcpRecvTime;
            pthread_rwlock_unlock(&ptuLock);
            
            if ((recvTime + 5 <= nowTime))
            {
                PtuTcpUnConnect();
                printf("main==>tcp time out nowTime:%d\n", nowTime);
                printf("main==>tcp time out ptuTcpRecvTime:%d\n", ptuTcpRecvTime);
                printf("main==>tcp time out\n");
            }
        }
        
        connect_old = connect;
    }
    
    //??10s????????ptu
    if (0)
    {
        //ptu????
        static int oldTime  = time((time_t *)NULL);;
        int nowTime;
        int connect = 1;
        connect = getTcpIsConnect();
        
        if (connect < 0)
        {
            nowTime = time((time_t *)NULL);
            
            if ((oldTime + 5 <= nowTime))
            {
                printf("main==>10s to connect ptu\n");
                int connect1;
                connect1 = PtuTcpconnect();
                
                if (connect1 > 0)
                {
                    //????????????????????
                    int nowTime;
                    nowTime = time((time_t *)NULL);
                    pthread_rwlock_wrlock(&ptuLock);
                    ptuTcpRecvTime = nowTime;
                    pthread_rwlock_unlock(&ptuLock);
                    printf("main==>10s to connect ptu ok\n");
                }
            }
        }
        else
        {
            oldTime = time((time_t *)NULL);
        }
    }
}
int ptuUdpProc(unsigned char *buf, int len)
{
    if (1)
    {
        //printf("ptu recv error data\n");
    }
    
    if (2)
    {
        //??????????
        int i = 0;
        int ret = -1;
        
        for (i = 0; i < len && 6 == len; i++)
        {
            if (buf[i] == i + 1)
            {
                ret = 1;
            }
            else
            {
                ret = -1;
            }
        }
        
        if (ret > 0)
        {
            //????????????????
            //printf("udpSynConnect\n");
            //????????????????????
            int connect;
            connect = PtuTcpconnect();
            
            if (connect > 0)
            {
                int nowTime;
                nowTime = time((time_t *)NULL);
                pthread_rwlock_wrlock(&ptuLock);
                ptuTcpRecvTime = nowTime;
                pthread_rwlock_unlock(&ptuLock);
            }
        }
    }
}
void *ptuUdpRecvProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    PtuUdpInit();
    printf("main==>ptuUdpRecvProc\n");
    
    while (1)
    {
        ret = PtuUdpReadData(buf, 1024);
        
        if (ret > 0)
        {
            //fun(buf,ret);
            ptuUdpProc(buf, ret);
            //PtuUdpWriteData(buf,ret);
        }
    }
}
int ptuTcpProc(unsigned char *buf, int len)
{
    if (1)
    {
        //printf("ptuTcpProc\n");
    }
    
    if (2)
    {
        ptuTcpRecvPacket(buf, len);
    }
}

void *ptuTcpRecvProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    PtuTcpconnect();
    
    while (1)
    {
        ret = PtuTcpReadData(buf, 1024); //????????
        
        if (ret > 0)
        {
            //??????put????????????????
            int nowTime;
            nowTime = time((time_t *)NULL);
            pthread_rwlock_wrlock(&ptuLock);
            ptuTcpRecvTime = nowTime;
            pthread_rwlock_unlock(&ptuLock);
            //printf("tcp lif nowTime:%d\n",nowTime);
            //printf("tcp lif  ptuTcpRecvTime:%d\n",ptuTcpRecvTime);
            //fun(buf,ret);
            ptuTcpProc(buf, ret);
        }
        else if (-2 == ret)
        {
            //??????????????????????????
            printf("main==>ptuTcpRecvProc error\n");
        }
        else if (-1 == ret)
        {
            sleep(1);//????TCP??????????1s??????????????????
        }
    }
}
/*
ptu tcp ??????????????
*/
void *ptuTcpSendProc(void *arg)
{
    //????????????????2s
    while (1)
    {
        sleep(2);
        ptuSendHeartbeat();
    }
}
void *timer300ms(void *arg)
{
    static unsigned int cnt = 0;
    sleep(3);
    
    while (1)
    {
        usleep(100 * 1000);
        
        if (++cnt % 10 == 1)
            drmdTimeOutProc();
            
        if (cnt % 20 == 1)
            ptuTimeOutProc();
    }
}
void *testSendProc(void *arg)
{
    unsigned char buf[1024];
    int len;
    int ret;
    sleep(2);
    
    while (0)
    {
        sleep(3);
        setCmd(SET_LEAVE);
        sendDrmdPacket();
        setLight(50);
        sleep(3);
        setCmd(SET_PRE);
        sendDrmdPacket();
        setContrast(60);
    }
}

void *ipCheckProc(void *arg)
{
    char buf[64] = {0};
    int ret;
    unsigned long long rand_no;
    unsigned int temp_num;
    static unsigned int MyTmCntMs = 0;
    unsigned int RandCourt = 0;
    static int ip_conflict_bak = 0;
    ret = GetValueFromEtcFile("/etc/net.conf", "netconfig", "IPADDR", buf, 512);
    
    if (ret >= 0)
    {
        printf("main==>ipCheckProc local ip: %s\n", buf);
    }
    
    initARP(buf);
    sleep(2);
    temp_num = read_dev_id() * 100;
    srand(temp_num);
    RandCourt = rand() % (DELAY_1000MS - 1) + 1;
    
    while (1)
    {
        // 1. ????????arp??????
        myUsleep(2000);
        MyTmCntMs++;
        
        if (MyTmCntMs == RandCourt)
        {
            MyTmCntMs = 0;
            RandCourt = rand() % (DELAY_1000MS - 1) + 1;
            
            if (RandCourt < 300)
                RandCourt += 300;
                
            arpSend(buf);
        }
        
        if (MyTmCntMs >= DELAY_1000MS )
        {
            MyTmCntMs = 0;
        }
        
#if 1
        ret = getIpconflict();
        
        if (ret)
        {
            if (ip_conflict_bak == 0)
            {
                setCmd(SET_IP_CONFLICT_ON);
                sendDrmdPacket();
            }
        }
        else
        {
            if (ip_conflict_bak)
            {
                setCmd(SET_IP_CONFLICT_OFF);
                sendDrmdPacket();
            }
        }
        
        ip_conflict_bak = ret;
#endif
    }
}

int startThread(pthread_t *fd, void *(*fun)(void *), void *arg)
{
    int ret;
    ret = pthread_create(fd, NULL, fun, arg);
    
    if (ret < 0)
    {
        perror("pthread_create");
        return -1;
    }
    
    return 1;
}
void dataInit()
{
    memset(&mainDevData, 0, sizeof(mainDevData));
    int nowTime = time((time_t *)NULL);
    mainDevData.netTime = nowTime;
    pthread_rwlock_init(&mainDevData.lock, NULL);
    memset(&ptuState, 0, sizeof(ptuState));
    pthread_rwlock_init(&ptuState.lock, NULL);
    pthread_rwlock_init(&ptuLock, NULL);
}

static void handle_pipe(int sig)
{
    perror("handle_pipe");
}
int signalInit()
{
    struct sigaction action;
    action.sa_handler = handle_pipe;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);
}
#include <sys/stat.h>
static int dirInit()
{
    int ret;
    ret = access("/root/socket", F_OK);
    
    if (ret < 0)
    {
        mkdir("/root/socket", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    
    return 1;
}
typedef struct
{
    char devId;
    char contrast;
    char Brightness;
    char ip[4];
    char mac[8];
    char ipstr[20];
} Config_St;
static Config_St g_config;
//??????????

#define DRMD_MAIN_VER_MAX 1
#define DRMD_MAIN_VER_MIN 1

int main(void)
{
    char buf[100];
    int temp;
    printf("main==>------drmd main Version:%d.%d 20191202------\n", VERSION_MAX, VERSION_MIN);
    printf("%s start:\n", __FUNCTION__);
    //????????????????
    dirInit();
    dataInit();
    PtuTcpDataInit();
    signalInit();
    read_ip(buf);
    read_mac(buf);
    read_light();
    read_contrast();
    read_dev_id();
    read_work_mode();
    temp = read_dev_id();
    initDevId(temp);
    temp = read_out_line_time();
    
    if (temp > 0)
        set_out_line_time(temp);
        
    temp = read_off_line_time();
    
    if (temp > 0)
        set_off_line_time(temp);
        
    //startThread(&vvsRecvThreadFd,vvsRecvProc,NULL);//vvs
    //startThread(&vvsSendThreadFd,vvsSendProc,NULL);//vvs
    startThread(&piscRecvThreadFd, piscRecvProc, NULL); //pisc
    startThread(&piscSendThreadFd, piscSendProc, NULL); //pisc
    startThread(&osdRecvThreadFd, osdRecvProc, NULL); //osd
    startThread(&ptuTcpRecvThreadFd, ptuTcpRecvProc, NULL); //ptu
    startThread(&ptuUdpRecvThreadFd, ptuUdpRecvProc, NULL); //ptu
    startThread(&ptuTcpSendThreadFd, ptuTcpSendProc, NULL); //ptu
    startThread(&drmdRecvThreadFd, drmdRecvProc, NULL); //drmd
    startThread(&testSendThreadFd, testSendProc, NULL); //test
    startThread(&timer300msFd, timer300ms, NULL); //timer300ms
    //startThread(&ipConflictFd,ipCheckProc,NULL);//timer300ms
    //sleep(10);
    //PtuTcpUnConnect();
    
    while (1)
    {
        sleep(2);
    }
    
    printf("%s end:\n", __FUNCTION__);
    return 0;
}
