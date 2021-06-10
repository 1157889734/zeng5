#ifndef __PTU_H__
#define __PTU_H__
int PtuUdpInit();
int PtuUdpReadData(unsigned char *buf,int len);
int PtuUdpWriteData(unsigned char *buf,int len);
#endif