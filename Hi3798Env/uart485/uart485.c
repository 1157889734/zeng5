#include <include.h>

pthread_mutex_t Uart485lock;

int open_serial(const char *dev_name)
{
	int fd;
	fd = open(dev_name, O_RDONLY|O_NONBLOCK);
	if (-1 == fd){ 
		perror("");
		exit(1);
	}

	return fd;
}

int set_speed(int fd)
{
	int ret = 0;
	struct  termios opt;

	tcgetattr(fd, &opt);
	tcflush(fd, TCIOFLUSH);
	cfsetispeed(&opt,B9600);
	cfsetospeed(&opt,B9600);

	ret = tcsetattr(fd,TCSANOW,&opt);
	if(ret <0) {
            perror("");
            return -1;
	}
	tcflush(fd,TCIOFLUSH);

	return ret;
}

int set_serial_rowmode(int fd)
{
	int ret = 0;
	struct termios options; 

	tcgetattr(fd, &options);

	tcflush(fd, TCIOFLUSH);
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSTOPB;
	options.c_oflag  &= ~OPOST;
	

	tcflush(fd,TCIFLUSH); 
	options.c_cc[VTIME] = 128;
	options.c_cc[VMIN] = 1;


	cfmakeraw(&options);

	//options.c_lflag |= ECHO;

	ret = tcsetattr(fd,TCSANOW,&options);
	if (ret < 0) { 
		perror(""); 
               return -1;
	} 

	return ret;
}

#define MAX_BUFF_SIZE (512)
int setUart2Reg()
{
    HI_SYS_WriteRegister(TRX_EN_REG, TRX_EN_REG_VAL);
    HI_SYS_WriteRegister(UART3_TXD_REG, UART3_TXD_REG_VAL);
    HI_SYS_WriteRegister(UART3_RXD_REG, UART3_RXD_REG_VAL);
    return 1;
}



int SelectReadSocket(int socket, int maxMsec)
{
	int nRet = -1;
	fd_set fd;
	FD_ZERO( &fd );//清空fd与所有文件句柄的联系
	FD_SET( socket, &fd );//建立文件句柄socket与fd的联系

	struct timeval timeout;
	timeout.tv_sec	= maxMsec / 1000;
	timeout.tv_usec	= (maxMsec % 1000) * 1000;

/*
* socket +1 件事的文件句柄数 视进程中打开的文件数而定,一般设为你要监视各文件
* 中的最大文件号加一
* fd  监视的可读文件句柄集合
* NULL 监视的可写文件句柄集合
* NULL 监视的异常文件句柄集合
* timeout 本次select()的超时结束时间
* 返回值：准备就绪的描述符数，若超时则返回0，若出错则返回-1
*/
	nRet = select( socket + 1, &fd, NULL, NULL, &timeout );
	if ( nRet > 0 )
	{
	//检查fd联系的文件句柄socket是否可读写，当>0表示可读写
		if ( FD_ISSET(socket, &fd) > 0 ) nRet = 1;
		else nRet = -1;
	}
	return nRet;
}

static unsigned char beg485Flag=0;
void set485BegFlag(unsigned char flag)
{
    beg485Flag = flag;
}

unsigned char  get485BegFlag(void)
{
    return beg485Flag;
}

static unsigned char rs485_com=1;
unsigned char get_rs485_com(void)
{
    return rs485_com;
}



static unsigned char u485ReadBuf[MAX_BUFF_SIZE];
static unsigned int u485In=0;
static unsigned int u485Out=0;

static int u485Fd;
int Uart485Init(void)
{
    unsigned char ret=-1;	  
    int cnt=0;
    setUart2Reg();
    //HI_UNF_GPIO_Init();
    //u485Fd = open_serial("/dev/ttyAMA2");
    u485Fd = open("/dev/ttyAMA2", O_RDONLY|O_NONBLOCK);
    if(u485Fd <0 )
    {
        printf("hiEnv==>open ttyAMA2 error!\n");
        exit(-1);
    }
    ret=set_speed(u485Fd);
    if(ret <0 )
    {
        printf("hiEnv==>set_speed error!\n");
        close(u485Fd);
        exit(-1);
    }
    ret=set_serial_rowmode(u485Fd);
    if(ret <0 )
    {
        printf("hiEnv==>set_serial_rowmode  error!\n");
        close(u485Fd);
        exit(-1);
    }
#if 0
    HI_UNF_GPIO_Init();
    HI_UNF_GPIO_SetDirBit(TRX_EN_PIN, HI_FALSE);
    HI_UNF_GPIO_WriteBit(TRX_EN_PIN, HI_FALSE);
    HI_UNF_GPIO_DeInit();
#else
    do
    {
        ret = writeGpio(2, 3, HI_FALSE);
        cnt++;
        if(cnt>5)
        {
            printf("hiEnv==>set 485 read io pin err exit\n");
            exit(-1);
        }
    }while(ret!=HI_SUCCESS);
#endif

    
}

