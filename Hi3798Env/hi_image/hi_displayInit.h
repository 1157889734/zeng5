#ifndef __SAMPLE_DISPLAYINIT_H__
#define __SAMPLE_DISPLAYINIT_H__

/* add include here */


#ifdef __cplusplus
extern "C" {
#endif
#include "include.h"


#define Printf  HI_PRINT

HI_S32 Display_Init()
{
    HI_SYS_Init();
    HIADP_MCE_Exit();
    HIADP_Disp_Init(HI_UNF_ENC_FMT_720P_50);
    return 0;
}
HI_S32 Display_DeInit()
{
    HIADP_Disp_DeInit();
    HI_SYS_DeInit();

    return  0;
}
#ifdef __cplusplus
}
#endif
#endif
