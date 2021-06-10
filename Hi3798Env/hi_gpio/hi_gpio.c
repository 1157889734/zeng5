#if 0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_unf_gpio.h"
#include "hi_adp_boardcfg.h"
#else
#include "include.h"
#endif

#define CARNUM 6
#define DRMDNUM 10

void myUsleep(unsigned long usec)
{
    struct timeval tv;
    tv.tv_sec=usec/1000000;
    tv.tv_usec=usec%1000000;
    int err;
    do{
        err=select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}


static HI_U32 g_u32Multi_useRegAddr = 0;
static HI_U32 g_u32Multi_useValue;

int gpioTest(void)
{
    HI_S32 Ret;
    HI_U32 u32IntWaitTime = 5000;
    HI_U32 u32TestGpioNum ;
    HI_U32 u32IntValue = 0xff;
    HI_BOOL ReadBitVal;
    HI_U32 u32Value = 0;
    HI_U32 u32Group;
    HI_U32 u32Bit;
    HI_CHAR str[32];

    (HI_VOID)HI_SYS_Init();

    u32Group = 1;   // 组号 GPIO1_2 组号为1 
    u32Bit = 2;  // 组号 GPIO1_2 位号为 2
    u32TestGpioNum = u32Group*8+u32Bit; // 管脚号=组号*8+位号 GPIO1_2=1*8+2=10

    Ret = HI_UNF_GPIO_Init();
    if (HI_SUCCESS != Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, Ret);
        goto ERR0;
    }
	
    Ret = HI_UNF_GPIO_SetDirBit(u32TestGpioNum, HI_TRUE);   // 设置管脚为读
    if (HI_SUCCESS != Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, Ret);
        goto ERR1;
    }

    Ret = HI_UNF_GPIO_ReadBit(u32TestGpioNum, &ReadBitVal);  // 读io
    if (HI_SUCCESS != Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, Ret);
        goto ERR1;
    }

    printf("GPIO%d_%d Input: %d\n", u32Group, u32Bit, ReadBitVal);   // 读出来的值

    // ...... 后面根据上面接口自己编写 ^_^!  有问题参考sdk “03-HMS 开发指南” 中的GPIO章节

    return Ret;

ERR1:

    Ret = HI_UNF_GPIO_DeInit();
    if (HI_SUCCESS != Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, Ret);
    }

ERR0:
    //HI_SYS_WriteRegister(g_u32Multi_useRegAddr, g_u32Multi_useValue);
    //(HI_VOID)HI_SYS_DeInit();   // 其他地方在用，在主程序退出再调用
	
    return Ret;
}
int readGpio(int group,int bit)
{
    HI_S32 Ret;
    HI_U32 u32IntWaitTime = 5000;
    HI_U32 u32TestGpioNum ;
    HI_U32 u32IntValue = 0xff;
    HI_BOOL ReadBitVal;
    HI_U32 u32Value = 0;
    HI_U32 u32Group;
    HI_U32 u32Bit;
    HI_CHAR str[32];

    //(HI_VOID)HI_SYS_Init();

    u32Group = group;   // 组号 GPIO1_2 组号为1
    u32Bit = bit;  // 组号 GPIO1_2 位号为 2
    u32TestGpioNum = u32Group*8+u32Bit; // 管脚号=组号*8+位号 GPIO1_2=1*8+2=10

    Ret = HI_UNF_GPIO_Init();
    if (Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, Ret);
        return -1;
    }

    //测试 28G需要特殊处理
    if(u32TestGpioNum==28)
    {
        HI_SYS_WriteRegister(0xF8A21070,0x0000134);
    }

    Ret = HI_UNF_GPIO_SetDirBit(u32TestGpioNum, HI_TRUE);   // 设置管脚为读
    if (Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, Ret);
        return -1;
    }

    Ret = HI_UNF_GPIO_ReadBit(u32TestGpioNum, &ReadBitVal);  // 读io
    if (Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, Ret);
        return -1;
    }

    printf("GPIO%d_%d Input: %d\n", u32Group, u32Bit, ReadBitVal);   // 读出来的值
    Ret = (HI_S32)ReadBitVal;

    //Ret = HI_UNF_GPIO_DeInit();
    // ...... 后面根据上面接口自己编写 ^_^!  有问题参考sdk “03-HMS 开发指南” 中的GPIO章节

    return ReadBitVal;
}


