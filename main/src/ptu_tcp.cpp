/*
ptu TCP
功能描述
-初始化tcp socket
-连接服务器
-被动关闭连接
-主动关闭连接
-接收数据
-发送数据
*/
#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include<errno.h>
#include <unistd.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "ptu_tcp.h"
#include "param.h"
#include "osd_proc.h"
#include "main.h"
#include "drmd_ui.h"
#include "ini_file.h"

static int flag = -1;
static int socketFd = -1;
static int connect_fd = -1;
static struct sockaddr_in serverAddr;
static socklen_t serverAddrLen;
static struct sockaddr_in myAddr;
static socklen_t myAddrLen;
static char *serverIp = "192.168.7.160";
static int serverPort = 9990;
static int recvTime = 0;
static pthread_rwlock_t lock;//锁
int PtuTcpDataInit()
{
    pthread_rwlock_init(&lock, NULL);
    pthread_rwlock_wrlock(&lock);
    flag = -1;
    socketFd = -1;
    pthread_rwlock_unlock(&lock);
    printf("PtuTcpDataInit\n");
    return 1;
}
int getTcpIsConnect()
{
    int connect = -1;
    pthread_rwlock_rdlock(&lock);
    
    if (flag > 0 && socketFd > 0)
    {
        connect = flag;
    }
    
    pthread_rwlock_unlock(&lock);
    return connect;
}
int PtuTcpClear()
{
    printf("PtuTcpUnConnect\n");
    pthread_rwlock_wrlock(&lock);
    flag = -1;
    socketFd = -1;
    pthread_rwlock_unlock(&lock);
}
int PtuTcpUnConnect()
{
    printf("PtuTcpUnConnect\n");
    pthread_rwlock_wrlock(&lock);
    
    if (flag > 0 && socketFd > 0)
    {
        int ret;
        ret = shutdown(socketFd, 2);
        printf("ret = %d\n", ret);
        printf("del socket = %d\n", socketFd);
        flag = -1;
        socketFd = -1;
    }
    
    pthread_rwlock_unlock(&lock);
}
int PtuTcpconnect()
{
    int ret = -1;
    int tempFlag = -1;
    pthread_rwlock_rdlock(&lock);
    tempFlag = flag;
    pthread_rwlock_unlock(&lock);
    
    if (tempFlag > 0)
    {
        return -1;
    }
    
    //创建socket
    int tempSocket = -1;
    tempSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (tempSocket < 0)
    {
        perror("socket");
        PtuTcpClear();
        return -1;
    }
    
    //发起连接
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    serverAddrLen = sizeof(serverAddr);
    int connect_fd = -1;
    connect_fd = connect(tempSocket, (struct sockaddr *)&serverAddr, serverAddrLen);
    
    if (connect_fd < 0)
    {
        perror("PtuTcpConnect connect_fd<0");
        shutdown(tempSocket, 2);
        PtuTcpClear();
        printf("del socket = %d\n", tempSocket);
        return -2;
    }
    
    //初始化成功
    pthread_rwlock_wrlock(&lock);
    socketFd = tempSocket;
    flag = 1;
    pthread_rwlock_unlock(&lock);
    printf("PtuTcpInit is ok\n");
    printf("new socket = %d\n", socketFd);
    return 1;
}
/*
接收数据
返回-1=没有连接
返回-2=接收异常
返回值值大于0=实际接收的字节数
*/
int PtuTcpReadData(unsigned char *buf, int len)
{
    int len1;
    int tempFlag = -1;
    int tempSocket = -1;
    pthread_rwlock_rdlock(&lock);
    tempFlag = flag;
    tempSocket = socketFd;
    pthread_rwlock_unlock(&lock);
    
    if (tempFlag < 0)
    {
        return -1;
    }
    
    if ((len1 = recv(tempSocket, buf, len, 0)) <= 0)
    {
        perror("PtuTcpReadData recv error");
        shutdown(tempSocket, 2);
        PtuTcpClear();
        return -2;
    }
    
    return len1;
}
/*
发送数据
返回-1=没有连接
返回-2=接收异常
返回值值大于0=实际接收的字节数
*/
int PtuTcpWriteData(unsigned char *buf, int len)
{
    int len1;
    int tempFlag = -1;
    int tempSocket = -1;
    pthread_rwlock_rdlock(&lock);
    tempFlag = flag;
    tempSocket = socketFd;
    pthread_rwlock_unlock(&lock);
    
    if (tempFlag < 0)
    {
        return -1;
    }
    
    if ((len1 = send(tempSocket, buf, len, 0)) < 0)
    {
        perror("PtuTcpWriteData send");
        shutdown(tempSocket, 2);
        PtuTcpClear();
        return -2;
    }
    
    return len1;
}
struct ptuTcpRecvPacket
{
    unsigned char head;
    unsigned short cmd;
    int packetLen;
    int dataLen;
    unsigned char buff[1024];
};
//字节倒置
void convert(char *buf, int len)
{
    char temp;
    
    for (int i = 0; i < len / 2; i++)
    {
        temp = buf[i];
        buf[i] = buf[len - 1 - i];
        buf[len - 1 - i] = temp;
    }
}
#include <sys/vfs.h>
int GetStorageInfo(char *MountPoint,   //SD卡随便一个分区
                   unsigned long long *Capacity,  //  想要获取的空间大小
                   int type) //获取什么类型的空间
{
    struct statfs statFS; //系统stat的结构体
    unsigned long long usedBytes = 0;
    unsigned long long freeBytes = 0;
    unsigned long long totalBytes = 0;
    unsigned long long endSpace = 0;
    
    if (statfs(MountPoint, &statFS) == -1)    //获取分区的状态
    {
        printf("statfs failed for path->[%s]\n", MountPoint);
        return (-1);
    }
    
    totalBytes = (unsigned long long)statFS.f_blocks * (unsigned long long)statFS.f_frsize; //详细的分区总容量， 以字节为单位
    freeBytes = (unsigned long long)statFS.f_bfree * (unsigned long long)statFS.f_frsize; //详细的剩余空间容量，以字节为单位
    usedBytes = (unsigned long long)(totalBytes - freeBytes); //详细的使用空间容量，以字节为单位
    
    switch ( type )
    {
        case 1:
            endSpace = totalBytes; //以B为单位的总容量
            break;
            
        case 2:
            endSpace = usedBytes; //以B为单位的使用空间
            break;
            
        case 3:
            endSpace = freeBytes; //以B为单位的剩余空间
            break;
            
        default:
            return ( -1 );
    }
    
    *Capacity = endSpace; //这个不用说了吧
    return 0;
}

