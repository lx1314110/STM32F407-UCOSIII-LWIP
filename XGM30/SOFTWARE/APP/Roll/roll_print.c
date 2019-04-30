/*****************************************************************************/
/* 文件名:    roll_print.c                                                    */
/* 描  述:    循环打印                                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"
#include "Roll/roll_print.h"
#include "mtfs30.h"
#include "Debug/mtfs30_debug.h"
#include "Gnss/gnss_handler.h"
#include "Roll/roll_task.h"
#include "Debug/usart_debug_task.h"
#include "Gnss/gnss_task.h"



/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
u8_t                    g_roll_print_switch;   /* 循环打印开关变量   */


/*-------------------------------*/
/* 全局变量声明                  */
/*-------------------------------*/


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static void roll_print_gnss_info(void);


/*****************************************************************************
 * 函  数:    roll_print_info                                                         
 * 功  能:    循环打印信息                                                                 
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void roll_print_info(void)
{
    
    
    /* 循环打印GNSS信息 */
    roll_print_gnss_info();	

}


/*****************************************************************************
 * 函  数:    roll_print_gnss_inf                                                           
 * 功  能:    循环打印GNSS信息                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static void roll_print_gnss_info(void)
{ 
    /* 打印GNSS信息 */
    gnss_info_print();
}


/*****************************************************************************
 * 函  数:    roll_print_set_handler                                                           
 * 功  能:    打开/关闭循环打印                                                                
 * 输  入:    p_param: 指向循环打印开关信息     
 * 输  出:    无                                                    
 * 返回值:    OK: 设置成功; NG：设置失败                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t roll_print_set_handler(int num,...)
{
    u8_t *p_param = NULL;
    u8_t  err_flag = OK;
    va_list valist;
    
    va_start(valist, num);
    p_param =(u8_t *) va_arg(valist, char *);
    
    if(p_param != NULL)
    {
      if (0 == strncmp((char const *)p_param, "OFF", strlen("OFF")))
      {
          /* 关闭循环打印 */
          g_roll_print_switch = 0;
      }
      else if (0 == strncmp((char const *)p_param, "ON", strlen("ON")))
      {
          /* 打开循环打印 */
          g_roll_print_switch = 1;
      }
      else
      {
          MTFS30_TIPS("参数(%s)有误!\n", p_param);  
          err_flag = NG;
      }
    }
    va_end(valist);
    
    return err_flag;
}


/*****************************************************************************
 * 函  数:    roll_print_task_info                                                           
 * 功  能:    循环打印任务栈信息                                                                
 * 输  入:    无    
 * 输  出:    无                                                    
 * 返回值:    无                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
void roll_print_task_info(void)
{
    OS_STK_SIZE free;
    OS_STK_SIZE used;
    OS_ERR err;
    

    OSTaskStkChk(&gtask_usart_debug_tcb, &free, &used, &err);
    MTFS30_TIPS("【调试任务栈】 已使用:%d, 未使用:%d, 使用率:%d%%", used, free, ((used * 100)/(free + used)));

    
    OSTaskStkChk(&g_gnss_task_tcb, &free, &used, &err);
    MTFS30_TIPS("【卫星任务栈】 已使用:%d, 未使用:%d, 使用率:%d%%", used, free, ((used * 100)/(free + used)));
    
    
    OSTaskStkChk(&g_roll_task_tcb, &free, &used, &err);
    MTFS30_TIPS("【循环任务栈】 已使用:%d, 未使用:%d, 使用率:%d%%", used, free, ((used * 100)/(free + used)));
    
}