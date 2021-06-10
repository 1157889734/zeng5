#include "hi_displayInit.h"
#include "include.h"

//#define FONT_NAME "/root/fonts/msyh.ttf"
#define WIDTH 2560


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


static int imageProc(void)
#if 0   // 文字显示
{
    HI_S32 s32Ret = 0;
    HIGO_TEXT_INFO_S pinfo={0};
    HI_HANDLE hFont = HIGO_INVALID_HANDLE;
    HIGO_LAYER_INFO_S stLayerInfo = {0};
    HI_HANDLE hLayer = HIGO_INVALID_HANDLE;
    HI_HANDLE hLayerSurface;
    HI_RECT rc = {0};
    //HI_CHAR szText[] = {0x77,0xe2,0x91,0xcf,0x5b,0x4f,0x53,0x66,0x34,0x79, 0x3a, 0x0}; // vector character display
    HI_CHAR szText[] = "scrolltext 滚动文字测试";
    HIGO_LAYER_E eLayerID = HIGO_LAYER_HD_1;
    
    s32Ret = Display_Init();
    if (HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }
    
    s32Ret = HI_GO_Init();
    if (HI_SUCCESS != s32Ret)
    {
        goto ERR1;
    }

    s32Ret = HI_GO_GetLayerDefaultParam(eLayerID, &stLayerInfo); 
    if (HI_SUCCESS != s32Ret)
    {
        goto ERR2;
    }
    
    stLayerInfo.PixelFormat = HIGO_PF_8888;
    stLayerInfo.LayerFlushType = HIGO_LAYER_FLUSH_NORMAL;
    //stLayerInfo.AntiLevel = HIGO_LAYER_DEFLICKER_HIGH;
    //stLayerInfo.ScreenWidth = 1280;
    //stLayerInfo.ScreenHeight = 30;
    //stLayerInfo.DisplayWidth = 1280;
    //stLayerInfo.DisplayHeight = 30;
    s32Ret = HI_GO_CreateLayer(&stLayerInfo, &hLayer);
    if (HI_SUCCESS != s32Ret)
    {
        goto ERR2;
    }

    s32Ret =  HI_GO_GetLayerSurface(hLayer, &hLayerSurface);
    if  (HI_SUCCESS != s32Ret)
    {
        goto ERR3;       
    }

    HI_GO_FillRect(hLayerSurface, NULL, 0x00000000, HIGO_COMPOPT_NONE);
    if  (HI_SUCCESS != s32Ret)
    {
        goto ERR3;       
    }

    pinfo.pSbcFontFile = FONT_NAME;
    pinfo.pMbcFontFile = NULL;
    pinfo.u32Size = 40;
    //s32Ret = HI_GO_CreateText(FONT_NAME, NULL,  &hFont);
    s32Ret = HI_GO_CreateTextEx(&pinfo,  &hFont);
    if (HI_SUCCESS != s32Ret) 
    {
        goto ERR3;       
    }

    s32Ret = HI_GO_SetTextColor(hFont, 0xffff0000);
    if (HI_SUCCESS != s32Ret) 
    {
        goto ERR4;       
    }
    
    rc.x = 0;
    rc.y = 550;
    rc.w = 1280;
    rc.h = 100;

    s32Ret = HI_GO_TextOutEx(hFont, hLayerSurface, szText, &rc, HIGO_LAYOUT_LEFT);
    if (HI_SUCCESS != s32Ret)
    {
        goto ERR4; 
    }

    s32Ret = HI_GO_RefreshLayer(hLayer, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        goto ERR4; 
    }

    while(1){
        //usleep(5000);

        rc.x +=5;
        if(rc.x>1280)
            rc.x=0;

        HI_GO_FillRect(hLayerSurface, NULL, 0x00000000, HIGO_COMPOPT_NONE);
        if  (HI_SUCCESS != s32Ret)
        {
            goto ERR3;
        }
        s32Ret = HI_GO_TextOutEx(hFont, hLayerSurface, szText, &rc, HIGO_LAYOUT_LEFT);
        if (HI_SUCCESS != s32Ret)
        {
            goto ERR4;
        }

        s32Ret = HI_GO_RefreshLayer(hLayer, NULL);
        if (HI_SUCCESS != s32Ret)
        {
            goto ERR4;
        }
    }

ERR4:
    HI_GO_DestroyText(hFont);
ERR3:
    HI_GO_DestroyLayer(hLayer);
ERR2:
    HI_GO_Deinit();
ERR1:
    //Display_DeInit();
    return s32Ret;
}
#else  // 图片显示
{
    HI_S32 s32Ret = 0;
    //HI_HANDLE hFont = HIGO_INVALID_HANDLE;
    HIGO_LAYER_INFO_S stLayerInfo = {0};
    HI_HANDLE hLayer = HIGO_INVALID_HANDLE;
    HI_HANDLE hLayerSurface,hDecoder,hDecSurface;
    HI_RECT stRect = {0, 150, 1280, 372};
    //HIGO_SCROLLTEXT_ATTR_S stScrollAttr = {0};
    //HIGO_SCROLLTEXT_DATA_S stScrollData = {0};
    //HI_U32  u32Cnt = 0;
    //HI_U32 u32Offset = 0;
	HIGO_DEC_ATTR_S stDecAttr;
    HIGO_BLTOPT_S stBltOpt = {0};

    static struct timeval tv,tv_bak;
    static struct timezone tz;
    static char playEnd=0;
    char play;
    char *str;

    s32Ret = Display_Init();
    if(HI_SUCCESS != s32Ret){
        return s32Ret;
    }

    s32Ret = HI_GO_Init();
    if(HI_SUCCESS != s32Ret){
        goto ERR1;
    }

#if 1
    s32Ret = HI_GO_GetLayerDefaultParam(HIGO_LAYER_HD_0, &stLayerInfo);
    if(HI_SUCCESS != s32Ret){
        goto ERR2;
    }

    stLayerInfo.PixelFormat    = HIGO_PF_8888;
    stLayerInfo.LayerFlushType = HIGO_LAYER_FLUSH_FLIP;      
    s32Ret = HI_GO_CreateLayer(&stLayerInfo, &hLayer);
    if(HI_SUCCESS != s32Ret){
        goto ERR2;
    }

    s32Ret = HI_GO_GetLayerSurface(hLayer, &hLayerSurface);
    if(HI_SUCCESS != s32Ret){
        goto ERR3;        
    }

    stBltOpt.EnableScale = HI_TRUE;

#endif

    printf("--------HIGO IMAGE START---------\n");

    while(1)
    {
        play = getImagePlay();

        if(play){
            gettimeofday(&tv_bak,&tz);
            str = getImageName();

            stDecAttr.SrcType = HIGO_DEC_SRCTYPE_FILE;
            stDecAttr.SrcInfo.pFileName = str;

#if 0
            s32Ret = HI_GO_CreateDecoder(&stDecAttr, &hDecoder);
            if(HI_SUCCESS != s32Ret){
                goto ERR3;
            }
            s32Ret = HI_GO_DecImgToSurface(hDecoder, 0, hDecSurface);
            if(HI_SUCCESS != s32Ret){
                goto ERR4;
            }
            s32Ret = HI_GO_Blit(hDecSurface,NULL,hLayerSurface,&stRect,&stBltOpt);
            if(HI_SUCCESS != s32Ret){
                goto ERR4;
            }
#else
            s32Ret = file_dec(str, &hDecSurface);
            if(HI_SUCCESS == s32Ret){
                /** Blit it to graphic layer Surface */
                s32Ret = HI_GO_Blit(hDecSurface, NULL, hLayerSurface, &stRect, &stBltOpt);
                if(HI_SUCCESS == s32Ret){
                    HI_GO_RefreshLayer(hLayer, NULL);
                }
                HI_GO_FreeSurface(hDecSurface);
            }else{
               printf("dec fail ret (%d)\n",s32Ret );
            }
#endif

            HI_GO_RefreshLayer(hLayer, NULL);

            setImagePlay(0);
            printf("--------ImagePlay--------- \n");
            playEnd =1;
        }

        if(playEnd){
            gettimeofday(&tv,&tz);
            printf("tv.s= %d tv_bak.s=%d\n",tv.tv_sec,tv_bak.tv_sec);
            while((tv.tv_sec - tv_bak.tv_sec < 8)&&(tv.tv_sec >= tv_bak.tv_sec))
            {
                gettimeofday(&tv,&tz);
                usleep(1000);
            }

            HI_GO_FillRect(hLayerSurface, NULL, 0x00000000, HIGO_COMPOPT_NONE);
            HI_GO_RefreshLayer(hLayer, NULL);
            ImagePlayEnd();
            printf("--------ImagePlayEnd--------- \n");
            playEnd =0;
        }
        usleep(1000);
    }
    
ERR4:
       printf("------------------ERR4--------------\n");
	HI_GO_DestroyDecoder(hDecoder);
ERR3:
    printf("------------------ERR3--------------\n");
	HI_GO_DestroyLayer(hLayer);
ERR2:
    printf("------------------ERR2--------------\n");
	HI_GO_Deinit();
ERR1:
    printf("------------------ERR1--------------\n");
	Display_DeInit();
	
    return s32Ret;
}
#endif

static pthread_t    g_lscrolltextTest;
int imageTest(void)
{
	pthread_create(&g_lscrolltextTest, HI_NULL, (HI_VOID *)imageProc, HI_NULL);
	 //getchar();
}
