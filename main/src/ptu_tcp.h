#ifndef __PTU_TCP_H__
#define __PTU_TCP_H__
int PtuTcpDataInit();
int getTcpIsConnect();
int PtuTcpconnect();
int PtuTcpUnConnect();
int PtuTcpClear();
int PtuTcpReadData(unsigned char *buf,int len);
int PtuTcpWriteData(unsigned char *buf,int len);
#define GET_STATE 0x0100  //      获取设备状态
#define GET_SYS_SIZE 0x0200 //    获取系统emmc大小状态
#define GET_VERSION 0x0300  //    获取版本
#define DEV_CTRL 0x0400   //      设备控制
#define GET_CTRL_TYPE 0x0500 //   获取控制类型
#define SET_CTRL_TYPE 0x0600  //  设置控制类型
#define GET_BACK_LIGHT 0x0700  // 获取背光亮度
#define SET_BACK_LIGHT 0x0800  // 设置背光亮度
#define GET_CONTRAST 0x0900   // 获取对比度
#define PTU_SET_CONTRAST 0x0A00  // 设置对比度
#define SET_IP 0x0B00   //  设置IP
#define GET_MAC 0x0C00  //  获取MAC地址
#define FORMAT_SD 0x1000
#define GET_OFFLINE_TIME 0x1100 // 设置列车退出运营时间命令
#define SET_OFFLINE_TIME 0x1200 // 设置列车无信号关屏时间命令
#define GET_OUTLINE_TIME 0x1300 // 设置列车退出运营时间命令
#define SET_OUTLINE_TIME 0x1400 // 设置列车退出运营时间命令
#define UPDATE_START 0x1500 // 
#define UPDATE_END 0x1600 // 
#define GET_BACK_PWM 0x1700  // 获取背光占空比
#define SET_BACK_PWM 0x1800  // 设置背光占空比
#define SET_CHECK_LCD 0x2000  // 显示测试
#define GET_CHECK_LCD 0x2100

#define GET_SYSTEM_LCD 0x2200
#define GET_SD_SIZE 0x2300

#define SET_TIME_DISP 0x2400
#define GET_TIME_DISP 0x2500

#define HEARTBEAT 0x9900  //心跳
int ptuSendHeartbeat();
int ptuTcpRecvPacket(unsigned char *buf,int size);
void ptuTcpSendData(short cmd,char *buff,unsigned int len);
#endif
