#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAC_BCAST_ADDR      (unsigned char *) "/xff/xff/xff/xff/xff/xff"


#define ETH_INTERFACE       "eth0"

struct arpMsg
{
    struct ethhdr ethhdr;       /* Ethernet header */
    u_short htype;              /* hardware type (must be ARPHRD_ETHER) */
    u_short ptype;              /* protocol type (must be ETH_P_IP) */
    u_char  hlen;               /* hardware address length (must be 6) */
    u_char  plen;               /* protocol address length (must be 4) */
    u_short operation;          /* ARP opcode */
    u_char  sHaddr[6];          /* sender's hardware address */
    u_char  sInaddr[4];         /* sender's IP address */
    u_char  tHaddr[6];          /* target's hardware address */
    u_char  tInaddr[4];         /* target's IP address */
    u_char  pad[18];            /* pad for min. Ethernet payload (60 bytes) */
};

struct server_config_t
{
    u_int32_t server;       /* Our IP, in network order */
    u_int32_t start;        /* Start address of leases, network order */
    u_int32_t end;          /* End of leases, network order */
    struct option_set *options; /* List of DHCP options loaded from the config file */
    char *interface;        /* The name of the interface to use */
    int ifindex;            /* Index number of the interface to use */
    unsigned char arp[6];       /* Our arp address */
    unsigned long lease;        /* lease time in seconds (host order) */
    unsigned long max_leases;   /* maximum number of leases (including reserved address) */
    char remaining;         /* should the lease file be interpreted as lease time remaining, or
                     * as the time the lease expires */
    unsigned long auto_time;    /* how long should udhcpd wait before writing a config file.
                     * if this is zero, it will only write one on SIGUSR1 */
    unsigned long decline_time;     /* how long an address is reserved if a client returns a
                         * decline message */
    unsigned long conflict_time;    /* how long an arp conflict offender is leased for */
    unsigned long offer_time;   /* how long an offered address is reserved */
    unsigned long min_lease;    /* minimum lease a client can request*/
    char *lease_file;
    char *pidfile;
    char *notify_file;      /* What to run whenever leases are written */
    u_int32_t siaddr;       /* next server bootp option */
    char *sname;            /* bootp server name */
    char *boot_file;        /* bootp boot file option */
};

struct server_config_t server_config;

static u_int32_t ip_addr;

static int Ipconflict = 0;
void setIpconflict(int flag)
{
    Ipconflict = flag;
}

int getIpconflict(void)
{
    return Ipconflict;
}


/*参数分别表示 网卡设备类型 接口检索索引 主机IP地址 主机arp地址*/
int read_interface(char *interface, int *ifindex, u_int32_t *addr, unsigned char *arp)
{
    int fd;
    /*ifreq结构定义在/usr/include/net/if.h，用来配置ip地址，激活接口，配置MTU等接口信息的。
    其中包含了一个接口的名字和具体内容——（是个共用体，有可能是IP地址，广播地址，子网掩码，MAC号，MTU或其他内容）。
    ifreq包含在ifconf结构中。而ifconf结构通常是用来保存所有接口的信息的。
    */
    struct ifreq ifr;
    struct sockaddr_in *our_ip;
    memset(&ifr, 0, sizeof(struct ifreq));
    
    /*建立一个socket函数，SOCK_RAW是为了获取第三个参数的IP包数据，
     IPPROTO_RAW提供应用程序自行指定IP头部的功能。
    */
    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0)
    {
        ifr.ifr_addr.sa_family = AF_INET;
        /*将网卡类型赋值给ifr_name*/
        strcpy(ifr.ifr_name, interface);
        
        if (addr)
        {
            /*SIOCGIFADDR用于检索接口地址*/
            if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
            {
                /*获取本机IP地址，addr是一个指向该地址的指针*/
                our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
                *addr = our_ip->sin_addr.s_addr;
                //printf("%s (our ip) = %s\n", ifr.ifr_name, inet_ntoa(our_ip->sin_addr));
            }
            else
            {
                printf("SIOCGIFADDR failed, is the interface up and configured?: %s\n",
                       strerror(errno));
                return -1;
            }
        }
        
        /*SIOCGIFINDEX用于检索接口索引*/
        if (ioctl(fd, SIOCGIFINDEX, &ifr) == 0)
        {
            //printf("adapter index %d\n", ifr.ifr_ifindex);
            /*指针ifindex 获取索引*/
            *ifindex = ifr.ifr_ifindex;
        }
        else
        {
            printf("SIOCGIFINDEX failed!: %s\n", strerror(errno));
            return -1;
        }
        
        /*SIOCGIFHWADDR用于检索硬件地址*/
        if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
        {
            /*所获取的硬件地址复制到结构server_config的数组arp[6]参数中*/
            memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);
            //printf("adapter hardware address %02x:%02x:%02x:%02x:%02x:%02x\n",
            //   arp[0], arp[1], arp[2], arp[3], arp[4], arp[5]);
        }
        else
        {
            printf("SIOCGIFHWADDR failed!: %s\n", strerror(errno));
            return -1;
        }
    }
    else
    {
        printf("socket failed!: %s\n", strerror(errno));
        return -1;
    }
    
    close(fd);
    return 0;
}

