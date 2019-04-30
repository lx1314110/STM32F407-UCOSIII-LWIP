/*****************************************************************************/
/* 文件名:    usart_debug_task.c                                             */
/* 描  述:    串口调试任务                                                   */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "opt.h"
#include "Debug/usart_debug_task.h"
//#include "Cmd/user_cmd.h"
#include "Debug/mtfs30_debug.h"






OS_TCB     gtask_usart_debug_tcb;                                      /* 调试任务控制块 */
static CPU_STK    gtask_usart_debug_stk[USART_DEBUG_TASK_STACK_SIZE];  /* 调试任务栈     */



/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern u8      g_debug_usart_rx_buf[]; /* 调试串口接收缓冲区 */
extern OS_SEM  g_usart_debug_sem;      /* 串口调试用信号量   */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void usart_debug_task(void *pdata);


/*****************************************************************************
 * 函  数:    usart_debug_task                                                           
 * 功  能:    创建串口调试任务                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void usart_debug_task_create(void)
{
    OS_ERR err;


    /* 创建卫星信息查询任务 */
    OSTaskCreate( (OS_TCB        *)	&gtask_usart_debug_tcb,
				  (CPU_CHAR      *)	"usart debug task",
				  (OS_TASK_PTR    )	usart_debug_task,
				  (void          *) 0,
				  (OS_PRIO        )	USART_DEBUG_THREAD_PRIO,
				  (CPU_STK       *)	&gtask_usart_debug_stk[0],
                  (CPU_STK       *) &gtask_usart_debug_stk[USART_DEBUG_TASK_STACK_SIZE / 10],
				  (OS_STK_SIZE    )	USART_DEBUG_TASK_STACK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);		

}


/*****************************************************************************
 * 函  数:    usart_debug_task                                                          
 * 功  能:    串口调试任务                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void usart_debug_task(void *pdata)
{
    OS_ERR err;
    //pdata = pdata;
  
    while(1)
    {
    
       /* block the serial if not receive command (serial) */
       OSSemPend ((OS_SEM *) &g_usart_debug_sem,
                  (OS_TICK ) 0,
                  (OS_OPT  ) OS_OPT_PEND_BLOCKING,
                  (CPU_TS *) NULL,
                  (OS_ERR *) &err);
       
       
       
       /* parser the command (serial) */
       //MTFS30_DEBUG("#######g_debug_usart_rx_buf: %s\n", g_debug_usart_rx_buf);
       user_cmd_parser((u8_t *)g_debug_usart_rx_buf);
        
    }
   
}