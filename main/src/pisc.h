#ifndef __PISC_H__
#define __PISC_H__

typedef unsigned char				uint8;
typedef unsigned short				uint16;
typedef unsigned int				uint32;

typedef char						int8;
typedef short						int16;
typedef int						int32;

#define stPACKED					__attribute__((packed))

#define LEN_FRAME_HEAD				21

#define TYPE_PISC					0x01
#define TYPE_DACU					0x02
#define TYPE_TCMS_INTERFACE			0x03
#define TYPE_DRMD					0x07

typedef struct
{
    uint8 frame_start;						// 分隔符		7e

    struct
    {
        uint8 vehicle_num_l;			
        uint8 vehicle_num_h;		
        uint8 type;					
        uint8 num;					
        uint8 ip[4];					
    }stPACKED dstn_eq;

    struct
    {
        uint8 vehicle_num_l;			
        uint8 vehicle_num_h;		
        uint8 type;					
        uint8 num;					
        uint8 ip[4];					
    }stPACKED src_eq;

    uint16 cmd;						// 控制码
    uint16 len;						// 字长
}stPACKED FRAME_HEAD;

typedef struct
{
    FRAME_HEAD frame_head;
    unsigned char signal_mk;      // bit: master key 5 4 3 2 1 0
    unsigned char signal_rundoor; // bit:up down orightflag oleftflag cright cleft nextOright nextOleft
    unsigned char signal_runst; // bit: leave arrived 5 4 3 2 1 startflag
    unsigned char reserve_4;
    unsigned char start;
    unsigned char end;
    unsigned char current;
    unsigned char next;    
    unsigned char reserve_9;
    unsigned char drmd_light;
    unsigned char reserve_11;
    unsigned char skip1_32[4];
    unsigned char reserve3_16_19[4];
    unsigned char emeId;
    unsigned char drmd_display_cmd;
    unsigned char skip33_64[4];
    unsigned char fire_alarm[8];
    unsigned char reserve3_34_35[2];
    unsigned char special_id;
    unsigned char reserve_37;
    
    //unsigned char tc1_door1_8_unlock;
    //unsigned char car1_8_door1_8_unlock[8]; // 车厢1-8 的1-8门解锁
    //unsigned char car2_door1_8_unlock;
    //unsigned char car3_door1_8_unlock;
    //unsigned char car4_door1_8_unlock;
    //unsigned char car5_door1_8_unlock;
    //unsigned char car6_door1_8_unlock;
    //unsigned char car7_door1_8_unlock;   
    //unsigned char car8_door1_8_unlock;       
    //unsigned char tc2_door1_8_unlock;

    //unsigned char car1_8_door9_10_unlock[2]; // 车厢1-8 的9-10门解锁
    //unsigned char tc1_2_door9_10_unlock;

    //unsigned char car1_8_door_isolation[16];  // 第1个byte:1-8门解锁 第2个byte:9-10门解锁 

}stPACKED PiscRecvSt;


int PiscInit();
int PiscReadData(unsigned char *buf,int len);
int PiscWriteData(unsigned char *buf,int len);
#endif
