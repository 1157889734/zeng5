#ifndef __OSD_PROC_H__
#define __OSD_PROC_H__

#define MAIN2OSD 0x01
#define OSD2MAIN 0x02

#define SEND_STATUS 0x00
#define SET_IP 0x01
#define SET_MAC 0x02
#define SET_LIGHT 0x03
#define SET_CONTRAST 0x04
#define SET_LCD 0x05
#define SET_485 0x06
#define SET_PWM 0x07

#define VERSION_MAX 1
#define VERSION_MIN 0

int SocketUnixInit();
int SocketUnixReadData(unsigned char *buf,int len);
int SocketUnixWriteData(unsigned char *buf,int len);
int setIP(unsigned char ip1,unsigned char ip2,unsigned char ip3,unsigned char ip4);
int setMac(unsigned char mac1,unsigned char mac2,unsigned char mac3,unsigned char mac4,unsigned char mac5,unsigned char mac6);
int setLight(unsigned char light);
int setContrast(unsigned char contrast);
int setLcd(unsigned char on_off);
int setPwm(unsigned char pwm);
void sendStatus(void);
void send485(unsigned char* pbuf,unsigned char len);
#endif
