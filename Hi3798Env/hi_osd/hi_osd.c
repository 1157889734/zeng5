
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

#include "hi_unf_ir.h"
//#include "hi_adp.h"

#include "hi_go.h"
//#include "Hi_go_decoder.h"
#include "sample_displayInit.h"
#include "hi_adp_mpi.h"
#include "sample_fb.h"
#include "hi_unf_gpio.h"

#include "ini_file.h"
#include "sample_osd.h"
#endif
#include "include.h"

#define BL_EN 41
#define BL_PWM 17
#define BL_PWM_DEFAULT 50
#define BL_PWM_MAX 100
#define BL_PWM_PERIOD 5000  //us
#define LCD_POWER 43


static char *prot_names[] = 
{
	"nec full 2headers",
	"nec simple 2headers changshu",
	"nec simple uPD6121G like",
	"nec simple LC7461MCL13 like",
	"nec full dezhou like",
	"nec full huizhou like",
	"nec simple 2headers gd",
	"credit",
	"sony d7c5",
	"sony d7c8",
	"tc9012",
	"rc6 32bit data",
	"rc6 20bit data",
	"rc6 16bit data",
	"rc5 14bit data",
	"extended rc5 14bit"
};

void usage(char *base)
{
	printf("usage: %s [-m 0|1] [-u 0|1] [-U num] [-r 0|1] [-R num] [-T readtimeout] [-E 0|1]"\
		" [-g protocolname] [-l] [-s protocolname -e 0|1] [-h]\n"\
		"\t-m : key fetch mode, 0 to read key parsed by driver, 1 to read symbols received.\n"\
		"\t-u : set keyup event, 0 to disable keyup event, 1 to enable keyup event.\n"\
		"\t-U : set keyup timeout, e.g. -U 300, driver will send a keyup event of last key parsed.\n"\
		"\t     after 300ms if no follow key recevied. Not supported yet.\n"\
		"\t-r : set repeat key event report or not. 0 to disable repeat event, 1 to enable.\n"\
		"\t-R : set repeat key event interval time. Default 200ms.\n"\
		"\t     e.g. -R 100 was set, then driver will report a key repeat event each 100ms.\n"\
		"\t-T : set read timeout value. e.g. -T 1000 was set, then read syscall will return after 1000 ms if no key recevied.\n"\
		"\t-E : enable or disable ir module. 0 to disable, 1 to enable. If 0 is set, ir module will do nothing.\n"
		"\t-g : get the enable/disable status of protocol which is specified by protocolname and then exit.\n"\
		"\t-l : list all default protocol name and exit. Only constains IR module, no customer's included!\n"\
		"\t-s : set the enable/disable status of protocol which is specified by protocolname, 0 to enable, 1 to disable.\n"\
		"\t     while set disable state, the respect protocol will not be considerd as a protocol parser,\n"\
		"\t     and if some corresponding infrared symbol received, they are not parsed and may be discarded.\n"\
		"\t-e : 0 to disable, 1 to enable, must be used with -s\n"\
		"\t-h : display this message and exit.\n"\
		"\tNOTES: Only [-s -e] option can be set more than once!\n",
		base);
}

extern char *optarg;
extern int optind, opterr, optopt;

/*lint -save -e729 */
int mode, mode_set;
int keyup_event, keyup_event_set;
unsigned int keyup_timeout, keyup_timeout_set;
int repeat_event, repeat_event_set;
unsigned int repeat_interval, repeat_interval_set;
unsigned int read_timeout = 200;
int ir_enable, ir_enable_set;
int prot_status;
char *prot_name;
int prot_name_set;
int prot_enable, prot_enable_set;

int g_thread_should_stop = 0;

static HI_S32 osd_get_valu(void);
static void osd_display(void);

/************** OSD Paragram Init **************************/
static   HIGO_TEXT_INFO_S pinfo={0};
static    HI_HANDLE hFont = HIGO_INVALID_HANDLE,hFontCh = HIGO_INVALID_HANDLE ;
static    HIGO_LAYER_INFO_S stLayerInfo = {0};
static    HI_HANDLE hLayer = HIGO_INVALID_HANDLE;
static    HI_HANDLE hLayerSurface,hMemSurface;//hDecoder,hDecSurface;
static    HI_RECT rc = {0};
//static    HI_CHAR MenuText[] = "Menu";

static unsigned char CurrentMenuIndex=MENU_DISPLAY;
//static unsigned char CurrentMenuIndexChild=0xff;
static    HIGO_LAYER_E eLayerID = HIGO_LAYER_HD_1;

static int exit_flag=0;
static int keyEnter[KEY_ENTER_CNT_MAX];
static int keyEnterNum=0;

static int valuEdit = 0;

typedef struct
{
    unsigned char valuNum;
    unsigned char valuEditBit;     // 设置数值位数
    int valu[20];
    unsigned char valuIndex;
    int valuMax[20];
} __attribute__((packed)) valu_st;

typedef struct
{
    char text[64];
    unsigned char childNum;
    unsigned char childIndex;
    char childText[CHILD_CNT_MAX][64];
    unsigned active;
    valu_st childvalu[CHILD_CNT_MAX];  
} __attribute__((packed)) menu_st;

/* valu index flag */
static const int valuSetIndex[MENU_CNT][CHILD_CNT_MAX]={
    //menu[0]
    {
        BRIGHTNESS_SET,CONTRAST_SET,0,0,0,0,0,0,0,0
    },
    //menu[1]
    {
        IPADDR_SET,0,0,0,0,0,0,0,0,0
    },
    //menu[2]
    {
        CYCLE_SET,CYCLETIME_SET,CYCLEINDEX_SET,0,0,0,0,0,0,0
    }    
};

static const char ONOFF[2][32]={
    "关",
    "开"
};

static const char COLOR[8][32]={
    "关",
    "红",
    "绿",
    "蓝",
    "黑",
    "灰",
    "白",
    "16灰阶",
};

/*menu init*/
static menu_st Menu[MENU_CNT]
#if 1
={
    //menu[0]
    {
        "参数设置",                            // text
        2,                                                                 // childNum
        0xff,                                                           // childIndex
        {"亮度","对比度"},           // childtext
        0,                                                                // active
        {{1,3,{50},0xff,{100}},{1,3,{60},0xff,{100}}, 
          {0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},
          {0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}}},                                         // valu
    },
    //menu[1]
    {
        "网络设置",                           // text
        1,                                                                 // childNum
        0xff,                                                          // childIndex
        {"IP 地址"},                                      // childtext
        0,                                                               // active
        {{4,3,{192,168,101,1},0xff,{255,255,255,255}},
          {0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},
          {0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}}},                                 // valu
    },

    //menu[2]
    {
        "显示测试",                           // text
        3,                                                                 // childNum
        0xff,                                                          // childIndex
        {"循环测试","循环间隔","指定画面"},                     // childtext
        0,                                                               // active
        {{1,1,{0},0xff,{1}},{1,3,{5},0xff,{255}},{1,3,{0},0xff,{7}},
          {0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},{0,0,{0},0,{0}},
          {0,0,{0},0,{0}},{0,0,{0},0,{0}}},                                 // valu
    },    
}; 

