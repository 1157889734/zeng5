#include "include.h"

#define FRAME_START_OFFSET 0
#define FRAME_TYPE_OFFSET 1
#define FRAME_CMD_OFFSET 2
#define FRAME_LEN_OFFSET 3
#define FRAME_DATA_OFFSET 4

//#define SET_IP 1




static void set_ip(unsigned char ip1,unsigned char ip2,unsigned char ip3,unsigned char ip4){
    char strGet[32]={0};
    char strSet[32]={0};
    int cnt=0;
    unsigned long int addr=0;
    unsigned char readValu[4]={0,0,0,0};

    sprintf(strSet,"%d.%d.%d.%d",ip1,ip2,ip3,ip4);

    while(1){         
        SetValueToEtcFile(NET_INI, "netconfig", "IPADDR", strSet);        
        if(GetValueFromEtcFile(NET_INI, "netconfig", "IPADDR", strGet, sizeof(strGet)-1)==0){
            addr = inet_addr(strGet);
            readValu[0]= addr&0xff;
            readValu[1]= (addr>>8)&0xff;
            readValu[2]= (addr>>16)&0xff;
            readValu[3]= (addr>>24)&0xff;
        }

        if((readValu[0]==ip1)&&(readValu[1]==ip2)&&(readValu[2]==ip3)&&(readValu[3]==ip4)){
            setIp(ip1,ip2,ip3,ip4);
            printf("setIp =%d.%d.%d.%d\n",ip1,ip2,ip3,ip4);
            break;
        }
      
        if(++cnt>10){
            printf("setIp err\n");
            break;
        }
    }   

}

static void set_brightness(unsigned char valu){
    char strGet[32]={0};
    char strSet[32]={0};
    int cnt=0;
    unsigned char readValu=0;

    sprintf((char*)strSet,"%d",valu);

    while(1){         
        SetValueToEtcFile(OSD_INI, "OSD", "Brightness", strSet);        
        if(GetValueFromEtcFile(OSD_INI, "OSD", "Brightness", strGet, sizeof(strGet)-1)==0){
            readValu= atoi(strGet);
        }

        if(readValu==valu){
            HI_UNF_DISP_SetBrightness(HI_UNF_DISPLAY1,valu);
            setLight(valu);
            printf("setBrightness =%d\n",valu);
            break;
        }
      
        if(++cnt>10){
            printf("setBrightness err\n");
            break;
        }
    }    
}

static void set_contrast(unsigned char valu){
    char strGet[32]={0};
    char strSet[32]={0};
    int cnt=0;
    unsigned char readValu=0;

    sprintf((char*)strSet,"%d",valu);

    while(1){         
        SetValueToEtcFile(OSD_INI, "OSD", "Contrast", strSet);        
        if(GetValueFromEtcFile(OSD_INI, "OSD", "Contrast", strGet, sizeof(strGet)-1)==0){
            readValu= atoi(strGet);
        }

        if(readValu==valu){
            HI_UNF_DISP_SetBrightness(HI_UNF_DISPLAY1,valu);
            setContrast(valu);
            printf("Contrast =%d\n",valu);
            break;
        }
      
        if(++cnt>10){
            printf("Contrast err\n");
            break;
        }
    }    
}

static void set_pwm(unsigned char valu){
    char strGet[32]={0};
    char strSet[32]={0};
    int cnt=0;
    unsigned char readValu=0;

    sprintf((char*)strSet,"%d",valu);

    while(1){         
        SetValueToEtcFile(OSD_INI, "OSD", "Pwm", strSet);        
        if(GetValueFromEtcFile(OSD_INI, "OSD", "Pwm", strGet, sizeof(strGet)-1)==0){
            readValu= atoi(strGet);
        }

        if(readValu==valu){
            //HI_UNF_DISP_SetBrightness(HI_UNF_DISPLAY1,valu);
            //Hi_SetPwm(valu);
            back_light_set_valu(valu);
            setPwm(valu);
            printf("Pwm =%d\n",valu);
            break;
        }
      
        if(++cnt>10){
            printf("Pwm err\n");
            break;
        }
    }    
}

static void set_rs485(unsigned char valu)
{
    // 启动485线程
    set485BegFlag(valu);
}


