#ifndef __PTU_TCP_H__
#define __PTU_TCP_H__
int PtuTcpDataInit();
int getTcpIsConnect();
int PtuTcpconnect();
int PtuTcpUnConnect();
int PtuTcpClear();
int PtuTcpReadData(unsigned char *buf,int len);
int PtuTcpWriteData(unsigned char *buf,int len);
#define GET_STATE 0x0100  //      ��ȡ�豸״̬
#define GET_SYS_SIZE 0x0200 //    ��ȡϵͳemmc��С״̬
#define GET_VERSION 0x0300  //    ��ȡ�汾
#define DEV_CTRL 0x0400   //      �豸����
#define GET_CTRL_TYPE 0x0500 //   ��ȡ��������
#define SET_CTRL_TYPE 0x0600  //  ���ÿ�������
#define GET_BACK_LIGHT 0x0700  // ��ȡ��������
#define SET_BACK_LIGHT 0x0800  // ���ñ�������
#define GET_CONTRAST 0x0900   // ��ȡ�Աȶ�
#define PTU_SET_CONTRAST 0x0A00  // ���öԱȶ�
#define SET_IP 0x0B00   //  ����IP
#define GET_MAC 0x0C00  //  ��ȡMAC��ַ
#define FORMAT_SD 0x1000
#define GET_OFFLINE_TIME 0x1100 // �����г��˳���Ӫʱ������
#define SET_OFFLINE_TIME 0x1200 // �����г����źŹ���ʱ������
#define GET_OUTLINE_TIME 0x1300 // �����г��˳���Ӫʱ������
#define SET_OUTLINE_TIME 0x1400 // �����г��˳���Ӫʱ������
#define UPDATE_START 0x1500 // 
#define UPDATE_END 0x1600 // 
#define GET_BACK_PWM 0x1700  // ��ȡ����ռ�ձ�
#define SET_BACK_PWM 0x1800  // ���ñ���ռ�ձ�
#define SET_CHECK_LCD 0x2000  // ��ʾ����
#define GET_CHECK_LCD 0x2100

#define GET_SYSTEM_LCD 0x2200
#define GET_SD_SIZE 0x2300

#define SET_TIME_DISP 0x2400
#define GET_TIME_DISP 0x2500

#define HEARTBEAT 0x9900  //����
int ptuSendHeartbeat();
int ptuTcpRecvPacket(unsigned char *buf,int size);
void ptuTcpSendData(short cmd,char *buff,unsigned int len);
#endif