#endif

#if 0
/*higo image dec*/
static HI_S32 file_dec(HI_CHAR *pszFileName, HI_HANDLE *pSurface)
{
    HI_S32 ret         = HI_SUCCESS;
    HI_HANDLE hDecoder = 0;
    HIGO_DEC_ATTR_S stSrcDesc;
    HIGO_DEC_IMGINFO_S stImgInfo;
    HIGO_DEC_IMGATTR_S stAttr;

    stSrcDesc.SrcType = HIGO_DEC_SRCTYPE_FILE;
    stSrcDesc.SrcInfo.pFileName = pszFileName;

    ret = HI_GO_CreateDecoder(&stSrcDesc, &hDecoder);
    if(HI_SUCCESS != ret){
        return ret;
    }

    ret = HI_GO_DecImgInfo(hDecoder, 0, &stImgInfo);
    if(HI_SUCCESS != ret){
        HI_GO_DestroyDecoder(hDecoder);
        return ret;
    }

    if(stImgInfo.Width > 4095){
       stAttr.Width = 4095;
    }else{
       stAttr.Width = stImgInfo.Width;
    }

    if(stImgInfo.Height > 4095){
        stAttr.Height = 4095;
    }else{
        stAttr.Height = stImgInfo.Height;
    }

    stAttr.Format = HIGO_PF_8888;

    if(stImgInfo.Width > 4095 || stImgInfo.Height > 4095){
        ret = HI_GO_DecImgData(hDecoder, 0, &stAttr, pSurface);
    }else{
        ret  = HI_GO_DecImgData(hDecoder, 0, NULL, pSurface);
    }
    ret |= HI_GO_DestroyDecoder(hDecoder);

    return ret;

}

/*disp image*/

static void osd_image_display(HI_CHAR *pszFileName,HI_RECT* stRect,HI_HANDLE dstSurface,HI_RECT* dstRect)
{
    HIGO_BLTOPT_S stBltOpt = {0};
    HI_S32 s32Ret=0;
    HI_HANDLE srcSurface;
    s32Ret = file_dec(pszFileName, &srcSurface);

    stBltOpt.EnableScale = HI_TRUE;
    
    if(HI_SUCCESS == s32Ret){
        /** Blit it to graphic layer Surface */
        s32Ret = HI_GO_Blit(srcSurface, stRect, dstSurface, dstRect, &stBltOpt);
        if(HI_SUCCESS == s32Ret){
             HI_GO_RefreshLayer(hLayer, NULL);
        }
        HI_GO_FreeSurface(srcSurface);
        }else{
            printf("dec fail ret (%d)\n",s32Ret );
    }
}
#endif

/*
 * 返回值: 成功：0  失败 ：非零
 */
int exec_cmd_popen(char *cmd)
{
    FILE * fp = NULL;
    int rc = 0; // 用于接收命令返回值
    if(cmd == NULL){
        return (-1);
    }

    fp = popen(cmd, "r");
    if(fp ==NULL){
        printf("popen error.\n");
        return (-1);
    }
#if 0
    //打印命令输出信息,可以对出处做进一步处理,这里只
    char pInfo[PATH_SIZE];
    memset(pInfo, 0, sizeof(pInfo));
    while(!feof(fp)){
        if(fgets(pInfo, sizeof(pInfo), fp) != NULL){
            printf("%s",pInfo);
        }
    }
#endif     
    rc = pclose(fp);
    if( rc == -1){
        printf("close fp error.\n");
        return (-1);
    }else{
//        printf ("%s\t return value: %d\n",cmd, WEXITSTATUS(rc));
    }
    return (WEXITSTATUS(rc));
}

#if 0
static unsigned char pwm=0;
void Hi_SetPwm(unsigned char valu)
{
    if(valu <= 100)
        pwm = valu;
}
#endif

static unsigned char cycleChg=0;
void set_cycle_chg(unsigned char valu)
{
    cycleChg = valu;
}
unsigned char get_cycle_chg(void)
{
    return cycleChg;
}


static unsigned char cycleMode=0;
unsigned char get_cycle_mode(void)
{
    return cycleMode;
}

static unsigned char cycleTime=2;
unsigned char get_cycle_time(void)
{
    return cycleTime;
}

static unsigned char cycleIndex=0;
unsigned char get_cycle_index(void)
{
    return cycleIndex;
}

HI_S32 back_light_init(){
    HI_S32 ret;
    //HI_SYS_WriteRegister(0XF8A21044, 0x00000132);//BL_PWM  GPIO2-1   // BL_EN GPIO5_1 default:led gpio

    ret = HI_SYS_WriteRegister(0XF8A21044, 0x00000130);
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        return ret;
    }
        
    ret = HI_SYS_WriteRegister(0xf8a2301c,0x7FFFFFFF);
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        return ret;
    }

    //HI_SYS_WriteRegister(0XF8A21044, 0x00000130);//BL_PWM  GPIO2-1   // BL_EN GPIO5_1 default:led gpio
    //HI_SYS_WriteRegister(0xf8a2301c,0x7FFFFFFF);
    back_light_set_valu(BL_PWM_DEFAULT);

#if 0    
    HI_UNF_GPIO_Init();
    //HI_UNF_GPIO_SetDirBit(BL_PWM, HI_FALSE);
    HI_UNF_GPIO_SetDirBit(BL_EN, HI_FALSE);
    HI_UNF_GPIO_SetDirBit(LCD_POWER, HI_FALSE);

    //HI_UNF_GPIO_WriteBit(BL_PWM, HI_TRUE);
    HI_UNF_GPIO_WriteBit(BL_EN, HI_TRUE);
    HI_UNF_GPIO_WriteBit(LCD_POWER, HI_TRUE);   

    HI_UNF_GPIO_DeInit();
#else
    ret=writeGpio(5,1,HI_TRUE);
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        return ret;
    }
    ret=writeGpio(5,3,HI_TRUE);
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        return ret;
    }
#endif
    return HI_SUCCESS;
}

/*
** back light ctrl 
** onoff: 1=on 0=off
** valu : 0-100%
*/
static unsigned int  back_light_valu=BL_PWM_DEFAULT;
static unsigned char back_light_onoff=ON;
void back_light_set_valu(unsigned char valu){
    back_light_valu = valu;
    if(back_light_valu>BL_PWM_MAX)
        back_light_valu=BL_PWM_DEFAULT;
}

