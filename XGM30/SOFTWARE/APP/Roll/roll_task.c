/*****************************************************************************/
/* 文件名:    roll_task.c                                                    */
/* 描  述:    循环任务                                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"
#include "Roll/roll_task.h"
#include "Roll/roll_print.h"

/* 闰秒预告测试 */
//#include "Util/util_ringbuffer.h"
#include "BSP/inc/bsp_usart.h"

#include "Gnss/gnss_handler.h"
//extern ringbuffer_t g_gnss_usart_rb;           /* 卫星消息缓冲区     */


/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
OS_TCB            g_roll_task_tcb;                       /* 循环任务控制块 */
static CPU_STK    g_roll_task_stk[ROLL_TASK_STACK_SIZE]; /* 循环任务栈     */


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern u8_t                    g_roll_print_switch;   /* 循环打印开关变量   */


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void roll_task(void *pdata);


/*****************************************************************************
 * 函  数:    roll_task_create                                                         
 * 功  能:    创建循环任务                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void roll_task_create(void)
{
    OS_ERR err;


    /* 创建循环任务 */
    OSTaskCreate( (OS_TCB        *)	&g_roll_task_tcb,
				  (CPU_CHAR      *)	"roll task",
				  (OS_TASK_PTR    )	roll_task,
				  (void          *) 0,
				  (OS_PRIO        )	ROLL_TASK_PRIO,
				  (CPU_STK       *)	&g_roll_task_stk[0],
                  (CPU_STK       *) &g_roll_task_stk[ROLL_TASK_STACK_SIZE / 10],
				  (OS_STK_SIZE    )	ROLL_TASK_STACK_SIZE,
				  (OS_MSG_QTY     ) 0,
				  (OS_TICK        ) 0,
				  (void          *) 0,
				  (OS_OPT         ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				  (OS_ERR        *) &err);		

}


/*****************************************************************************
 * 函  数:    roll_task                                                           
 * 功  能:    卫星信息解析任务                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void roll_task(void *pdata)
{
 
	pdata = pdata;
        
    
    /*-------------------------------------*/
    /* 解析卫星信息                        */
    /*-------------------------------------*/
	while(1)
	{
 
       
        /* 循环打印信息 */
        if (1 == g_roll_print_switch)
        {
            roll_print_info();
        }
        
        check_gnss_available();
        
        /* 循环打印栈信息 */
        //roll_print_task_info();
        
//        /* 清除保存的卫星信息 */
//        gnss_info_clear();
       
//        /* 闰秒预告测试 */
//        static u8_t buf[256];
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#GPSUTC,98,GPS,FINE,2041,452247000,0,0,18,1;2042,61440,-4.656612873077393e-09,-1.687538997e[2019-02-22 13:37:14]-14,1929,7,18,19,0,0*305d1777\r\n");
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));  
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#BDSUTC,98,GPS,FINE,2038,541972000,0,0,18,1;0,0,7.450580596923828e-09,-2.042810365e-14,573,6,4,5,0,0*599bfd59\r\n");
////        static u8_t buf[] = "#BDSUTC,98,GPS,FINE,2038,541972000,0,0,18,1;0,0,7.450580596923828e-09,-2.042810365e-14,573,6,4,4,0,0*64fbd4e9\r\n";
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#GALUTC,98,GPS,FINE,2038,541972000,0,0,18,2;9.313225746154785e-10,-8.881784197001252e-16,18,120,1008,905,7,18,0.000000000000000e+00,0.000000000000000e+00,0,0*f1be4547\r\n");       
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#BDSUTC,98,GPS,FINE,2038,541973000,0,0,18,1;0,0,7.450580596923828e-09,-2.042810365e-14,573,6,5,5,0,0*a68ace86\r\n");
////        static u8_t buf[] = "#BDSUTC,98,GPS,FINE,2038,541972000,0,0,18,1;0,0,7.450580596923828e-09,-2.042810365e-14,573,6,4,4,0,0*64fbd4e9\r\n";
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));        
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#GALUTC,98,GPS,FINE,2038,541973000,0,0,18,2;9.313225746154785e-10,-8.881784197001252e-16,18,120,1008,905,7,19,0.000000000000000e+00,0.000000000000000e+00,0,0*820ebc39\r\n");
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#GALUTC,98,GPS,FINE,2038,541974000,0,0,18,1;9.313225746154785e-10,-8.881784197001252e-16,19,120,1008,905,7,18,0.000000000000000e+00,0.000000000000000e+00,0,0*de266c71\r\n");
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#BDSUTC,98,GPS,FINE,2038,541974000,0,0,18,1;0,0,7.450580596923828e-09,-2.042810365e-14,573,6,5,5,0,0*f3c961d6\r\n");
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#GALUTC,98,GPS,FINE,2038,541975000,0,0,18,2;9.313225746154785e-10,-8.881784197001252e-16,18,120,1008,905,7,18,0.000000000000000e+00,0.000000000000000e+00,0,0*6ab76c87\r\n");
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
//        memset(buf, 0x00, sizeof(buf));
//        strcpy(buf, "#BDSUTC,98,GPS,FINE,2038,541975000,0,0,18,1;0,0,7.450580596923828e-09,-2.042810365e-14,573,6,5,4,0,0*97cf700d\r\n");
//        util_ringbuffer_write(&g_gnss_usart_rb, buf, strlen((char const *)buf));
//        
        BSP_Sleep(1000);    
	}

}