void ipCheck(void)
{
    char str_set[32]={0};
    char str_get[32]={0};
    int drmdAddr[4];
    int carAddr[3];
    unsigned long int addr;
    int carID,drmdId,devID,devIDbak,cnt=0;

    while(1){
        #if 0
        drmdAddr[0] = readGpio(4,2);
        drmdAddr[1] = readGpio(4,1);
        drmdAddr[2] = readGpio(4,0);
        drmdAddr[3] = readGpio(3,7);
        carAddr[0] = readGpio(3,6);
        carAddr[1] = readGpio(3,5);
        carAddr[2] = readGpio(3,4);
        #else
        carAddr[0] = readGpio(4,2);
        carAddr[1] = readGpio(4,1);
        carAddr[2] = readGpio(4,0);
        drmdAddr[0] = readGpio(3,7);
        drmdAddr[1] = readGpio(3,6);
        drmdAddr[2] = readGpio(3,5);
        drmdAddr[3] = readGpio(3,4);
        #endif

        if((drmdAddr[0]<0)||(drmdAddr[1]<0)||(drmdAddr[2]<0)||(drmdAddr[3]<0)||(carAddr[0]<0)
            ||(carAddr[1]<0)||(carAddr[2]<0))
        {
            printf("readGpio err\n");
            cnt++;
            if(cnt>=2)
            {                
                devID = 0xffff;
            }
            else
                continue;
        }
        carID = (carAddr[2]|(carAddr[1]<<1)|(carAddr[0]<<2));
        drmdId = (drmdAddr[3]|(drmdAddr[2]<<1)|(drmdAddr[1]<<2)|(drmdAddr[0]<<3));
        //drmdId = (drmdAddr[2]|(drmdAddr[1]<<1)|(drmdAddr[0]<<2));
        devID = carID*DRMDNUM+drmdId+1;
        
        if((devID>CARNUM*DRMDNUM+1)||(carID>=CARNUM)||(drmdId>=DRMDNUM))
        {
            printf("gpio addr err\n");
            devID = 0xffff;
            //return;
            break;
        }

        if(devIDbak == devID){
            cnt++;
            printf("devID:%d \n",devID);
            if(cnt>=2)
                break;
        }
        else{
            cnt=0;
            devIDbak = devID;
            printf("ipChecking \n");
        }
    }

    if(devID<=255)
    {
        sprintf(str_set,"192.168.7.%d", devID);
        if(GetValueFromEtcFile(NET_INI, "netconfig", "IPADDR", str_get, sizeof(str_get)-1)==0){
        }
        if((memcmp(str_get,(char *)str_set,strlen(str_set))!=0)||(memcmp(str_get,(char *)str_set,strlen(str_get))!=0))
        {
            SetValueToEtcFile(NET_INI, "netconfig", "IPADDR", str_set);    

            sprintf(str_set,"255.255.0.0");
            SetValueToEtcFile(NET_INI, "netconfig", "NETMASK", str_set);

            sprintf(str_set,"12:34:C0:A8:7:%02x",devID);
            SetValueToEtcFile(NET_INI, "netconfig", "MAC", str_set);

            sprintf(str_set,"192.168.7.1");
            SetValueToEtcFile(NET_INI, "netconfig", "GATEWAY", str_set);
            
            system("reboot");
        }       
    }
	
	/*检查ip 子网掩码是否正确*/
    int rebootFlag = 0;
    if(GetValueFromEtcFile(NET_INI, "netconfig", "NETMASK", str_get, sizeof(str_get)-1)==0){
         sprintf(str_set,"255.255.0.0");
    }
    if((memcmp(str_get,(char *)str_set,strlen(str_set))!=0)||(memcmp(str_get,(char *)str_set,strlen(str_get))!=0))
    {
        sprintf(str_set,"255.255.0.0");
        SetValueToEtcFile(NET_INI, "netconfig", "NETMASK", str_set);  
        printf("mask chg\n");
        rebootFlag = 1;
        //system("reboot");
    }  

    if(GetValueFromEtcFile(NET_INI, "netconfig", "GATEWAY", str_get, sizeof(str_get)-1)==0){
         sprintf(str_set,"192.168.7.1");
    }
    if((memcmp(str_get,(char *)str_set,strlen(str_set))!=0)||(memcmp(str_get,(char *)str_set,strlen(str_get))!=0))
    {
        sprintf(str_set,"192.168.7.1");
        SetValueToEtcFile(NET_INI, "netconfig", "GATEWAY", str_set);  
        printf("gateway chg\n");
        rebootFlag = 1;
        //system("reboot");
    }  

    sprintf(str_set,"192.168.7");
    if(GetValueFromEtcFile(NET_INI, "netconfig", "IPADDR", str_get, sizeof(str_get)-1)==0){
    }
    if((memcmp(str_get,(char *)str_set,strlen(str_set))!=0))
    {
        unsigned int temp = (unsigned int)inet_addr(str_get);
        char *ptemp = (char *)&temp;
        printf("read_ip=%d.%d.%d.%d\n",ptemp[0],ptemp[1],ptemp[2],ptemp[3]);
        printf("ip seg is not correct\n");

        sprintf(str_set,"192.168.7.%d",ptemp[3]);
        SetValueToEtcFile(NET_INI, "netconfig", "IPADDR", str_set);    

        sprintf(str_set,"255.255.0.0");
        SetValueToEtcFile(NET_INI, "netconfig", "NETMASK", str_set);

        sprintf(str_set,"12:34:C0:A8:7:%02x",devID);
        SetValueToEtcFile(NET_INI, "netconfig", "MAC", str_set);

        sprintf(str_set,"192.168.7.1");
        SetValueToEtcFile(NET_INI, "netconfig", "GATEWAY", str_set);

        rebootFlag = 1;
        //system("reboot");
    }  

    if(rebootFlag)
    {
        printf("rebootFlag\n");
        system("reboot");
    }
}
HI_S32 writeGpio(int group,int bit, HI_BOOL valu)
{
    HI_S32 Ret;
    HI_U32 u32TestGpioNum ;
    HI_U32 u32Group;
    HI_U32 u32Bit;

    Ret = HI_UNF_GPIO_Init();
    if (Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, Ret);
        return Ret;
    }
    u32Group = group;   // 组号 GPIO1_2 组号为1
    u32Bit = bit;  // 组号 GPIO1_2 位号为 2
    u32TestGpioNum = u32Group*8+u32Bit; // 管脚号=组号*8+位号 GPIO1_2=1*8+2=10

    Ret = HI_UNF_GPIO_SetDirBit(u32TestGpioNum, HI_FALSE);   // 设置管脚为写
    if (Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, Ret);
        return Ret;
    }

    Ret = HI_UNF_GPIO_WriteBit(u32TestGpioNum, valu);  // 读io
    if (Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, Ret);
        return Ret;
    }

    //Ret = HI_UNF_GPIO_DeInit();

    return HI_SUCCESS;
}