void back_light_power(int onoff){
    HI_S32 ret=HI_FAILURE;
    int cnt=0;
    int flag=0;

    do
    {
        flag=0;
        if(onoff){
            //HI_UNF_GPIO_WriteBit(BL_EN, HI_TRUE);
            ret=writeGpio(5,1,HI_TRUE);
            if (ret != HI_SUCCESS)
            {
                printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
                cnt++;
                flag=-1;
            }
            ret=writeGpio(5,3,HI_TRUE);
            if (ret != HI_SUCCESS)
            {
                printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
                cnt++;
                flag=-1;
            }
            
            back_light_onoff = ON;
        }
        else{
            //HI_UNF_GPIO_WriteBit(BL_EN, HI_FALSE);
            ret=writeGpio(5,1,HI_FALSE);
            if (ret != HI_SUCCESS)
            {
                printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
                cnt++;
                flag=-1;
            }
            ret=writeGpio(5,3,HI_FALSE);
            if (ret != HI_SUCCESS)
            {
                printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
                cnt++;
                flag=-1;
            }
            back_light_onoff = OFF;
        }

        if(cnt>10)
            exit(-1);
    }while(flag<0);       
}



void *back_light_thread(void *arg){
    //static struct timeval tv,tv_back;   // tv.sec tc.usec 5ms=5000us
    //static struct timezone tz;
    //static unsigned char bl_st=ON,bl_st_back;
    static unsigned int back_light_valu_bak=0;
    unsigned int valu=0;
    HI_S32 ret,cnt=0;

    while(1){

        //if(back_light_onoff==OFF){
        //    usleep(100000);
        //    continue;
        //}           

        #if 1
        if(back_light_valu !=back_light_valu_bak)
        {
            back_light_valu_bak = back_light_valu;

            if((back_light_valu_bak>0)&&(back_light_valu_bak<=100))
            {
                valu =0xFFFF*back_light_valu_bak/100;
                valu = 0xFFFF + (valu<<16);

                cnt=0;
                do
                {
                    ret = HI_SYS_WriteRegister(0xf8a2301c,valu);
                    if (ret != HI_SUCCESS)
                        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
                    usleep(100000);
                    if(++cnt>20)
                        exit(-1);
                }while(ret != HI_SUCCESS);  
            }                      
        }
        #endif
        usleep(100000);
    }
    
}

/*
** osd higo display init 
*/
HI_S32 osd_init()
{
    HI_S32 s32Ret = 0;

    printf("osd_init begin\n");
    
    s32Ret = HIADP_Disp_Init(HI_UNF_ENC_FMT_1080P_60);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }
    s32Ret = HI_GO_Init();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_GO_GetLayerDefaultParam(eLayerID, &stLayerInfo); 
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }
    
    stLayerInfo.PixelFormat = HIGO_PF_8888;
    stLayerInfo.LayerFlushType = HIGO_LAYER_FLUSH_NORMAL;
    stLayerInfo.AntiLevel = HIGO_LAYER_DEFLICKER_HIGH;
    stLayerInfo.ScreenWidth = MENU_WIDTH+MENU_X_OFFSET;
    stLayerInfo.ScreenHeight = MENU_HEIGHT+MENU_Y_OFFSET;
    stLayerInfo.DisplayWidth = MENU_WIDTH+MENU_X_OFFSET;
    stLayerInfo.DisplayHeight = MENU_HEIGHT+MENU_Y_OFFSET;
    stLayerInfo.CanvasWidth = MENU_WIDTH+MENU_X_OFFSET;
    stLayerInfo.CanvasHeight = MENU_HEIGHT+MENU_Y_OFFSET;
    s32Ret = HI_GO_CreateLayer(&stLayerInfo, &hLayer);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    s32Ret =  HI_GO_GetLayerSurface(hLayer, &hLayerSurface);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_GO_CreateSurface(stLayerInfo.CanvasWidth,stLayerInfo.CanvasHeight ,HIGO_PF_1555,&hMemSurface);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    /** set the attribute of memory surface */
    s32Ret = HI_GO_SetSurfaceAlpha(hMemSurface,0xff);  // 设置透明度
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    HI_GO_FillRect(hMemSurface, NULL, 0x00000000, HIGO_COMPOPT_NONE);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    pinfo.pSbcFontFile = OSD_FONT_NAME;
    pinfo.pMbcFontFile = NULL;
    pinfo.u32Size = FONT_EN;
    //s32Ret = HI_GO_CreateText(OSD_FONT_NAME, NULL,  &hFont);
    s32Ret = HI_GO_CreateTextEx(&pinfo,  &hFont);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    pinfo.pSbcFontFile = OSD_FONT_NAME;
    pinfo.pMbcFontFile = NULL;
    pinfo.u32Size = FONT_CH;
    //s32Ret = HI_GO_CreateText(OSD_FONT_NAME, NULL,  &hFont);
    s32Ret = HI_GO_CreateTextEx(&pinfo,  &hFontCh);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_GO_SetTextColor(hFont, 0xffffffff);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_GO_SetTextColor(hFontCh, 0xffffffff);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    s32Ret=HI_GO_FillRect(hMemSurface, NULL, 0xff000000, HIGO_COMPOPT_NONE);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }
    
    s32Ret=osd_get_valu();
    //osd_display();

    return s32Ret;
}

static void osd_exit(void)
{
    HI_GO_FillRect(hMemSurface, NULL, 0x00000000, HIGO_COMPOPT_NONE);
    HI_GO_Blit(hMemSurface,NULL,hLayerSurface,NULL,NULL);
    exit_flag =1;
    valuEdit = 0;
}