void Uart485Write(unsigned char * temp)
{
    pthread_mutex_lock(&Uart485lock);
    u485ReadBuf[u485In++]=*temp;    
    if(u485In>=MAX_BUFF_SIZE)
        u485In=0;
    pthread_mutex_unlock(&Uart485lock);
}

int Uart485Read(unsigned char * temp)
{    
    if(u485Out ==u485In)
        return 0;
    else
    {
        pthread_mutex_lock(&Uart485lock);
        *temp = u485ReadBuf[u485Out++];
        pthread_mutex_unlock(&Uart485lock);
        if(u485Out>=MAX_BUFF_SIZE)
            u485Out=0;
        return 1;
    }        
}

int uart485_read(int argc, char *argv[])
{
    unsigned char temp=0;
    while(1)
    {
        if(read(u485Fd,&temp,1)>0)
        {
            Uart485Write(&temp);     
        }        
        else
            usleep(10);
    }
    close(u485Fd);
    return 0;
}




static unsigned char uRreadStatus = UART_HEAD;
static unsigned char u485ProcBuf[MAX_BUFF_SIZE];
static unsigned int u485Len=0;
int uart485_proc(int argc, char *argv[])
{
    int i;
    unsigned char temp=0;	  
    static unsigned int packet_len=0;
    static unsigned char chgFlag=0;
    int nowTime=0;
    static int rs485Time=0;

    for(i=0;i<MAX_BUFF_SIZE;i++)
        u485ProcBuf[i]=0;
    
    while(1)
    {
        if(Uart485Read(&temp)>0)
        {              
            #if 1
            if(get485BegFlag()>0)
            {
                switch(uRreadStatus)
                {
                    case UART_HEAD:
                        if(temp == 0x7e)
                        {
                            chgFlag = 0;
                            u485Len = 0;
                            u485ProcBuf[u485Len++]=temp;
                            uRreadStatus = UART_HEADDATA;
                        }                      
                        break;

                    case UART_HEADDATA:    // 1+16+2+2 = 21
                        if(temp==0x7F)
                        {
                            chgFlag = 1;
                        }
                        else
                        {
                            if(chgFlag)
                            {
                                if(temp == 0x80)
                                    u485ProcBuf[u485Len++]=0x7E;
                                if(temp == 0x81)
                                    u485ProcBuf[u485Len++]=0x7F;
                            }
                            else
                            {
                                u485ProcBuf[u485Len++]=temp;
                            }
                            chgFlag = 0;
                            
                            if(u485Len>=21)
                            {
                                uRreadStatus = UART_DATA;
                                packet_len = u485ProcBuf[19]+(u485ProcBuf[20]<<8);
                                if(packet_len>=MAX_BUFF_SIZE-21)
                                    uRreadStatus = UART_HEAD;
                                    
                                printf("packet_len=%d \n",packet_len);
                            }
                        }                        
                        break;
                        
                    case UART_DATA:
                        if(temp==0x7F)
                        {
                            chgFlag = 1;
                        }
                        else
                        {
                            if(chgFlag)
                            {
                                if(temp == 0x80)
                                    u485ProcBuf[u485Len++]=0x7E;
                                if(temp == 0x81)
                                    u485ProcBuf[u485Len++]=0x7F;
                            }
                            else
                            {
                                u485ProcBuf[u485Len++]=temp;
                            }
                            chgFlag = 0;
                            
                            if(u485Len >= (packet_len+21))
                            {
                                //printf("\n********u485Buf len: %d  packet: %d******\n",u485Len,++recvPacketCnt);
                                //for(i=0;i<u485Len;i++)
                                //{
                                //    printf("%0.2x ",u485ProcBuf[i]);
                                //}
                                //printf("\n");
                                send485(u485ProcBuf,u485Len);                        
                                uRreadStatus = UART_HEAD;
                                rs485Time = time((time_t*)NULL);
                                rs485_com =0; 
                            }        

                            if(u485Len >= (MAX_BUFF_SIZE-1))
                            {
                                u485Len = 0;
                                uRreadStatus = UART_HEAD;
                            }
                        }                        
                        break;
                }
            }
            else
            {
                rs485Time = time((time_t*)NULL);
                rs485_com =0; 
            }
            #endif
        }
        else
        {
            usleep(100);
            nowTime = time((time_t*)NULL);
            if(nowTime > rs485Time+5)
            {
                rs485_com = 1;
            }
            //if(++cnt%20000 ==0)
            //    printf("\n");
        }
    }
    return 0;
}


static pthread_t    g_485Recv;
static pthread_t    g_485Proc;
int uart485Test(void)
{
    int ret=-1;
    while(ret<0)
    {
        ret=Uart485Init();
        usleep(1000);
    }

    pthread_mutex_init(&Uart485lock,NULL);
    pthread_create(&g_485Recv, HI_NULL, (HI_VOID *)uart485_read, HI_NULL);
    pthread_create(&g_485Proc, HI_NULL, (HI_VOID *)uart485_proc, HI_NULL);
    return 0;
}



