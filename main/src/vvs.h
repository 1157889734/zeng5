#ifndef __VVS_H__
#define __VVS_H__
#include "udp.h"
int VvsInit();
int VvsReadData(unsigned char *buf,int len);
int VvsWriteData(unsigned char *buf,int len);
#endif