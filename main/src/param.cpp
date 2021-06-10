/*
参数管理模块
*/
#include "param.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>

#include "ini_file.h"
#include "drmd_ui.h"
#include "main.h"



#define NET_CONF "/etc/net.conf"
#define OSD_INI "/root/config/osd.ini"

using namespace std;


static int devid = 101;
static unsigned long local_ipaddr;

int socket_getlocalip (unsigned long *local_ip)
{
    struct ifreq ifr;
    int inet_sock = -1;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (inet_sock < 0)
    {
        printf("getlocalhostip get socket failed sock_fd:%d \n", inet_sock);    //test
        return 1;
    }
    
    strcpy(ifr.ifr_name, "eth0");
    
    if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0)    //get ip failed
    {
        perror("ioctl");
        printf("get local ip failed \n");    //test
        close(inet_sock);
        return 2;
    }
    
    memcpy(local_ip, &(((struct sockaddr_in *) & (ifr.ifr_addr))->sin_addr), sizeof(unsigned long));
    close(inet_sock);
    return 0;
}
unsigned long getlocalip (void)
{
    socket_getlocalip(&local_ipaddr);
    return local_ipaddr;
}




/*
读ip地址
return -1  失败1 成功
ip 保存ip地址信息，4字节
*/
int read_ip(char *ip)
{
    char buf[512];
    int ret;
    ret = GetValueFromEtcFile(NET_CONF, "netconfig", "IPADDR", buf, 512);
    
    if (ret >= 0)
    {
        printf("IPADDR=%s\n", buf);
        unsigned int temp = (unsigned int)inet_addr(buf);
        char *ptemp = (char *)&temp;
        printf("read_ip=%d.%d.%d.%d\n", ptemp[0], ptemp[1], ptemp[2], ptemp[3]);
        
        for (int i = 0; i < 4; i++)
        {
            ip[i] = ptemp[i];
        }
        
        devid = ip[3];
    }
    
    return 1;
}

/*
 * 返回值: 成功：0  失败 ：非零
 */
int exec_cmd_popen(char *cmd)
{
    FILE *fp = NULL;
    int rc = 0; // 用于接收命令返回值
    
    if (cmd == NULL)
    {
        return (-1);
    }
    
    fp = popen(cmd, "r");
    
    if (fp == NULL)
    {
        printf("popen error.\n");
        return (-1);
    }
    
#if 0
    //打印命令输出信息,可以对出处做进一步处理,这里只
    char pInfo[PATH_SIZE];
    memset(pInfo, 0, sizeof(pInfo));
    
    while (!feof(fp))
    {
        if (fgets(pInfo, sizeof(pInfo), fp) != NULL)
        {
            printf("%s", pInfo);
        }
    }
    
#endif
    rc = pclose(fp);
    
    if ( rc == -1)
    {
        printf("close fp error.\n");
        return (-1);
    }
    else
    {
        //        printf ("%s\t return value: %d\n",cmd, WEXITSTATUS(rc));
    }
    
    return (WEXITSTATUS(rc));
}

