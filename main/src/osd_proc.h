#ifndef __OSD_PROC_H__
#define __OSD_PROC_H__

#define MAIN2OSD 0x01
#define OSD2MAIN 0x02

#define OSD_STATUS 0x00
#define SET_IP_OSD 0x01
#define SET_MAC 0x02
#define SET_LIGHT 0x03
#define SET_CONTRAST 0x04
#define SET_LCD 0x05
#define SET_485 0x06
#define SET_PWM 0x07

#define LCD_IDLE 0
#define LCD_ON 1
#define LCD_OFF 2   

#define RS485_ON 1
#define RS485_OFF 0  

#define LOCAL_START_OFFSET 0
#define LOCAL_TYPE_OFFSET 1
#define LOCAL_CMD_OFFSET 2
#define LOCAL_LEN_OFFSET 3
#define LOCAL_DATA_OFFSET 4

int OsdInit();
int OsdReadData(unsigned char *buf,int len);
int OsdWriteData(unsigned char *buf,int len);
int setIp(unsigned char ip1,unsigned char ip2,unsigned char ip3,unsigned char ip4);
int setMac(unsigned char mac1,unsigned char mac2,unsigned char mac3,unsigned char mac4,unsigned char mac5,unsigned char mac6);
int setLight(unsigned char light);
int setContrast(unsigned char contrast);
int setLcd(unsigned char on_off);
int set485(unsigned char beg485);
int setPwm(unsigned char pwm);
#endif
