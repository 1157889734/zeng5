#ifndef __MAIN_H__
#define __MAIN_H__

void set_out_line_time(int time);
int get_out_line_time(void);
void set_off_line_time(int time);
int get_off_line_time(void);


typedef struct
{
    unsigned char leaveFlag;
    unsigned char arrivedFlag;

    unsigned char startId;
    unsigned char currentId;
    unsigned char nextId;
    unsigned char endId;

    unsigned char emeId;
    unsigned char pisc_dev_id;
    unsigned char masterFlag;
    unsigned char key;    
    unsigned char nextOpenLeft;
    unsigned char nextOpenRight;
    unsigned char openleft;
    unsigned char openright;
    unsigned char openPiscId;
    unsigned char openleftPulse;
    unsigned char openrightPulse;
    unsigned char up;
    unsigned char down;
    unsigned char closeleft;
    unsigned char closeright;
    unsigned char doorerror;
    unsigned char skipActive;
    unsigned char upDownFlag;

    unsigned char skipStation[8];

    unsigned char chgFlag;
    unsigned char preFlag;
    unsigned char nosignal;       // wr 
    unsigned char closeLcd;      // wr 
    unsigned char devId;
    unsigned char doorflag;      // 1 开门，0关门     
    unsigned char year_l;
    unsigned char year_h;
    unsigned char month;
    unsigned char date;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;

    int leaveTime;
    int nowTime;
    int netTime;          // wr 
    int rs485Time;    
    int crcTime;
    pthread_rwlock_t lock;
}mainDevDataSt;

#endif