int ptuTcpPacketProc(const struct ptuTcpRecvPacket Ppacket)
{
    unsigned long long usedBytes = 0;
    unsigned long long freeBytes = 0;
    unsigned long long totalBytes = 0;
    char *path;
    unsigned long long Capacity = 0;
    unsigned long long sdBuff[2];
    int type = 1;
    char buf[20];
    char sysbuf[20];
    char valu;
    unsigned char uvalu;
    unsigned char ubuf[20];
    int ivalu;
    unsigned short cmd;
    cmd = Ppacket.cmd;
    
    switch (cmd)
    {
        case GET_STATE:
            //printf("GET_STATE\n");
            buf[0] = read_run_state();
            ptuTcpSendData(cmd, buf, 1);
            break;
            
        case GET_SYS_SIZE:
            printf("GET_SYS_SIZE\n");
            path = "/";
            Capacity = 0;
            type = 1;
            GetStorageInfo(path, &Capacity, type);
            totalBytes = Capacity;
            Capacity = 0;
            type = 2;
            GetStorageInfo(path, &Capacity, type);
            usedBytes = Capacity;
            Capacity = 0;
            type = 3;
            GetStorageInfo(path, &Capacity, type);
            freeBytes = Capacity;
            sdBuff[0] = totalBytes; //SD卡总内存
            printf("totalBytes=%lldB\n", totalBytes);
            printf("totalBytes=%dG\n", (int)(totalBytes / 1024 / 1024 / 1024));
            convert((char *)&sdBuff[0], 8);
            sdBuff[1] = freeBytes;//SD卡剩余内存
            printf("freeBytes=%lldB\n", freeBytes);
            printf("freeBytes=%dG\n", (int)( freeBytes / 1024 / 1024 / 1024));
            convert((char *)&sdBuff[1], 8);
            ptuTcpSendData(cmd, (char *)sdBuff, 16);
            break;
            
        case GET_SD_SIZE:
            printf("GET_SD_SIZE\n");
            Capacity = 0;
            type = 1;
            path = "/root/sdcard0/";
            GetStorageInfo(path, &Capacity, type);
            totalBytes = Capacity;
            Capacity = 0;
            type = 2;
            GetStorageInfo(path, &Capacity, type);
            usedBytes = Capacity;
            Capacity = 0;
            type = 3;
            GetStorageInfo(path, &Capacity, type);
            freeBytes = Capacity;
            sdBuff[0] = totalBytes; //SD卡总内存
            printf("totalBytes=%lldB\n", totalBytes);
            printf("totalBytes=%dG\n", (int)(totalBytes / 1024 / 1024 / 1024));
            convert((char *)&sdBuff[0], 8);
            sdBuff[1] = freeBytes;//SD卡剩余内存
            printf("freeBytes=%lldB\n", freeBytes);
            printf("freeBytes=%dG\n", (int)( freeBytes / 1024 / 1024 / 1024));
            convert((char *)&sdBuff[1], 8);
            ptuTcpSendData(cmd, (char *)sdBuff, 16);
            break;
            
        case FORMAT_SD:
            printf("FORMAT_SD\n");
            valu = 0;
            
            // todo format sdcard0
            
            if (access("/dev/mmcblk1p1", F_OK) == 0)
            {
                if (exec_cmd_popen("umount /dev/mmcblk1p1") == 0)
                {
                    if (exec_cmd_popen("mkfs.vfat /dev/mmcblk1p1") == 0)
                    {
                        if (exec_cmd_popen("mount /dev/mmcblk1p1 /root/sdcard0") == 0)
                            ptuTcpSendData(cmd, &valu, 1);
                    }
                }
            }
            else if (access("/dev/mmcblk1", F_OK) == 0)
            {
                if (exec_cmd_popen("umount /dev/mmcblk1") == 0)
                {
                    if (exec_cmd_popen("mkfs.vfat /dev/mmcblk1") == 0)
                    {
                        if (exec_cmd_popen("mount /dev/mmcblk1 /root/sdcard0") == 0)
                            ptuTcpSendData(cmd, &valu, 1);
                    }
                }
            }
            else
            {
                valu = -1;
                ptuTcpSendData(cmd, &valu, 1);
            }
            
            break;
            
        case GET_VERSION:
            //printf("GET_VERSION\n");
            valu = Ppacket.buff[0];;
            
            if (1 == valu)
            {
                buf[0] = 1;
                read_version(&buf[1]);
                ptuTcpSendData(cmd, buf, 7);
            }
            
            break;
            
        case DEV_CTRL:
            printf("DEV_CTRL\n");
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            
            if (Ppacket.buff[0] == 0x01) // 0x01重启指令
                system("reboot");
                
            break;
            
        case GET_CTRL_TYPE:
            valu = read_work_mode();
            printf("GET_CTRL_TYPE\n");
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case SET_CTRL_TYPE:
            printf("SET_CTRL_TYPE\n");
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case GET_BACK_LIGHT:
            valu = read_light();
            printf("GET_BACK_LIGHT\n");
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case SET_BACK_LIGHT:
            printf("SET_BACK_LIGHT=%d\n", uvalu);
            uvalu = Ppacket.buff[0];
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            setLight(uvalu);
            break;
            
        case GET_BACK_PWM:
            valu = read_pwm();
            printf("GET_BACK_PWM\n");
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case SET_BACK_PWM:
            printf("SET_BACK_PWM=%d\n", uvalu);
            uvalu = Ppacket.buff[0];
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            setPwm(uvalu);
            break;
            
        case GET_CONTRAST:
            valu = read_contrast();
            printf("GET_CONTRAST\n");
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case PTU_SET_CONTRAST:
            printf("SET_CONTRAST=%d\n", uvalu);
            uvalu = Ppacket.buff[0];
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            setContrast(uvalu);
            break;
            
        case SET_IP:
            unsigned char ipAddr[4];
            printf("SET_IP\n");
            
            for (int i = 0; i < 4; i++)
            {
                ubuf[i] = Ppacket.buff[i];
            }
            
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            setIp(ubuf[0], ubuf[1], ubuf[2], ubuf[3]);
            break;
            
        case GET_MAC:
            printf("GET_MAC\n");
            read_mac(&buf[0]);
            ptuTcpSendData(cmd, buf, 6);
            break;
            
        case SET_OUTLINE_TIME:
            printf("SET_OUTLINE_TIME\n");
            ivalu = (Ppacket.buff[0] << 8) + Ppacket.buff[1];
            write_out_line_time(ivalu);
            printf("usvalu=%d\n", ivalu);
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case GET_OUTLINE_TIME:
            printf("GET_OUTLINE_TIME\n");
            ivalu = read_out_line_time();
            printf("usvalu=%d\n", ivalu);
            buf[0] = ivalu >> 8;
            buf[1] = ivalu;
            ptuTcpSendData(cmd, buf, 2);
            break;
            
        case SET_OFFLINE_TIME:
            printf("SET_OFFLINE_TIME\n");
            ivalu = (Ppacket.buff[0] << 8) + Ppacket.buff[1];
            write_off_line_time(ivalu);
            printf("ivalu=%d\n", ivalu);
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case GET_OFFLINE_TIME:
            printf("GET_OFFLINE_TIME\n");
            ivalu = read_off_line_time();
            printf("usvalu=%d\n", ivalu);
            buf[0] = ivalu >> 8;
            buf[1] = ivalu;
            ptuTcpSendData(cmd, buf, 2);
            break;
            
        case UPDATE_START:
            setCmd(SET_UPDATE_ING);
            sendDrmdPacket();
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            break;
            
        case UPDATE_END:
            setCmd(SET_UPDATE_OK);
            sendDrmdPacket();
            valu = 0;
            
            if (exec_cmd_popen("sync") == 0)
                ptuTcpSendData(cmd, &valu, 1);
                
            break;
            
        case SET_CHECK_LCD:
            printf("SET_CHECK_LCD\n");
            valu = 0;
            ptuTcpSendData(cmd, &valu, 1);
            
            if ((get_osd_CycleMode() != Ppacket.buff[0])
                || (get_osd_CycleTime() != Ppacket.buff[1])
                || (get_osd_CycleIndex() != Ppacket.buff[2]))
            {
                if ((get_osd_CycleMode() == Ppacket.buff[0]) && (get_osd_CycleMode() == 0))
                {
                    set_osd_CycleMode(Ppacket.buff[0]);
                    set_osd_CycleTime(Ppacket.buff[1]);
                    set_osd_CycleIndex(Ppacket.buff[2]);
                }
                else
                {
                    set_osd_CycleMode(Ppacket.buff[0]);
                    set_osd_CycleTime(Ppacket.buff[1]);
                    set_osd_CycleIndex(Ppacket.buff[2]);
                    sendDrmdCyclePacket();
                }
            }
            
            break;
            
        case GET_CHECK_LCD:
            printf("GET_CHECK_LCD\n");
            buf[0] = get_osd_CycleMode();
            buf[1] = get_osd_CycleTime();
            buf[2] = get_osd_CycleIndex();
            ptuTcpSendData(cmd, buf, 3);
            break;
            
        case GET_SYSTEM_LCD:
            printf("GET_SYSTEM_LCD\n");
            
            if (GetValueFromEtcFile("/opt/rootfsVersion.conf", "Version", "version", buf, sizeof(buf) - 1) >= 0)
            {
                sscanf(buf, "%d.%d", &sysbuf[0], &sysbuf[1]);
                printf("read_version=%d.%d\n", sysbuf[0], sysbuf[1]);
            }
            
            ptuTcpSendData(cmd, sysbuf, 2);
            break;
            
        case HEARTBEAT:
            break;
            
        default:
            printf("error:recvFlag:cmd=%d\n", (unsigned short)cmd);
            break;
    }
}
//校验数据包
int ptuTcpRecvPacket(unsigned char *buf, int size)
{
    unsigned char temp = 0;
    unsigned char const head = 0x7e;
    unsigned char const end = 0xfe;
    unsigned char recvEnd = 0xfe;
    unsigned char checkflag = 1;
    unsigned char check = 0;
    unsigned char recvCheck = 0;
    int packetLen = 0;
    int dataLen = 0;
    unsigned short cmd = 0;
    int const len =  size;
    struct ptuTcpRecvPacket Ppacket;
    
    for (int i = 0; i + 13 <= len;)
    {
        temp = buf[i];
        
        if (head == temp)
        {
            cmd = 0;
            cmd += (short)buf[i + 1];
            cmd += (((short)buf[i + 2]) << 8);
            convert((char *)&cmd, 2);
            packetLen = 0;
            packetLen += ((int)buf[i + 3]);
            packetLen += (((int)buf[i + 4]) << 8);
            packetLen += (((int)buf[i + 5]) << 16);
            packetLen += (((int)buf[i + 6]) << 24);
            convert((char *)&packetLen, 4);
            dataLen = 0;
            dataLen += ((int)buf[i + 7]);
            dataLen += (((int)buf[i + 8]) << 8);
            dataLen += (((int)buf[i + 9]) << 16);
            dataLen += (((int)buf[i + 10]) << 24);
            convert((char *)&dataLen, 4);
            
            if (packetLen >= 13 && (packetLen + i) <= len && dataLen + 13 == packetLen)
            {
                check = 0;
                
                for (int j = 1; j < packetLen - 2; j++)
                {
                    checkflag = (j % 2 == 1) ? 1 : 3;
                    check += checkflag * (unsigned char)buf[i + j];
                }
                
                check /= 11;
                recvCheck = (unsigned char)buf[i + packetLen - 2];
                recvEnd = (unsigned char)buf[i + packetLen - 1];
                
                if (check == recvCheck && end == recvEnd)
                {
                    Ppacket.cmd = cmd;
                    Ppacket.packetLen = packetLen;
                    Ppacket.dataLen = dataLen;
                    
                    for (int j = 0; j < dataLen; j++)
                    {
                        Ppacket.buff[j] = (unsigned char)buf[i + 11 + j];
                    }
                    
                    i += packetLen;
                    ptuTcpPacketProc(Ppacket);
                    continue;
                }
                else
                {
                    printf("error:recvEnd=%d\n", recvEnd);
                    printf("error:recvCheck=%d\n", recvCheck);
                    printf("error:check=%d\n", check);
                }
            }
        }
        
        i++;
    }
}
void ptuTcpSendData(short cmd, char *buff, unsigned int len)
{
    unsigned char sendBuf[1024];
    unsigned char sendLen;
    sendLen = 0;
    int i = 0;
    sendBuf[sendLen++] = 0x7e;//包头
    //命令
    sendBuf[sendLen++] = ((char *)&cmd)[1];
    sendBuf[sendLen++] = ((char *)&cmd)[0];
    //包长
    unsigned int packetlen = 13 + len;
    sendBuf[sendLen++] = ((char *)&packetlen)[3];
    sendBuf[sendLen++] = ((char *)&packetlen)[2];
    sendBuf[sendLen++] = ((char *)&packetlen)[1];
    sendBuf[sendLen++] = ((char *)&packetlen)[0];
    //数据长度
    sendBuf[sendLen++] = ((char *)&len)[3];
    sendBuf[sendLen++] = ((char *)&len)[2];
    sendBuf[sendLen++] = ((char *)&len)[1];
    sendBuf[sendLen++] = ((char *)&len)[0];
    
    //此处填充数据内容
    for (int i = 0; i < len; i++)
    {
        sendBuf[sendLen++] = buff[i];
    }
    
    //填充校验码
    unsigned char check = 0;
    unsigned char checkflag = 1;
    
    for (int i = 1; i < packetlen - 2; i++)
    {
        checkflag = (i % 2 == 1) ? 1 : 3;
        check += checkflag * (unsigned char)sendBuf[i];
    }
    
    check /= 11;
    sendBuf[sendLen++] = check;
    sendBuf[sendLen++] = 0xfe;//包尾
    PtuTcpWriteData(sendBuf, sendLen);
}
/*
发送心跳数据包
*/
int ptuSendHeartbeat()
{
    //发送心跳包
    ptuTcpSendData(HEARTBEAT, NULL, 0);
    return 1;
}
