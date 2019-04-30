/*****************************************************************************/
/* 文件名:    watchdog_task.h                                                */
/* 描  述:    看门狗任务头文件                                               */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __WATCHDOG_TASK_H_
#define __WATCHDOG_TASK_H_
#include <includes.h>


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define    WATCHDOG_TASK_STACK_SIZE   128  /* 看门狗任务栈大小 */
#define    WATCHDOG_TASK_PRIO         5    /* 看门狗任务优先级 */

#define    SYSTEM_WDI_ENABLE_REG_ADDR    0x0005   /* 看门狗使能寄存器地址 */
#define    SYSTEM_WDI_REG_ADDR           0x0006   /* 看门狗喂狗寄存器地址 */

/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern OS_TCB            g_watchdog_task_tcb;                       /* 看门狗任务控制块 */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void watchdog_task_create(void);

#endif
