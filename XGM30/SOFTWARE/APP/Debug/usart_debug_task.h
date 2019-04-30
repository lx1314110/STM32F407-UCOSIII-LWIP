/*****************************************************************************/
/* 文件名:    usart_debug_task.h                                             */
/* 描  述:    串口调试任务头文件                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __USART_DEBUG_TASK_H_
#define __USART_DEBUG_TASK_H_
#include "arch/cc.h"
#include <includes.h>


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define    USART_DEBUG_TASK_STACK_SIZE   300  /* 调试任务栈大小 */
#define    USART_DEBUG_THREAD_PRIO       7   /* 调试任务优先级 */


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern OS_TCB     gtask_usart_debug_tcb;                                      /* 调试任务控制块 */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void usart_debug_task_create(void);
extern void net_debug(u8_t *pnet_debug_data, u16_t len);

#endif