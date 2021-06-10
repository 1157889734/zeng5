#ifndef __GPIO_H__
#define __GPIO_H__

//#include "include.h"
int ledproc(void);
void myUsleep(unsigned long usec);
void ipCheck(void);
int readGpio(int group,int bit);
HI_S32 writeGpio(int group, int bit, HI_BOOL valu);
#endif