static void osd_display(void)
{
    HI_S32 s32Ret = 0;
    unsigned char i=0,j=0,k=0;
    char str[32];
    int childIndex=0;
    int childValuIndex=0;
    //char *imgPath;

    //if(exit_flag){       
    if(1){
        //HI_GO_FillRect(hMemSurface, NULL, 0xff303030, HIGO_COMPOPT_NONE);
        rc.x = MENU_X_OFFSET;
        rc.y = MENU_Y_OFFSET;
        rc.w = MENU_WIDTH;
        rc.h = MENU_STATUS_HEIGHT;
        HI_GO_FillRect(hMemSurface, &rc, 0xff000000, HIGO_COMPOPT_NONE);

        rc.x = MENU_X_OFFSET;
        rc.y = MENU_STATUS_HEIGHT+MENU_Y_OFFSET;
        rc.w = MENU1_WIDTH;
        rc.h = MENU1_HEIGHT;
        HI_GO_FillRect(hMemSurface, &rc, 0xff606060, HIGO_COMPOPT_NONE);

        rc.x = MENU1_WIDTH+MENU_X_OFFSET;
        rc.y = MENU_STATUS_HEIGHT+MENU_Y_OFFSET;
        rc.w = MENU_WIDTH - MENU1_WIDTH;
        rc.h = MENU1_HEIGHT;
        HI_GO_FillRect(hMemSurface, &rc, 0xffb0b0b0, HIGO_COMPOPT_NONE);

        //HI_GO_DrawLine(hLayerSurface, 1,50,999,50, 0xffff0000);
        HI_GO_DrawLine(hMemSurface, MENU_CHILD_VALU_LINE_X0,MENU_CHILD_VALU_LINE_Y1,MENU_CHILD_VALU_LINE_X1,MENU_CHILD_VALU_LINE_Y1, 0xffffffff);
        HI_GO_DrawLine(hMemSurface, MENU_CHILD_VALU_LINE_X0,MENU_CHILD_VALU_LINE_Y2,MENU_CHILD_VALU_LINE_X1,MENU_CHILD_VALU_LINE_Y2, 0xffffffff);
        HI_GO_DrawLine(hMemSurface, MENU_CHILD_VALU_LINE_X0,MENU_CHILD_VALU_LINE_Y3,MENU_CHILD_VALU_LINE_X1,MENU_CHILD_VALU_LINE_Y3, 0xffffffff);
        HI_GO_DrawLine(hMemSurface, MENU_CHILD_VALU_LINE_X0,MENU_CHILD_VALU_LINE_Y4,MENU_CHILD_VALU_LINE_X1,MENU_CHILD_VALU_LINE_Y4, 0xffffffff);

        rc.x = MENU_TEXT_X;
        rc.y = MENU_TEXT_Y;
        rc.w = MENU_WIDTH;
        rc.h = MENU_STATUS_HEIGHT;

        s32Ret = HI_GO_TextOutEx(hFont, hMemSurface, "Menu", &rc, HIGO_LAYOUT_LEFT);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
        }
    }

    //menu display
    for(i=0;i<MENU_CNT;i++){
        if(CurrentMenuIndex==i){
            rc.x = MENU_X_OFFSET;
            rc.y = MENU_CHILD_VALU_HEIGHT*(i+1)+MENU_Y_OFFSET;
            rc.w = MENU1_WIDTH;
            rc.h = MENU_CHILD_VALU_HEIGHT;
            s32Ret = HI_GO_SetTextColor(hFontCh, 0xff000000);
            if(Menu[CurrentMenuIndex].childIndex==0xff)
                HI_GO_FillRect(hMemSurface, &rc, 0xffffff00, HIGO_COMPOPT_NONE);
            else
                HI_GO_FillRect(hMemSurface, &rc, 0xffb0b0b0, HIGO_COMPOPT_NONE);
        }
        else
            s32Ret = HI_GO_SetTextColor(hFontCh, 0xffffffff);
           
        rc.x = MENU1_TEXT_X+MENU_X_OFFSET;
        rc.y = MENU_CHILD_VALU_HEIGHT*(i+1)+MENU1_TEXT_Y;
        rc.w =MENU1_WIDTH;
        rc.h = MENU_CHILD_VALU_HEIGHT;
        s32Ret = HI_GO_TextOutEx(hFontCh, hMemSurface, Menu[i].text, &rc, HIGO_LAYOUT_LEFT);
    }
    // child menu display
    #if 1
    if(CurrentMenuIndex<MENU_CNT){
        for(i=0;i<Menu[CurrentMenuIndex].childNum;i++){
            if(Menu[CurrentMenuIndex].childIndex==i){
                rc.x = MENU1_WIDTH +MENU_X_OFFSET;;
                rc.y = MENU_CHILD_VALU_HEIGHT*(i+1)+MENU_Y_OFFSET;;
                rc.w = MENU2_WIDTH;
                rc.h = MENU_CHILD_VALU_HEIGHT;
                s32Ret = HI_GO_SetTextColor(hFontCh, 0xff000000);
                if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex==0xff)
                    HI_GO_FillRect(hMemSurface, &rc, 0xffffff00, HIGO_COMPOPT_NONE);
                else
                    HI_GO_FillRect(hMemSurface, &rc, 0xffb0b0b0, HIGO_COMPOPT_NONE);
            }
            else
                s32Ret = HI_GO_SetTextColor(hFontCh, 0xffffffff);

            // text
            rc.x = MENU1_WIDTH+MENU2_TEXT_X;
            rc.y = MENU_CHILD_VALU_HEIGHT*(i+1)+MENU2_TEXT_Y;
            rc.w =MENU_WIDTH -MENU1_WIDTH;
            rc.h = MENU_CHILD_VALU_HEIGHT;
            s32Ret = HI_GO_TextOutEx(hFontCh, hMemSurface, Menu[CurrentMenuIndex].childText[i], &rc, HIGO_LAYOUT_LEFT);

            // child valu
            for(j=0;j<Menu[CurrentMenuIndex].childNum;j++){//child rect
                for(k=0;k<Menu[CurrentMenuIndex].childvalu[j].valuNum;k++){
                    if(k==Menu[CurrentMenuIndex].childvalu[j].valuIndex){
                        rc.x = MENU1_WIDTH+MENU2_WIDTH+MENU_X_OFFSET;;//+k*50;
                        rc.y = MENU_CHILD_VALU_HEIGHT*(j+1)+MENU_Y_OFFSET;
                        rc.w =MENU_WIDTH - (MENU1_WIDTH+MENU2_WIDTH);
                        rc.h = MENU_CHILD_VALU_HEIGHT+1;
                        if(valuEdit)
                            HI_GO_FillRect(hMemSurface, &rc, 0xffe0e0e0, HIGO_COMPOPT_NONE);
                        else
                            HI_GO_FillRect(hMemSurface, &rc, 0xffe0e0e0,  HIGO_COMPOPT_NONE);
                    }
                }
            
                for(k=0;k<Menu[CurrentMenuIndex].childvalu[j].valuNum;k++){//valu text
                    if(k==Menu[CurrentMenuIndex].childvalu[j].valuIndex){
                        rc.x = MENU1_WIDTH+MENU2_WIDTH+MENU3_TEXT_X+MENU_X_OFFSET+k*MENU3_VALU_SPACE;
                        rc.y = MENU_CHILD_VALU_HEIGHT*(j+1)+MENU_Y_OFFSET;
                        rc.w =MENU_CHILD_VALU_HEIGHT;
                        rc.h = MENU_CHILD_VALU_HEIGHT;

                        childIndex = Menu[CurrentMenuIndex].childIndex;
                        childValuIndex = Menu[CurrentMenuIndex].childvalu[childIndex].valuIndex;
                        if((CurrentMenuIndex==MENU_COLORTEST)&&(Menu[CurrentMenuIndex].childIndex==CYCLETIME_VALU_INDEX))
                            rc.w =MENU_CHILD_VALU_HEIGHT+28;    // 16 gray need big area
                        if((CurrentMenuIndex==MENU_COLORTEST)&&(Menu[CurrentMenuIndex].childIndex==CYCLEINDEX_VALU_INDEX)
                            &&(Menu[CurrentMenuIndex].childvalu[CYCLEINDEX_VALU_INDEX].valu[childValuIndex]== Menu[MENU_COLORTEST].childvalu[CYCLEINDEX_VALU_INDEX].valuMax[childValuIndex]))
                            rc.w =MENU_CHILD_VALU_HEIGHT+50;    // 16 gray need big area
                        
                        if(valuEdit)
                            HI_GO_FillRect(hMemSurface, &rc, 0xff0000ff, HIGO_COMPOPT_NONE);
                        else
                            HI_GO_FillRect(hMemSurface, &rc, 0xffffff00, HIGO_COMPOPT_NONE);

                        rc.x = MENU1_WIDTH+MENU2_WIDTH+MENU3_TEXT_X+MENU_X_OFFSET+k*MENU3_VALU_SPACE;
                        rc.y = MENU_CHILD_VALU_HEIGHT*(j+1)+MENU3_TEXT_Y;
                        rc.w =MENU3_VALU_WIDTH;//500;
                        rc.h = MENU_CHILD_VALU_HEIGHT;

                        if(valuEdit)
                            s32Ret = HI_GO_SetTextColor(hFontCh, 0xffffffff);
                        else
                            s32Ret = HI_GO_SetTextColor(hFontCh, 0xff000000);
                        
                    }
                    else{

                        if(Menu[CurrentMenuIndex].childvalu[j].valuIndex!=0xff)
                            s32Ret = HI_GO_SetTextColor(hFontCh, 0xff000000);
                        else
                            s32Ret = HI_GO_SetTextColor(hFontCh, 0xffffffff);
                        
                        rc.x = MENU1_WIDTH+MENU2_WIDTH+MENU3_TEXT_X+MENU_X_OFFSET+k*MENU3_VALU_SPACE;
                        rc.y = MENU_CHILD_VALU_HEIGHT*(j+1)+MENU3_TEXT_Y;
                        rc.w =MENU3_VALU_WIDTH;//500;
                        rc.h = MENU_CHILD_VALU_HEIGHT;
                    }          

                    switch(CurrentMenuIndex)
                    {
                        osd_get_valu();
                        case MENU_DISPLAY:
                                sprintf(str,"%d", Menu[CurrentMenuIndex].childvalu[j].valu[k]);
                            break;
                    
                        case MENU_NET:
                                if(j==IP_VALU_INDEX)
                                {
                                    if(k!=Menu[CurrentMenuIndex].childvalu[j].valuNum-1)
                                        sprintf(str,"%d.", Menu[CurrentMenuIndex].childvalu[j].valu[k]);
                                    else
                                        sprintf(str,"%d", Menu[CurrentMenuIndex].childvalu[j].valu[k]);   
                                }                          
                            break;

                        case MENU_COLORTEST:                                

                            if(j==CYCLEMODE_VALU_INDEX)
                            {
                                sprintf(str,"%s",ONOFF[Menu[CurrentMenuIndex].childvalu[j].valu[k]]);
                            }
                            else if(j==CYCLETIME_VALU_INDEX)
                            {
                                sprintf(str,"%d秒", Menu[CurrentMenuIndex].childvalu[j].valu[k]);
                            }
                            else if(j==CYCLEINDEX_VALU_INDEX)
                            {
                                sprintf(str,"%s", COLOR[Menu[CurrentMenuIndex].childvalu[j].valu[k]]);
                            }
                            else
                                sprintf(str,"%d", Menu[CurrentMenuIndex].childvalu[j].valu[k]);
                            break;

                        default:
                            break;
                    }                                          
                    
                    s32Ret = HI_GO_TextOutEx(hFontCh, hMemSurface,str, &rc, HIGO_LAYOUT_LEFT);   
                }
            }
        }
    }
    #endif

    s32Ret = HI_GO_Blit(hMemSurface,NULL,hLayerSurface,NULL,NULL);
    s32Ret = HI_GO_RefreshLayer(hLayer, NULL);

    return ;
}

