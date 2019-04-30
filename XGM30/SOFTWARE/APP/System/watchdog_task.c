/*****************************************************************************/
/* 文件名:    watchdog_task.c                                                */
/* 描  述:    看门狗任务                                                     */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "System/watchdog_task.h"
#include "bsp_spi.h"
#include "led_interrupt.h"

/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
OS_TCB            g_watchdog_task_tcb;                       /* 看门狗任务控制块 */
static CPU_STK    g_watchdog_task_stk[WATCHDOG_TASK_STACK_SIZE]; /* 看门狗任务栈     */


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/



/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void watchdog_task(void *pdata);


/*****************************************************************************
 * 函  数:    watchdog_task_create                                                         
 * 功  能:    创建看门狗任务                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void watchdog_task_create(void)
{
    OS_ERR err;


    /* 创建看门狗任务 */
    OSTaskCreate( (OS_TCB        *)	&g_watchdog_task_tcb,
				  (CPU_CHAR      *)	"watchdog task",
				  (OS_TASK_PTR    )	watchdog_task,
				  (void          *) 0,
				  (OS_PRIO        )	WATCHDOG_TASK_PRIO,
				  (CPU_STK       *)	&g_watchdog_task_stk[0],
                  (CPU_STK       *) &g_watchdog_task_stk[WATCHDOG_TASK_STACK_SIZE / 10],
				  (OS_STK_SIZE    )	WATCHDOG_TASK_STACK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);		

}


/*****************************************************************************
 * 函  数:    watchdog_task                                                           
 * 功  能:    卫星信息解析任务                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void watchdog_task(void *pdata)
{
     //pdata = pdata;
  
    /*initialize the led and interrupt */
    led_gpio_init();
    /*enable hardware watchdog  */
    SPI_FPGA_ByteWrite(0x0, SYSTEM_WDI_ENABLE_REG_ADDR);
    
    while(1)
    {
 
        SPI_FPGA_ByteWrite(0x0, SYSTEM_WDI_REG_ADDR);
        SPI_FPGA_ByteWrite(0x1, SYSTEM_WDI_REG_ADDR);
        BSP_Sleep(500);    
     }

}