#define WDG_ENABLE 1
int led_proc(int argc, char *argv[])
{
    static int runLedSt=0;
    static unsigned int runCntMs=0;

    
    #if WDG_ENABLE    // WDG
    HI_S32 s32Ret;
    HI_U32 u32Loop;
    HI_U32 u32Value;

    /* Open WDG*/
    s32Ret = HI_UNF_WDG_Init();
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    u32Value = 10000;    //ms
    s32Ret = HI_UNF_WDG_SetTimeout(0, u32Value);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, s32Ret);
    }

    /* Enable WDG */
    s32Ret = HI_UNF_WDG_Enable(0);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, s32Ret);
    }
    HI_UNF_WDG_Clear(0);
    printf("%s: %d wdg start\n", __FILE__, __LINE__, s32Ret);    
    #endif
    
    printf("led disp start\n");
    sleep(2);
    while(1)
    {
        myUsleep(1000);
        runCntMs++;

        if(runCntMs%500 ==0)
        {
            if(runLedSt)
            {   
                writeGpio(4,6,HI_FALSE);
                runLedSt = 0;
            }
            else
            {
                writeGpio(4,6,HI_TRUE);
                runLedSt = 1;
            }

#if WDG_ENABLE
    s32Ret = HI_UNF_WDG_Clear(0);
    if (s32Ret != HI_SUCCESS)
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);            
#endif
        }        
    }
}

static pthread_t    g_ledproc;
//static pthread_t    g_mainSend;
int ledproc(void)
{
    pthread_create(&g_ledproc, HI_NULL, (HI_VOID *)led_proc, HI_NULL);
    //pthread_create(&g_mainSend, HI_NULL, (HI_VOID *)sample_main_send, HI_NULL);
     //getchar();
}