static void set_lcd(unsigned char valu){
    if(valu==1)   // 1 开屏 2关屏
        back_light_power(ON);

    if(valu==2)
        back_light_power(OFF);

    setLcd(valu);
}

static void set_mac(unsigned char mac1,unsigned char mac2,unsigned char mac3,unsigned char mac4,unsigned char mac5,unsigned char mac6){
    char strGet[32]={0};
    char strSet[32]={0};
    int cnt=0;
    unsigned long int addr;
    unsigned char readValu[6]={0x00,0x00,0x00,0x00,0x00,0x00};

    sprintf(strSet,"%02x:%02x:%02x:%02x:%02x:%02x",mac1,mac2,mac3,mac4,mac5,mac6);

    while(1){         
        SetValueToEtcFile(NET_INI, "netconfig", "MAC", strSet);        
        if(GetValueFromEtcFile(NET_INI, "netconfig", "MAC", strGet, sizeof(strGet)-1)==0){
            sscanf(strGet,"%02x:%02x:%02x:%02x:%02x:%02x",&readValu[0],&readValu[1],&readValu[2],&readValu[3],&readValu[4],&readValu[5]);
        }

        if((readValu[0]==mac1)&&(readValu[1]==mac2)&&(readValu[2]==mac3)&&(readValu[3]==mac4)&&(readValu[4]==mac5)&&(readValu[5]==mac6)){
            setMac(mac1,mac2,mac3,mac4,mac5,mac6);
            printf("setMac =%02x:%02x:%02x:%02x:%02x:%02x\n",mac1,mac2,mac3,mac4,mac5,mac6);
            break;
        }
      
        if(++cnt>10){
            printf("setMac err\n");
            break;
        }
    }   

}

int sample_main(void)
{
    unsigned char buf[1024];
    int len,i;
    int ret=-1;

    while(ret<0)
    {
        ret = SocketUnixInit();
        usleep(1000);
    }    
    printf("sample_main start\n");
    
    while(1){
        len= SocketUnixReadData(buf, 1024);
        //printf("len=%02x\n",len);
        if(len>=8){

            //printf("SocketUnixReadData:\n");
            // for(i=0;i<len;i++){
            //    printf("%02x",buf[i]);
            //}
            //printf("\n");
            
            if((buf[FRAME_START_OFFSET]==0xFE)&&(buf[FRAME_TYPE_OFFSET]==MAIN2OSD)){
                switch(buf[FRAME_CMD_OFFSET]){
                    case SET_IP:
                        set_ip(buf[FRAME_DATA_OFFSET],buf[FRAME_DATA_OFFSET+1],buf[FRAME_DATA_OFFSET+2],buf[FRAME_DATA_OFFSET+3]);
                        break;

                    case SET_LIGHT:
                        set_brightness(buf[FRAME_DATA_OFFSET]);
                        break;

                    case SET_CONTRAST:
                        set_contrast(buf[FRAME_DATA_OFFSET]);
                        break;

                    case SET_PWM:
                        set_pwm(buf[FRAME_DATA_OFFSET]);
                        break;

                    case SET_LCD:
                        set_lcd(buf[FRAME_DATA_OFFSET]);
                        break;
                        
                    case SET_MAC:
                        set_mac(buf[FRAME_DATA_OFFSET],buf[FRAME_DATA_OFFSET+1],buf[FRAME_DATA_OFFSET+2],buf[FRAME_DATA_OFFSET+3],buf[FRAME_DATA_OFFSET+4],buf[FRAME_DATA_OFFSET+5]);
                        break;

                    case SET_485:
                        set_rs485(buf[FRAME_DATA_OFFSET]);
                        break;   

                    default:
                        break;                  
                        
                }
            }
        }
        else
            usleep(2000);
        usleep(1000);
    }    
}

int sample_main_send(void)
{
    sleep(10);
    while(1)
    {
        sendStatus();
        usleep(500*1000);
    }
}

static pthread_t    g_mainTest;
static pthread_t    g_mainSend;
int mainTest(void)
{
    pthread_create(&g_mainTest, HI_NULL, (HI_VOID *)sample_main, HI_NULL);
    pthread_create(&g_mainSend, HI_NULL, (HI_VOID *)sample_main_send, HI_NULL);
     //getchar();
    return 0;
}
