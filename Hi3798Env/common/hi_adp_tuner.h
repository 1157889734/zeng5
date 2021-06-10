#ifndef     _COMMON_TUNER_H
#define     _COMMON_TUNER_H

#if 0
#include "hi_type.h"
#include "hi_unf_frontend.h"
#include "hi_adp_boardcfg.h"
#else
#include "include.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif/* __cplusplus */

/* **********************************public interface of Tuner********************************/
HI_S32 HIADP_Tuner_GetConfig(HI_U32 TunerId, HI_UNF_TUNER_ATTR_S *TunerAttr);

HI_S32 HIADP_Tuner_Init(HI_VOID);

HI_S32 HIADP_Tuner_Connect(HI_U32 TunerID, HI_U32 Freq, HI_U32 SymbolRate, HI_U32 ThirdParam);

HI_VOID HIADP_Tuner_DeInit(HI_VOID);

#ifdef __cplusplus
}
#endif

#endif