int socket_getlocalip (unsigned long *local_ip)
{   
    struct ifreq ifr;
	int inet_sock = -1;
	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(inet_sock < 0)
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
    memcpy(local_ip, &(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr), sizeof(unsigned long));
 
    close(inet_sock); 
    return 0;
}

static HI_S32 osd_get_valu(void)
{
    char str_get[32]={0};
    //unsigned char childIndex,valuIndex;
    unsigned long addr;
    //unsigned char ip[4];
    HI_S32 s32Ret;

    //childIndex = Menu[CurrentMenuIndex].childIndex;
    //valuIndex = Menu[CurrentMenuIndex].childvalu[childIndex].valuIndex;
    
    //获取显示参数：亮度 对比度
    if(GetValueFromEtcFile(OSD_INI, "OSD", "Brightness", str_get, sizeof(str_get)-1)==0){
        printf("Brightness:%s\n",str_get);
        Menu[MENU_DISPLAY].childvalu[BRIGHTNESS_VALU_INDEX].valu[BRIGHTNESS_VALU_CHILD_INDEX]= atoi(str_get);

        if(Menu[MENU_DISPLAY].childvalu[BRIGHTNESS_VALU_INDEX].valu[BRIGHTNESS_VALU_CHILD_INDEX]>100)
            Menu[MENU_DISPLAY].childvalu[BRIGHTNESS_VALU_INDEX].valu[BRIGHTNESS_VALU_CHILD_INDEX] = 60;


        s32Ret = HI_UNF_DISP_SetBrightness(HI_UNF_DISPLAY1,Menu[MENU_DISPLAY].childvalu[BRIGHTNESS_VALU_INDEX].valu[BRIGHTNESS_VALU_CHILD_INDEX]);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
            return s32Ret;
        }
    }
    
    if(GetValueFromEtcFile(OSD_INI, "OSD", "Contrast", str_get, sizeof(str_get)-1)==0){
        printf("Contrast:%s\n",str_get);
        Menu[MENU_DISPLAY].childvalu[CONTRAST_VALU_INDEX].valu[CONTRAST_VALU_CHILD_INDEX]= atoi(str_get);

        if(Menu[MENU_DISPLAY].childvalu[CONTRAST_VALU_INDEX].valu[CONTRAST_VALU_CHILD_INDEX]>100)
            Menu[MENU_DISPLAY].childvalu[CONTRAST_VALU_INDEX].valu[CONTRAST_VALU_CHILD_INDEX] = 60;

        s32Ret=HI_UNF_DISP_SetContrast(HI_UNF_DISPLAY1,Menu[MENU_DISPLAY].childvalu[CONTRAST_VALU_INDEX].valu[CONTRAST_VALU_CHILD_INDEX]);
        if (s32Ret != HI_SUCCESS)
        {
            printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, s32Ret);
            return s32Ret;
        }
    }

    if(GetValueFromEtcFile(OSD_INI, "OSD", "Pwm", str_get, sizeof(str_get)-1)==0){        
        back_light_valu= atoi(str_get);
        printf("Contrast:%d\n",back_light_valu);
    }

    //获取网络设置参数：IP
    socket_getlocalip (&addr);
    Menu[MENU_NET].childvalu[IP_VALU_INDEX].valu[0]= addr&0xff;
    Menu[MENU_NET].childvalu[IP_VALU_INDEX].valu[1]= (addr>>8)&0xff;
    Menu[MENU_NET].childvalu[IP_VALU_INDEX].valu[2]= (addr>>16)&0xff;
    Menu[MENU_NET].childvalu[IP_VALU_INDEX].valu[3]= (addr>>24)&0xff;

    Menu[MENU_COLORTEST].childvalu[CYCLEMODE_VALU_INDEX].valu[CYCLEMODE_VALU_CHILD_INDEX]= get_cycle_mode();
    Menu[MENU_COLORTEST].childvalu[CYCLETIME_VALU_INDEX].valu[CYCLETIME_VALU_CHILD_INDEX]= get_cycle_time();
    Menu[MENU_COLORTEST].childvalu[CYCLEINDEX_VALU_INDEX].valu[CYCLEINDEX_VALU_CHILD_INDEX]= get_cycle_index();

    return HI_SUCCESS;
}