static int socketArpSend;                      /* socket */
static int socketArpRecv;                      /* socket */
static pthread_t arpRecvThreadFd;

/*参数说明 目标IP地址，本机IP地址，本机mac地址，网卡类型*/
void arpSend(char *addrc)
{
    int rv = 1;                 /* return value */
    struct sockaddr addr;       /* for interface name */
    struct arpMsg arp;
    u_int32_t yiaddr;
    u_int32_t ip;
    unsigned char *mac;
    
    //printf("------------------------------------------------------------------------------\n");
    //printf("read interface\n");
    /*读以太网接口函数，获取一些配置信息*/
    if (read_interface(ETH_INTERFACE, &server_config.ifindex, &server_config.server, server_config.arp) < 0)
    {
        exit(0);
    }
    
    //printf("------------------------------------------------------------------------------\n");
    /*IP检测函数*/
    yiaddr = inet_addr(addrc);
    ip = server_config.server;
    mac = server_config.arp;
    /* 对arp设置，这里按照arp包的封装格式赋值即可 */
    memset(&arp, 0, sizeof(arp));
    memcpy(arp.ethhdr.h_dest, mac, 6);   /* MAC DA */
#if 1
    arp.ethhdr.h_dest[0] = 0xff;
    arp.ethhdr.h_dest[1] = 0xff;
    arp.ethhdr.h_dest[2] = 0xff;
    arp.ethhdr.h_dest[3] = 0xff;
    arp.ethhdr.h_dest[4] = 0xff;
    arp.ethhdr.h_dest[5] = 0xff;
#endif
    memcpy(arp.ethhdr.h_source, mac, 6);        /* MAC SA */
    arp.ethhdr.h_proto = htons(ETH_P_ARP);      /* protocol type (Ethernet) */
    arp.htype = htons(ARPHRD_ETHER);        /* hardware type */
    arp.ptype = htons(ETH_P_IP);            /* protocol type (ARP message) */
    arp.hlen = 6;                   /* hardware address length */
    arp.plen = 4;                   /* protocol address length */
    arp.operation = htons(ARPOP_REQUEST);       /* ARP op code */
    *((u_int *) arp.sInaddr) = ip;          /* source IP address */
    memcpy(arp.sHaddr, mac, 6);         /* source hardware address */
    *((u_int *) arp.tInaddr) = yiaddr;      /* target IP address */
    memset(&addr, 0, sizeof(addr));
    strcpy(addr.sa_data, ETH_INTERFACE);
    
    /*发送arp请求*/
    if (sendto(socketArpSend, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
        rv = 0;
}

void *arpRecv(void *arg)
{
    fd_set fdset;
    int rv = 1;                 /* return value */
    int timeout;
    static time_t time_pre;
    struct arpMsg arp;
    struct timeval tm;
    tm.tv_usec = 0;
    tm.tv_sec = 2;
    
    /* 利用select函数进行多路等待*/
    while (1)
    {
        FD_ZERO(&fdset);
        FD_SET(socketArpRecv, &fdset);
        
        if (select(socketArpRecv + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0)
        {
            printf("Error on ARPING request: %s\n", strerror(errno));
            
            if (errno != EINTR)
                rv = 0;
        }
        else if (FD_ISSET(socketArpRecv, &fdset))
        {
            if (recv(socketArpRecv, &arp, sizeof(arp), 0) < 0 )
                rv = 0;
                
            // printf("******************************************************************************\n");
            //printf("arp receive receive sInaddr: %d.%d.%d.%d  ARPOP_REPLY:%x \n",arp.sInaddr[0],arp.sInaddr[1],
            //   arp.sInaddr[2],arp.sInaddr[3],arp.operation);
            
            /*如果条件 htons(ARPOP_REPLY) bcmp(arp.tHaddr, mac, 6) == 0 *((u_int *) arp.sInaddr) == yiaddr 三者都为真，
            则ARP应答有效,说明这个地址是存在的*/
            //if (arp.operation == htons(ARPOP_REPLY) &&  bcmp(arp.tHaddr, mac, 6) == 0 && *((u_int *) arp.sInaddr) == yiaddr)
            //if (arp.operation == htons(ARPOP_REPLY)  && *((u_int *) arp.sInaddr) == ip_addr)
            if (*((u_int *) arp.sInaddr) == ip_addr)
            {
                printf("IP:%x.%x.%x.%x  cofilct\n", arp.sInaddr[0], arp.sInaddr[1], arp.sInaddr[2], arp.sInaddr[3]);
                time(&time_pre);
                setIpconflict(1);
                rv = 0;
            }
            else
            {
                timeout = time(NULL) - time_pre;
                
                if (timeout > 6)
                {
                    setIpconflict(0);
                    time(&time_pre);
                }
            }
            
            // printf("******************************************************************************\n");
        }
        else
        {
            timeout = time(NULL) - time_pre;
            
            if (timeout > 6)
            {
                setIpconflict(0);
                time(&time_pre);
            }
        }
        
        usleep(100);
    }
}


int initARP(char *addrc)
{
    int optval = 1;
    
    /*socket发送一个arp包*/
    if ((socketArpSend = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1)
    {
        printf("Could not open raw socket\n");
        return -1;
    }
    
    /*设置套接口类型为广播，把这个arp包是广播到这个局域网*/
    if (setsockopt(socketArpSend, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1)
    {
        printf("Could not setsocketopt on raw socket\n");
        close(socketArpSend);
        return -1;
    }
    
    if ((socketArpRecv = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1)
    {
        printf("Could not open raw socket\n");
        return -1;
    }
    
    /*设置套接口类型为广播，把这个arp包是广播到这个局域网*/
    if (setsockopt(socketArpRecv, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1)
    {
        printf("Could not setsocketopt on raw socket\n");
        close(socketArpRecv);
        return -1;
    }
    
    ip_addr = inet_addr(addrc);
    pthread_create(&arpRecvThreadFd, NULL, &arpRecv, NULL);
}



#if 0
int check_ip(u_int32_t addr)
{
    struct in_addr temp;
    
    if (arpping(addr, server_config.server, server_config.arp, ETH_INTERFACE) == 0)
    {
        temp.s_addr = addr;
        printf("%s belongs to someone, reserving it for %ld seconds\n", inet_ntoa(temp), server_config.conflict_time);
        return 1;
    }
    else
        return 0;
}

int ipCheckMain(char *addr)
{
    /*读以太网接口函数，获取一些配置信息*/
    if (read_interface(ETH_INTERFACE, &server_config.ifindex, &server_config.server, server_config.arp) < 0)
    {
        exit(0);
    }
    
    /*IP检测函数*/
    if (check_ip(inet_addr(addr)) == 0)
    {
        printf("IP:%s can use\n", addr);
    }
    else
    {
        printf("IP:%s conflict\n", addr);
        return 1;
    }
    
    printf("-------------------------------------------------------\n");
    return 0;
}
#endif

