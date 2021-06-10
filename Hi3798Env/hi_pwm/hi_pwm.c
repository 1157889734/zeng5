/******************************************************************************

  Copyright (C), 2011-2021, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : sample_pwm.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/10/28
  Description   :
  History       :
  1.Date        : 2013/10/28
    Author      : h00183514
    Modification: Created file

******************************************************************************/
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

#include "hi_unf_common.h"
#include "hi_unf_pwm.h"

#include "hi_adp.h"
#include "hi_adp_hdmi.h"
#include "hi_adp_boardcfg.h"
#include "hi_adp_mpi.h"
#include "hi_adp_tuner.h"
#else
#include "include.h"
#endif


#define PWM_CLK_FREQ        (54 * 1000 * 1000)      /*pwm work clock freq*/

/********************* pwm register start **************************/
#define PWM_REG_BASE        (0xF8B01000)
#define PWM0_REG_CFG        (PWM_REG_BASE + 0x0)
#define PWM0_REG_CTRL       (PWM_REG_BASE + 0x4)
#define PWM0_REG_STATE      (PWM_REG_BASE + 0x8)
#define PWM1_REG_CFG        (PWM_REG_BASE + 0x10)
#define PWM1_REG_CTRL       (PWM_REG_BASE + 0x14)
#define PWM1_REG_STATE      (PWM_REG_BASE + 0x18)
#define PWM2_REG_CFG        (PWM_REG_BASE + 0x20)
#define PWM2_REG_CTRL       (PWM_REG_BASE + 0x24)
#define PWM2_REG_STATE      (PWM_REG_BASE + 0x28)
/********************** pwm register end ***************************/

static pthread_t                   g_stSendThread;
static HI_BOOL                     g_bThreadRun = HI_FALSE;

HI_VOID *SendThread(HI_VOID *pArgs)
{
    /*cycle send 10110100*/
    while(g_bThreadRun)
    {
        sleep(5);
        /*send 1*/
        //HI_UNF_PWM_SendSignal(HI_UNF_PWM_1, 560, 1690);
        /*send 0*/
        //HI_UNF_PWM_SendSignal(HI_UNF_PWM_1, 560, 560);   
    }

    return HI_NULL;
}

HI_S32 pwmTest(void)
{
#if 0
    HI_S32                  Ret;
    HI_UNF_PWM_ATTR_S       stAttr;
    HI_CHAR                 InputCmd[32];
    
    Ret = HI_UNF_PWM_Init();
    if (HI_SUCCESS != Ret)
    {
        printf("HI_UNF_PWM_Init ERR, Ret=%#x\n", Ret);
        return Ret;
    }

    Ret = HI_UNF_PWM_Open(HI_UNF_PWM_1);
    if (HI_SUCCESS != Ret)
    {
        printf("HI_UNF_PWM_Open ERR, Ret=%#x\n", Ret);
        HI_UNF_PWM_DeInit();
        return Ret;
    }

    Ret = HI_UNF_PWM_GetAttr(HI_UNF_PWM_1, &stAttr);
    stAttr.u32Freq = 200;
    stAttr.u32DutyRatio = 50;
    Ret |= HI_UNF_PWM_SetAttr(HI_UNF_PWM_1, &stAttr);
    if (HI_SUCCESS != Ret)
    {
        printf("HI_UNF_PWM_Open ERR, Ret=%#x\n", Ret);
        HI_UNF_PWM_Close(HI_UNF_PWM_1);
        HI_UNF_PWM_DeInit();
        return Ret;
    }

    g_bThreadRun = HI_TRUE;
    pthread_create(&g_stSendThread, HI_NULL, (HI_VOID *)SendThread, HI_NULL);

    while (1)
        sleep(20);

    g_bThreadRun = HI_FALSE;
    pthread_join(g_stSendThread, HI_NULL);     
    
    HI_UNF_PWM_Close(HI_UNF_PWM_1);

    HI_UNF_PWM_DeInit();    
#else
    return 0;
 

 #endif
}