/*osd handle*/
static void osd_proc(HI_U64 key)
{
    //static int brightness=0;
    int childIndex=0;
    int childValuIndex=0;
    char str_set[32]={0};
    int i = 0,j=0;

    if(key==KEY_EXIT){
        osd_exit();
        return;
    }

    if(key==KEY_POWER)
        system("reboot");

    if(exit_flag){
        if(key==KEY_MENU){
            exit_flag = 0;
            CurrentMenuIndex = 0;
            for(i=0;i<MENU_CNT;i++){
                Menu[i].childIndex=0xff;
                for(j=0;j<CHILD_CNT_MAX;j++)
                    Menu[i].childvalu[j].valuIndex=0xff;                
            }
            
            // get valu display
            osd_get_valu();            
            osd_display();
            back_light_power(ON);
        }
        return;
    }

    back_light_power(ON);

    /* Menu Edit */
    // 1.MenuIndex chg
    if(Menu[CurrentMenuIndex].childIndex==0xff){
        keyEnterNum=0;
        switch(key){
            case KEY_UP:
                if(CurrentMenuIndex==0)
                    CurrentMenuIndex = MENU_CNT-1;
                else if(CurrentMenuIndex>0)
                    CurrentMenuIndex --;
                else{}
                break;

            case KEY_DOWN:
                if(CurrentMenuIndex<MENU_CNT-1)
                    CurrentMenuIndex ++;
                else
                    CurrentMenuIndex = 0;
                break;

            case KEY_RIGHT:                
                Menu[CurrentMenuIndex].childIndex=0;
                break;

            default:
                break;
        }
    }       
    // 2. childIndex  chg
    else if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex==0xff){
        keyEnterNum=0;
        switch(key){
            case KEY_UP:
                if(Menu[CurrentMenuIndex].childIndex==0)
                    Menu[CurrentMenuIndex].childIndex = Menu[CurrentMenuIndex].childNum-1;
                else if(Menu[CurrentMenuIndex].childIndex>0)
                    Menu[CurrentMenuIndex].childIndex --;
                else{}
                break;

            case KEY_DOWN:
                if(Menu[CurrentMenuIndex].childIndex<Menu[CurrentMenuIndex].childNum-1)
                    Menu[CurrentMenuIndex].childIndex ++;
                else
                    Menu[CurrentMenuIndex].childIndex = 0;
                break;

            case KEY_LEFT:                
                Menu[CurrentMenuIndex].childIndex=0xff;
                break;

            case KEY_RIGHT:                
                Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex=0;
                break;

            default:
                break;
        }
    }

    // 3. valu index && valu change
    else{
        // 3.1 valu change
        //if(valuEdit)
        {
            childIndex = Menu[CurrentMenuIndex].childIndex;
            childValuIndex = Menu[CurrentMenuIndex].childvalu[childIndex].valuIndex;
            
            switch(key){

                case KEY_0:
                    if(valuEdit==0)
                        break;
                    
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=0;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=0;
                    }
                    break;
                    
                case KEY_1:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=1;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=1;
                    }
                    break;
                    
                case KEY_2:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=2;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=2;
                    }
                    break;
                    
                case KEY_3:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=3;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=3;
                    }
                    break;
                    
                case KEY_4:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=4;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=4;
                    }
                    break;
                    
                case KEY_5:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=5;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=5;
                    }
                    break;
                    
                case KEY_6:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=6;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=6;
                    }
                    break;
                    
                case KEY_7:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=7;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=7;
                    }
                    break;
                    
                case KEY_8:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=8;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=8;
                    }
                    break;
                    
                case KEY_9:
                    if(valuEdit==0)
                        break;
                    if(keyEnterNum<Menu[CurrentMenuIndex].childvalu[childIndex].valuEditBit)
                        keyEnter[keyEnterNum++]=9;
                    else{
                        keyEnterNum = 0;
                        keyEnter[keyEnterNum++]=9;
                    }
                    break;
                    
                case KEY_UP:   
                    if(valuEdit==0)
                        break;
                    if(Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]<
                        Menu[CurrentMenuIndex].childvalu[childIndex].valuMax[childValuIndex])
                        Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]++;
                    else if(CurrentMenuIndex == MENU_COLORTEST)
                    {
                        if(Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]== Menu[CurrentMenuIndex].childvalu[childIndex].valuMax[childValuIndex])
                            Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]=0;
                    }
                    else
                    {}
                    
                    break;

                case KEY_DOWN:      
                    if(valuEdit==0)
                        break;
                    if(Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex])
                        Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]--;
                    else if(CurrentMenuIndex == MENU_COLORTEST)
                    {
                        if(Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]== 0)
                            Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]=Menu[CurrentMenuIndex].childvalu[childIndex].valuMax[childValuIndex];
                    }
                    else
                    {}
                    break;

                case KEY_LEFT:                
                    if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex>0)
                    {
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex--;
                        //valuEdit = 0;
                        keyEnterNum=0;
                    }
                    else if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex==0)
                    {
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex=0xff;
                        //valuEdit = 0;
                        keyEnterNum=0;
                    }
                    else{}
                    break;

                case KEY_RIGHT:                
                    if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex<
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuNum-1)
                    {
                        keyEnterNum=0;
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex++;
                    }
                    else
                    {
                        keyEnterNum=0;
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex=0;
                    }
                    break;

                case KEY_OK:
                    if(valuEdit==0)
                        valuEdit = 1;
                    else
                        valuEdit = 0;
                    keyEnterNum=0;                    
                    break;

                default:
                    break;
            }

            switch(keyEnterNum){
                case 1:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = keyEnter[keyEnterNum-1];
                    break;
                    
                case 2:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];
                    break;

                case 3:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                case 4:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-4]*1000+keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                case 5:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-5]*10000+keyEnter[keyEnterNum-4]*1000+keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                case 6:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-6]*100000+keyEnter[keyEnterNum-5]*10000+keyEnter[keyEnterNum-4]*1000+keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                case 7:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-7]*1000000+keyEnter[keyEnterNum-6]*100000+keyEnter[keyEnterNum-5]*10000+keyEnter[keyEnterNum-4]*1000+keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                case 8:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-8]*10000000+keyEnter[keyEnterNum-7]*1000000+keyEnter[keyEnterNum-6]*100000+keyEnter[keyEnterNum-5]*10000+keyEnter[keyEnterNum-4]*1000+keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                case 9:
                    Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] = 
                    keyEnter[keyEnterNum-9]*100000000+keyEnter[keyEnterNum-8]*10000000+ keyEnter[keyEnterNum-7]*1000000+keyEnter[keyEnterNum-6]*100000+keyEnter[keyEnterNum-5]*10000+keyEnter[keyEnterNum-4]*1000+keyEnter[keyEnterNum-3]*100+keyEnter[keyEnterNum-2]*10+keyEnter[keyEnterNum-1];   
                    break;

                default:
                    break;
            }                

            if(Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]>Menu[CurrentMenuIndex].childvalu[childIndex].valuMax[childValuIndex])
                Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex] =Menu[CurrentMenuIndex].childvalu[childIndex].valuMax[childValuIndex];

            switch(valuSetIndex[CurrentMenuIndex][childIndex]){
                
                case BRIGHTNESS_SET:
                    HI_UNF_DISP_SetBrightness(HI_UNF_DISPLAY1,Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    sprintf(str_set,"%d", Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    SetValueToEtcFile(OSD_INI, "OSD", "Brightness", str_set);
                    break;
                    
                case CONTRAST_SET:
                    HI_UNF_DISP_SetContrast(HI_UNF_DISPLAY1,Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    sprintf(str_set,"%d", Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    SetValueToEtcFile(OSD_INI, "OSD", "Contrast", str_set);
                    break;

                case IPADDR_SET:             
                    sprintf(str_set,"%d.%d.%d.%d", Menu[CurrentMenuIndex].childvalu[childIndex].valu[0],Menu[CurrentMenuIndex].childvalu[childIndex].valu[1],Menu[CurrentMenuIndex].childvalu[childIndex].valu[2],Menu[CurrentMenuIndex].childvalu[childIndex].valu[3]);
                    SetValueToEtcFile(NET_INI, "netconfig", "IPADDR", str_set);
                    //system("reboot");                    
                    break;       

                case CYCLE_SET:             
                    //sprintf(str_set,"%d", Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    //SetValueToEtcFile(NET_INI, "OSD", "CycleMode", str_set);   
                    if(cycleMode != Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex])
                    {
                        cycleMode = Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex];
                        cycleChg =1;
                    }
                    
                    break;

                case CYCLETIME_SET:             
                    //sprintf(str_set,"%d", Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    //SetValueToEtcFile(NET_INI, "OSD", "CycleTime", str_set);  
                    if(cycleTime != Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex])
                    {
                        cycleTime = Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex];
                        cycleChg =1;
                    }
                    break;   
                case CYCLEINDEX_SET:             
                    //sprintf(str_set,"%d", Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex]);
                    //SetValueToEtcFile(NET_INI, "OSD", "CycleIndex", str_set);        
                    if(cycleIndex != Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex])
                    {
                        cycleIndex = Menu[CurrentMenuIndex].childvalu[childIndex].valu[childValuIndex];
                        cycleChg =1;
                    }
                    break;      
                
                default:
                    break;
            }
            
        }
        // 3.2 valu index chg
        #if 0
        else{
            switch(key){
                case KEY_LEFT:                
                    if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex>0)
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex--;
                    else if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex==0)
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex=0xff;
                    else{}
                    break;

                case KEY_RIGHT:                
                    if(Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex<
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuNum-1)
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex++;
                    else
                        Menu[CurrentMenuIndex].childvalu[Menu[CurrentMenuIndex].childIndex].valuIndex=0;
                    break;

                case KEY_OK:
                    valuEdit = 1;
                    keyEnterNum=0;                    
                    break;

                default:
                    break;
            }
        }
        #endif
    }
    osd_display();
}

