/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                The Real-Time Kernel
*
*
*                                      (c) Copyright 2009, Micrium, Weston, FL
*                                                 All Rights Reserved
*
*                                                  ARM Cortex-M3 Port
*
* File      : OS_CPU.H
* Version   : V3.00.4
* By        : Jean J. Labrosse
*
* For       : ARMv7M Cortex-M3
* Mode      : Thumb2
* Toolchain : IAR EWARM
************************************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H


#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif


/*
************************************************************************************************************************
*                                                        MACROS
************************************************************************************************************************
*/

#define  OS_TASK_SW()                  OSCtxSw()

#if      CPU_CFG_TS_TMR_EN == DEF_ENABLED
#define  OS_TS_GET()                   CPU_TS_TmrRd()
#else
#define  OS_TS_GET()                  (CPU_TS)0
#endif

/*
************************************************************************************************************************
*                                                      PROTOTYPES
************************************************************************************************************************
*/

void  OSCtxSw              (void);
void  OSIntCtxSw           (void);
void  OSStartHighRdy       (void);

void  OS_CPU_PendSVHandler (void);


void  OS_CPU_SysTickHandler(void);
void  OS_CPU_SysTickInit   (CPU_INT32U  cnts);

#endif
