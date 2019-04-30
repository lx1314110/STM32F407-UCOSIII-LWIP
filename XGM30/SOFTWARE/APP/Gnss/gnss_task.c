/*****************************************************************************/
/* 文件名:    gnss_task.c                                                    */
/* 描  述:    卫星信息解析任务                                               */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"
#include "Gnss/gnss_task.h"
#include "Gnss/gnss_receiver.h"
#include "Gnss/gnss_handler.h"
#include "Debug/mtfs30_debug.h"
#include "Util/util_ringbuffer.h"
#include "BSP/inc/bsp_usart.h"



/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
OS_TCB            g_gnss_task_tcb;                       /* 卫星信息查询任务控制块 */
static CPU_STK    g_gnss_task_stk[GNSS_TASK_STACK_SIZE]; /* 卫星信息查询任务栈     */

/*-------------------------------*/
/* 全局变量声明                  */
/*-------------------------------*/
extern ringbuffer_t g_gnss_usart_rb;           /* 卫星消息缓冲区     */
extern u8_t         g_gnss_init_finished_flag; /* 卫星初始化完成标志 */
extern OS_SEM       g_gnss_idle_sem;           /* GNSS空闲中断信号量 */
extern gnss_info_t  g_gnss_info;               /* GNSS信息           */
extern u8_t         g_gnss_cmd_send_flag;      /* 卫星命令发送标志   */
extern OS_Q         g_gnss_msgq;               /* 卫星信息消息队列   */
extern u8_t         g_gnss_echo_switch;        /* 卫星语句回显开关   */
/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void gnss_task(void *pdata);


/*****************************************************************************
 * 函  数:    gnss_task_create                                                         
 * 功  能:    创建卫星查询任务                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void gnss_task_create(void)
{
    OS_ERR err;


    /* 创建卫星信息查询任务 */
    OSTaskCreate( (OS_TCB        *)	&g_gnss_task_tcb,
				  (CPU_CHAR      *)	"gnss task",
				  (OS_TASK_PTR    )	gnss_task,
				  (void          *) 0,
				  (OS_PRIO        )	GNSS_TASK_PRIO,
				  (CPU_STK       *)	&g_gnss_task_stk[0],
                  (CPU_STK       *) &g_gnss_task_stk[GNSS_TASK_STACK_SIZE / 10],
				  (OS_STK_SIZE    )	GNSS_TASK_STACK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);		

}


/*****************************************************************************
 * 函  数:    gnss_task                                                           
 * 功  能:    卫星信息解析任务                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void gnss_task(void *pdata)
{

   u8_t *pmsg = NULL;
   OS_ERR err;
   OS_MSG_SIZE size;
	
   //pdata = pdata;
    
	while(1)
	{
        /* 从消息队列中取出卫星语句信息 */
        pmsg = OSQPend((OS_Q*        )&g_gnss_msgq,   
                       (OS_TICK      )0,
                       (OS_OPT       )OS_OPT_PEND_BLOCKING,
                       (OS_MSG_SIZE* )&size, 
                       (CPU_TS*      )0,
                       (OS_ERR*      )&err);
		
        
        if ((err == OS_ERR_NONE) && (pmsg != NULL))
	{
            
            /* 卫星语句回显 */
            if (1 == g_gnss_echo_switch)
            {
                //MTFS30_TIPS(pmsg);
                gnss_stmt_echo(pmsg);
            }
			
            /* 卫星语句解析 */
			if(1 == g_gnss_init_finished_flag)
			{	
				gnss_receiver_msg_parser(pmsg);
			}
		} 
         

	}    
    


}

//static void gnss_task(void *pdata)
//{
//
//    u16_t read_size = 0;                        /* 从缓形冲区读出的数据大小        */
//    u16_t can_read_size = 0;                    /* 环形缓冲区可读的数据大小        */
//    u8_t read_buf[RINGBUFFER_READ_MAX_SIZE + 1];/* 存放从环形缓冲区读出的数据      */  
//	
//    
//    pdata = pdata;
//    
//    /*-------------------------------------*/
//    /* 解析卫星信息                        */
//    /*-------------------------------------*/
//	while(1)
//	{
// 
//         
//        can_read_size = util_ringbuffer_can_read(&g_gnss_usart_rb);
//        if (can_read_size == 0)
//        {
//            /* 如果用户发送了卫星模块命令，为了立即接收卫星模块的回复信息,减少睡眠时间 */
//            if (g_gnss_cmd_send_flag == 1)
//            {
//                BSP_Sleep(10);
//                continue;
//            } 
//            
//            BSP_Sleep(100);
//            continue;
//        }
//
//
//        /* 等待10ms，防止数据还没写完 */
//        BSP_Sleep(10);
//       
//        /*  
//         * 取出每条数据(每条GNSS语句以"\n"结束)
//         */        
//        while((read_size = util_ringbuffer_end_ch_read(&g_gnss_usart_rb, read_buf, RINGBUFFER_READ_MAX_SIZE, '\n')) > 0)
//        {
//            
//            read_buf[read_size] = '\0';
//            MTFS30_DEBUG("数据长度 = %d, 数据: %s", read_size, read_buf);
//
//            /* 解析卫星信息 */           
//            gnss_receiver_msg_parser(read_buf);
//
//        }
//        
//        /* 如果用户发送了卫星模块命令，为了立即接收卫星模块的回复信息,跳过睡眠 */
//        if (g_gnss_cmd_send_flag == 1)
//        {
//            continue;
//        } 
//
//        BSP_Sleep(1000);
//        
//	}    
//    
//
//
//}