static int check_params(void)
{
	if (mode_set >  1 || ( mode != 0 && mode != 1)) {
		printf("fetch mode error!\n");
		return -1;
	}

	if (keyup_event_set > 1 || (keyup_event != 0 && keyup_event != 1)) {
		printf("keyup event error!\n");
		return -1;
	}
	if (repeat_event_set > 1 || repeat_interval_set > 1) {
		printf("Only -g [-s -e] option can be set more than once!\n");
		return -1;
	}
	return 0;
}
static int ir_get_prot_status(char *prot)
{
	int ret;
	HI_BOOL enabled;

	ret = HI_UNF_IR_Init();
	if (ret) {
		perror("Fail to open ir device!\n");
		return -1;
	}

	ret = HI_UNF_IR_GetProtocolEnabled(prot, &enabled);
	if (ret) {
		HI_UNF_IR_DeInit();
		printf("Fail to get protocol status!\n");
		return -1;
	}

	HI_UNF_IR_DeInit();
	return enabled;

}
static int ir_sample_set_prot_enable(char *prot, int enable)
{
	int ret;
	
	ret = HI_UNF_IR_Init();
	if (ret) {
		perror("Fail to open ir device!\n");
		return -1;
	}

	printf("Try set enable status %d to protocol %s.\n", enable, prot);
	if (enable) 
		ret = HI_UNF_IR_EnableProtocol(prot);
	else
		ret = HI_UNF_IR_DisableProtocol(prot);
	if (ret) {
		printf("Fail to set enable status of %s\n",prot);
		HI_UNF_IR_DeInit();
		return -1;
	}

	printf("Check it out if we do have set succussfully!\n");
	if (enable == ir_get_prot_status(prot))
		printf("Check pass!\n");
	else
		printf("Check fail!\n");

	HI_UNF_IR_DeInit();
	return 0;
}
static void process_options(int argc, char *argv[])
{
	int op, ret;
	opterr = 0;
	while ((op = getopt(argc, argv, "m:u:U:r:R:T:E:g:ls:e:h")) != -1) {
		switch (op) {
			case 'm' :
				mode = atoi(optarg);
				mode_set ++;
				printf("key fetch mode set to :%d\n", mode);
				break;
			case 'u' :
				keyup_event = atoi(optarg);
				keyup_event_set ++;
				printf("keyup event set to %d\n",keyup_event);
				break;
			case 'U' :
				keyup_timeout = strtoul(optarg, 0, 10);
				keyup_timeout_set ++;
				printf("keyup timeout set to %d\n",keyup_timeout);
				break;
			case 'r' :
				repeat_event = atoi(optarg);
				repeat_event_set ++;
				printf("repeat event set to %d\n",repeat_event);
				break;
			case 'R' :
				repeat_interval = strtoul(optarg, 0, 10);
				repeat_interval_set ++;
				printf("repeat interval set to %d\n",repeat_interval);
				break;
			case 'T' :
				read_timeout = strtoul(optarg, 0, 10);
				printf("read syscall timeout set to %d\n",read_timeout);
				break;
			case 'E':
				ir_enable = atoi(optarg);
				if (ir_enable != 0 && ir_enable != 1) {
					printf("-E parameter not correct, only 0 or 1 allowed!\n");
					break;
				}
				ir_enable_set = 1;
				printf("Set ir mode to %s state!\n", ir_enable ? "enable": "disable");
				break;
			case 'g' :
				printf("try to get the enable status of %s\n",optarg);
				
				prot_status = ir_get_prot_status(optarg);
				if (prot_status >= 0)
					printf("The status of %s is :%s\n",
							optarg,
							prot_status == 0 ? "disabled" : "enabled");
				else
					printf("Fail to get protocol status!\n");

				exit(0);
			case 'l' :
				{
					int i;
					printf("Available default protocols list:\n");
					for (i = 0; i < (HI_S32)(sizeof(prot_names) / sizeof(prot_names[0])); i++)
						printf("%s\n",prot_names[i]);
				}
				exit(0);
			case 's' :
				prot_name = optarg;
				prot_name_set = 1;

				printf("prot_name :%s.\n", prot_name);

				if (prot_enable_set && prot_name_set) {
					prot_enable_set = 0;
					prot_name_set = 0;
					ret = ir_sample_set_prot_enable(prot_name, prot_enable);
					if (ret) {
						perror("Fail to set prot_enable!\n");
						exit(-1);
					}
				}

				break;
			case 'e' :
				prot_enable = atoi(optarg);
				prot_enable_set = 1;

				printf("protocol enable status will set to :%d\n",prot_enable);

				if (prot_enable_set && prot_name_set) {
					prot_enable_set = 0;
					prot_name_set = 0;
					ret = ir_sample_set_prot_enable(prot_name, prot_enable);
					if (ret) {
						perror("Fail to set prot_enable!\n");
						exit(-1);
					}
				}
				break;
			case 'h' :
            default :
				usage(argv[0]);
				exit(0);
		}
	}
}
/*lint -save -e715 */


