#ifndef __IPCHECK_H__
#define __IPCHECK_H__

//…Ë÷√IP
int ipCheckMain(char *addr);
int initARP(char *addrc);
void arpSend(char *addrc);
int getIpconflict(void);

#endif
