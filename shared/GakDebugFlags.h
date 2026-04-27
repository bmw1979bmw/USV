#pragma once

#include "DebugFlags.h"


// GAK
#define DEBUGFLAG_NODETECT       DEBUGFLAG_00      
#define      FLAG_NODETECT       (g_dwDebugFlags & DEBUGFLAG_NODETECT)
#define      FLAG_DETECT         (!FLAG_NODETECT) 
#define DEBUGFLAG_NONOISES       DEBUGFLAG_01      
#define      FLAG_NONOISES       (g_dwDebugFlags & DEBUGFLAG_NONOISES)
#define      FLAG_NOISES         (!FLAG_NONOISES) 
#define DEBUGFLAG_NOSIGNALS      DEBUGFLAG_02      
#define      FLAG_NOSIGNALS      (g_dwDebugFlags & DEBUGFLAG_NOSIGNALS)
#define      FLAG_SIGNALS        (!FLAG_NOSIGNALS)
#define DEBUGFLAG_NORAYSTRUCT    DEBUGFLAG_03      
#define      FLAG_NORAYSTRUCT    (g_dwDebugFlags & DEBUGFLAG_NORAYSTRUCT)
#define      FLAG_RAYSTRUCT      (!FLAG_NORAYSTRUCT)
#define DEBUGFLAG_NONOISERAND    DEBUGFLAG_04      
#define      FLAG_NONOISERAND    (g_dwDebugFlags & DEBUGFLAG_NONOISERAND)
#define DEBUGFLAG_NOSEANOISES    DEBUGFLAG_05      
#define      FLAG_NOSEANOISES    (g_dwDebugFlags & DEBUGFLAG_NOSEANOISES)
#define      FLAG_SEANOISES      (!FLAG_NOSEANOISES)
#define DEBUGFLAG_NOCARNOISES    DEBUGFLAG_06      
#define      FLAG_NOCARNOISES    (g_dwDebugFlags & DEBUGFLAG_NOCARNOISES)
#define      FLAG_CARNOISES      (!FLAG_NOCARNOISES)
#define DEBUGFLAG_NOSPECTRUM     DEBUGFLAG_07      
#define      FLAG_NOSPECTRUM     (g_dwDebugFlags & DEBUGFLAG_NOSPECTRUM)
#define      FLAG_SPECTRUM       (!FLAG_NOSPECTRUM)
#define DEBUGFLAG_NOADDACL       DEBUGFLAG_08      
#define      FLAG_NOADDACL       (g_dwDebugFlags & DEBUGFLAG_NOADDACL)
#define      FLAG_ADDACL         (!FLAG_NOADDACL)
#define DEBUGFLAG_NOADDNBN       DEBUGFLAG_09      
#define      FLAG_NOADDNBN       (g_dwDebugFlags & DEBUGFLAG_NOADDNBN)
#define      FLAG_ADDNBN         (!FLAG_NOADDNBN)
#define DEBUGFLAG_NOADDWBN       DEBUGFLAG_10      
#define      FLAG_NOADDWBN       (g_dwDebugFlags & DEBUGFLAG_NOADDWBN)
#define      FLAG_ADDWBN         (!FLAG_NOADDWBN)
#define DEBUGFLAG_SHOWSIGNAL     DEBUGFLAG_11      
#define      FLAG_SHOWSIGNAL     (g_dwDebugFlags & DEBUGFLAG_SHOWSIGNAL)
#define      FLAG_DONTSHOWSIGNAL (!FLAG_SHOWSIGNAL)
#define DEBUGFLAG_NOSIGNALRAND   DEBUGFLAG_16      
#define      FLAG_NOSIGNALRAND   (g_dwDebugFlags & DEBUGFLAG_NOSIGNALRAND)
#define      FLAG_NOISERAND      (!FLAG_NONOISERAND)
#define DEBUGFLAG_SEANOISE1      DEBUGFLAG_17      
#define      FLAG_SEANOISE1      (g_dwDebugFlags & DEBUGFLAG_SEANOISE1)
//#define DEBUGFLAG_SHOWRAYPATH    DEBUGFLAG_18      
#define DEBUGFLAG_SHOWACL        DEBUGFLAG_19      
#define      FLAG_SHOWACL        (g_dwDebugFlags & DEBUGFLAG_SHOWACL)
#define DEBUGFLAG_TRACESONLY     DEBUGFLAG_20      
#define      FLAG_TRACESONLY     (g_dwDebugFlags & DEBUGFLAG_TRACESONLY)
#define DEBUGFLAG_NOREVNOISES    DEBUGFLAG_21      
#define      FLAG_NOREVNOISES    (g_dwDebugFlags & DEBUGFLAG_NOREVNOISES)
#define      FLAG_REVNOISES      (!FLAG_NOREVNOISES)
#define DEBUGFLAG_400000         DEBUGFLAG_22      
#define DEBUGFLAG_NOTRACER       DEBUGFLAG_23      
#define      FLAG_NOTRACER       (g_dwDebugFlags & DEBUGFLAG_NOTRACER)
#define DEBUGFLAG_KAPPA          DEBUGFLAG_24      
#define      FLAG_KAPPA          (g_dwDebugFlags & DEBUGFLAG_KAPPA)
#define DEBUGFLAG_NOTRK          DEBUGFLAG_25      
#define      FLAG_NOTRK          (g_dwDebugFlags & DEBUGFLAG_NOTRK)
#define      FLAG_TRK            (!FLAG_NOTRK)
#define DEBUGFLAG_NOHLCORRSAMPLE DEBUGFLAG_26      
#define      FLAG_NOHLCORRSAMPLE (g_dwDebugFlags & DEBUGFLAG_NOHLCORRSAMPLE)
#define DEBUGFLAG_1              DEBUGFLAG_27      
#define      FLAG_1              (g_dwDebugFlags & DEBUGFLAG_1)
#define DEBUGFLAG_2              DEBUGFLAG_28      
#define      FLAG_2              (g_dwDebugFlags & DEBUGFLAG_2)
#define DEBUGFLAG_3              DEBUGFLAG_29      
#define      FLAG_3              (g_dwDebugFlags & DEBUGFLAG_3)
#define DEBUGFLAG_UNNEEDEDOFF    DEBUGFLAG_30      
#define      FLAG_UNNEEDEDOFF    (g_dwDebugFlags & DEBUGFLAG_UNNEEDEDOFF)
#define      FLAG_UN             FLAG_UNNEEDEDOFF