const HI_U64 key_value[18]={
    0xf20dff00,   // 0
    0xf30cff00,   // 1
    0xe718ff00,   // 2
    0xa15eff00,   // 3
    0xf708ff00,   // 4
    0xe31cff00,   // 5
    0xa55aff00,   // 6
    0xbd42ff00,   // 7
    0xad52ff00,   // 8
    0xb54aff00,   // 9
    0xba45ff00,   // 10  power 
    0xbb44ff00,   // 11  menu
    0xbc43ff00,   // 12  exit
    0xea15ff00,   // 13  ok
    0xbf40ff00,   // 14  up   
    0xe619ff00,   // 15  down
    0xf807ff00,   // 16  left
    0xf609ff00   // 17  right
};


void *ir_sample_thread(void *arg)
{
    int ret;
    int i=0;
    HI_U64 key, lower, upper;

    char name[64];
    HI_UNF_KEY_STATUS_E status;

    //return;

    while (!g_thread_should_stop) {
        if (mode) {
            ret = HI_UNF_IR_GetSymbol(&lower, &upper, read_timeout);
            if (!ret) {
                printf("Received symbols [%d, %d].\n",(HI_U32)lower, (HI_U32)upper);
            }
        } else {
            ret = HI_UNF_IR_GetValueWithProtocol(&status, &key, name, sizeof(name), read_timeout);
            if (!ret) {
                printf("Received key: 0x%.08llx, %s,\tprotocol: %s.\n",
                key, status == HI_UNF_KEY_STATUS_DOWN ? "DOWN" :
                (status == HI_UNF_KEY_STATUS_UP ? "UP" : "HOLD"),
                name);
                if(status == HI_UNF_KEY_STATUS_UP)
                    status =HI_UNF_KEY_STATUS_UP;
                else if(status == HI_UNF_KEY_STATUS_DOWN)
                    status = HI_UNF_KEY_STATUS_DOWN;
                else
                    status = HI_UNF_KEY_STATUS_HOLD;

                if(status == HI_UNF_KEY_STATUS_DOWN)//||(status == HI_UNF_KEY_STATUS_HOLD))
                {
                    for(i=0;i<18;i++){
                        if(key == key_value[i]){
                            osd_proc(i);
                            break;
                        }
                    }                               
                }
            }
        }
        usleep(1000);
    }
    return (void *)0;
}
int osd(void)
{
    int ret;
    pthread_t thread;
    pthread_t thread_light;
    /*lint -save -e550 */
    //char c;
    //fbTest();
    ret = back_light_init();
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        exit(-1);    
    }

#if 1
    ret = pthread_create(&thread_light, NULL, back_light_thread, NULL);	
    if (ret < 0) {		
        perror("Failt to create back_light thread!");       
        exit(-1);   
    }
 #endif   

    ret = osd_init();
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        exit(-1);    
    }
    
    ret = HI_UNF_IR_Init();
    if (ret != HI_SUCCESS)
            printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
    if (ret) {
        perror("Fail to open ir dev!");
        exit(-1);
    }

	/* key mode. */
	if (!mode) {
		HI_UNF_IR_SetCodeType(0);		
		if (keyup_event_set) {
			ret = HI_UNF_IR_EnableKeyUp(keyup_event);
			if (ret) {
				perror("Fail to set keyup event!");
				exit(-1);
			}
		}

		if (keyup_timeout_set) {
			printf("This version cannot support dynamicly change keyup timeout!\n");
		}

		if (repeat_event_set) {
			ret = HI_UNF_IR_EnableRepKey(repeat_event);
			if (ret) {
				perror("Fail to set repeat key event!");
				exit(-1);
			}
		}

		if (repeat_interval_set) {
			ret = HI_UNF_IR_SetRepKeyTimeoutAttr(repeat_interval);
			if (ret) {
				perror("Fail to set repeat interval!");
				exit(-1);
			}
		}
	}
	
	ret = HI_UNF_IR_SetFetchMode(mode);
	if (ret) {
		perror("Fail to set key fetch mode!");
		if (ret == HI_ERR_IR_UNSUPPORT) {
			printf("You may not use s2 mode, please set -E 1 parameter to enable ir module!\n");
		} else {
			exit(-1);
		}
	}

	if (ir_enable_set) {
		ret = HI_UNF_IR_Enable(ir_enable);
		if (ret) {
			perror("Fail to set enable state of ir module!");
			exit(-1);
		}
	}

	printf("Create ir sample thread, press q to exit!\n");

	ret = pthread_create(&thread, NULL, ir_sample_thread, NULL);
	if (ret < 0) {
		perror("Failt to create thread!");
		exit(-1);
	}
	while (1)
		sleep(3);

	g_thread_should_stop = 1;

	pthread_join(thread, NULL);

	printf("Ir sample thread exit! Bye!\n");

	HI_UNF_IR_DeInit();

	return 0;
}

static pthread_t    g_osdTest;
int osdTest(void)
{
	pthread_create(&g_osdTest, HI_NULL, (HI_VOID *)osd, HI_NULL);
	 //getchar();
	 return 0;
}