int read_mac(char *mac)
{
    char buf[512];
    char strGet[32] = {0};
    char strSet[32] = {0};
    int cnt = 0;
    unsigned long int addr;
    unsigned char readValu[6];
    unsigned char temp[6];
    int ret;
    ret = GetValueFromEtcFile(NET_CONF, "netconfig", "MAC", buf, 512);
    
    if (ret >= 0)
    {
        sscanf(buf, "%2x:%2x:%2x:%2x:%2x:%2x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);
        printf("read_mac=%2x:%2x:%2x:%2x:%2x:%2x\n", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
        
        if (temp[5] != devid)
        {
            temp[5] = devid ;
            printf("mac chg \n");
            sprintf(strSet, "%02x:%02x:%02x:%02x:%02x:%02x", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
            
            while (1)
            {
                SetValueToEtcFile(NET_CONF, "netconfig", "MAC", strSet);
                
                if (GetValueFromEtcFile(NET_CONF, "netconfig", "MAC", strGet, sizeof(strGet) - 1) == 0)
                {
                    sscanf(strGet, "%02x:%02x:%02x:%02x:%02x:%02x", &readValu[0], &readValu[1], &readValu[2], &readValu[3], &readValu[4], &readValu[5]);
                }
                
                if ((readValu[0] == temp[0]) && (readValu[1] == temp[1]) && (readValu[2] == temp[2]) && (readValu[3] == temp[3]) && (readValu[4] == temp[4]) && (readValu[5] == temp[5]))
                {
                    printf("set2Mac =%02x:%02x:%02x:%02x:%02x:%02x\n", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
                    break;
                }
                
                if (++cnt > 10)
                {
                    printf("set2Mac err\n");
                    break;
                }
            }
            
            exec_cmd_popen("sh /etc/net_set");
        }
        
        for (int i = 0; i < 6; i++)
        {
            mac[i] = temp[i];
        }
    }
    
    return 1;
}
int read_light()
{
    char buf[512];
    int ret;
    ret = GetValueFromEtcFile(OSD_INI, "OSD", "Brightness", buf, 512);
    
    if (ret >= 0)
    {
        //printf("Brightness=%s\n",buf);
        int temp;
        sscanf(buf, "%d", &temp);
        //printf("read_light=%d\n",temp);
        return temp;
    }
    else
    {
        return -1;
    }
}

int read_pwm()
{
    char buf[512];
    int ret;
    ret = GetValueFromEtcFile(OSD_INI, "OSD", "Pwm", buf, 512);
    
    if (ret >= 0)
    {
        //printf("Brightness=%s\n",buf);
        int temp;
        sscanf(buf, "%d", &temp);
        //printf("read_light=%d\n",temp);
        return temp;
    }
    else
    {
        return -1;
    }
}

int read_contrast()
{
    char buf[512];
    int ret;
    ret = GetValueFromEtcFile(OSD_INI, "OSD", "Contrast", buf, 512);
    
    if (ret >= 0)
    {
        printf("Contrast=%s\n", buf);
        int temp;
        sscanf(buf, "%d", &temp);
        printf("read_contrast=%d\n", temp);
        return temp;
    }
    else
    {
        return -1;
    }
}
int read_dev_id()
{
    return devid;
}
int read_run_state()
{
    int cmd;
    cmd = getDisplayCmd();
    
    if (cmd == SET_CLOSE_LCD)
        cmd = LCD_SLEEP;
    else
        cmd = LCD_ONLINE;
        
    return cmd;
}
int read_work_mode()
{
}

static int drmd_ver_max = 0, drmd_ver_min = 0;

int set_drmd_version(int max, int min)
{
    drmd_ver_max = max;
    drmd_ver_min = min;
}

static int osd_ver_max = 0, osd_ver_min = 0;

int set_osd_version(int max, int min)
{
    osd_ver_max = max;
    osd_ver_min = min;
}

static unsigned char osd485st = 0;
void set_osd_485st(unsigned char valu)
{
    osd485st = valu;
}
unsigned char get_osd_485st(void)
{
    return osd485st ;
}

static unsigned char osdCycleMode = 0;
void set_osd_CycleMode(unsigned char valu)
{
    osdCycleMode = valu;
}
unsigned char get_osd_CycleMode(void)
{
    return osdCycleMode ;
}

static unsigned char osdCycleTime = 2;
void set_osd_CycleTime(unsigned char valu)
{
    osdCycleTime = valu;
}
unsigned char get_osd_CycleTime(void)
{
    return osdCycleTime ;
}

static unsigned char osdCycleIndex = 0;
void set_osd_CycleIndex(unsigned char valu)
{
    osdCycleIndex = valu;
}
unsigned char get_osd_CycleIndex(void)
{
    return osdCycleIndex ;
}

int read_version(char *version)
{
    version[0] = VERSION_MAX;
    version[1] = VERSION_MIN;
    version[2] = drmd_ver_max;
    version[3] = drmd_ver_min;
    version[4] = osd_ver_max;
    version[5] = osd_ver_min;
    return 1;
}

void write_off_line_time(int  valu)
{
    char strGet[32] = {0};
    char strSet[32] = {0};
    int cnt = 0;
    int readValu;
    sprintf((char *)strSet, "%d", valu);
    set_off_line_time(valu);
    
    while (1)
    {
        SetValueToEtcFile(OSD_INI, "OSD", "OffLineTime", strSet);
        
        if (GetValueFromEtcFile(OSD_INI, "OSD", "OffLineTime", strGet, sizeof(strGet) - 1) == 0)
        {
            readValu = atoi(strGet);
        }
        
        if (readValu == valu)
        {
            printf("OffLineTime =%d\n", valu);
            break;
        }
        
        if (++cnt > 10)
        {
            printf("OffLineTime err\n");
            break;
        }
    }
}

int read_off_line_time(void)
{
    char buf[100];
    int ret;
    int temp;
    ret = GetValueFromEtcFile(OSD_INI, "OSD", "OffLineTime", buf, 100);
    
    if (ret >= 0)
    {
        printf("OffLineTime=%s\n", buf);
        sscanf(buf, "%d", &temp);
        printf("read_OffLineTime=%d\n", temp);
        return temp;
    }
    else
    {
        printf("read_OffLineTime err\n", temp);
        return -1;
    }
}

void write_out_line_time(int  valu)
{
    char strGet[32] = {0};
    char strSet[32] = {0};
    int cnt = 0;
    int readValu;
    sprintf((char *)strSet, "%d", valu);
    set_out_line_time(valu);
    
    while (1)
    {
        SetValueToEtcFile(OSD_INI, "OSD", "OutLineTime", strSet);
        
        if (GetValueFromEtcFile(OSD_INI, "OSD", "OutLineTime", strGet, sizeof(strGet) - 1) == 0)
        {
            readValu = atoi(strGet);
        }
        
        if (readValu == valu)
        {
            printf("OutLineTime =%d\n", valu);
            break;
        }
        
        if (++cnt > 10)
        {
            printf("OutLineTime err\n");
            break;
        }
    }
}

int read_out_line_time(void)
{
    char buf[100];
    int temp;
    int ret;
    ret = GetValueFromEtcFile(OSD_INI, "OSD", "OutLineTime", buf, 100);
    
    if (ret >= 0)
    {
        printf("OutLineTime=%s\n", buf);
        sscanf(buf, "%d", &temp);
        printf("read_OutLineTime=%d\n", temp);
        return temp;
    }
    else
    {
        printf("read_OutLineTime err\n", temp);
        return -1;
    }
}

