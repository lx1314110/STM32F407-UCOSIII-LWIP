/*****************************************************************************/
/* 文件名:    debug.c                                                        */
/* 描  述:    调试处理                                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "mtfs30.h"
#include "Debug/mtfs30_debug.h"
#include "Debug/usart_debug_task.h"
#include <includes.h>
#include "arch/cc.h"
#include "bsp_usart.h"

/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
u8_t                    g_debug_switch;        /* 串口调试开关变量   */
u8_t                    g_debug_level = DBG_DISABLE;

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/


/*****************************************************************************/
/* 函  数:    debug_task_create                                              */
/* 功  能:    创建调试任务                                                   */
/* 输  入:    无                                                             */
/* 输  出:    无                                                             */
/* 返回值:    无                                                             */
/* 创  建:    2018-05-12 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/*****************************************************************************/
void debug_task_create( void )
{

    
    /* 创建串口调试任务 */
    usart_debug_task_create();  
}


/*****************************************************************************
 * 函  数:    debug_msg_format                                                           
 * 功  能:    将调试信息格式化后再输出                                                                 
 * 输  入:    无                       
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void debug_msg_format(u8_t *format, ...)
{
        u16_t len = 0;
        u8_t buf[DEBUG_MSG_MAX_LEN] = {0}; /* 用于存放格式化后的调试信息 */
        va_list args; 
        

        /* 格式化调试信息 */
        va_start(args, format);
        vsnprintf((char *)buf, DEBUG_MSG_MAX_LEN - 3, (const char *)format, args);
        va_end(args);
        
        
        /* 格式化为串口消息格式 */
        len = strlen((const char *)buf);
        buf[len] = '\r';
        buf[len + 1] = '\n';
        buf[len + 2] = '\0';
        
        len = len + 2; 
        /* 通过串口输出调试信息 */
        DEBUG_USART_Send((u8_t *)buf, len);

        
#if MTFS30_NET_DEBUG_EN
           
        /* 通过网络输出调试信息 */
        net_debug((u8_t *)buf, len);
#endif
                  
}
void debug_level_format(u8_t *format, ...)
{
        u16_t len = 0;
        u8_t buf[DEBUG_MSG_MAX_LEN] = {0}; /* 用于存放格式化后的调试信息 */
        int level = 0;
        va_list args; 
        

        /* 格式化调试信息 */
        va_start(args, format);
        level = va_arg(args, int);
        if(level <= g_debug_level)
          vsnprintf((char *)buf, DEBUG_MSG_MAX_LEN - 3, (const char *)format, args);
        va_end(args);
        
        if(level <= g_debug_level)
        {
        /* 格式化为串口消息格式 */
          len = strlen((const char *)buf);
          buf[len] = '\r';
          buf[len + 1] = '\n';
          buf[len + 2] = '\0';
          
          len = len + 2; 
          /* 通过串口输出调试信息 */
          DEBUG_USART_Send((u8_t *)buf, len);
       }

        
#if MTFS30_NET_DEBUG_EN
           
        /* 通过网络输出调试信息 */
        net_debug((u8_t *)buf, len);
#endif
                  
}


/*****************************************************************************
 * 函  数:    mtfs30_debug_set_handler                                                           
 * 功  能:    打开/关闭调试信息                                                                
 * 输  入:    p_param: 指向调试开关信息     
 * 输  出:    无                                                    
 * 返回值:    OK: 设置成功; NG：设置失败                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t mtfs30_debug_set_handler(int num,...)
{
    va_list valist;
    u8_t *p_param = NULL;
    u8_t err_flag = OK;
    
    
    /*initalize the varlist */
    va_start(valist, num);
    p_param = va_arg(valist, char *);
    if(p_param != NULL)
    {
      if (0 == strncmp(p_param, "OFF", strlen("OFF")))
      {
          /* 关闭调试 */
          g_debug_switch = 0;
      }
      else if (0 == strncmp((char const *)p_param, "ON",strlen("ON")))
      {
          /* 打开调试 */
          g_debug_switch = 1;
      }
      else
      {
          MTFS30_TIPS("参数(%s)有误!\n", p_param);  
          err_flag = NG;
      }
    }
    /*clear valist memory*/
    va_end(valist);
 
    return err_flag; 
}

u8_t debug_level_set_handler(int num,...)
{
    u8_t *p_param = NULL;
    u8_t  err_flag = OK;
    int level = 0;
    va_list valist;
    
    va_start(valist, num);
    p_param =(u8_t *) va_arg(valist, char *);
    
    if(p_param != NULL)
    {
      level = atoi(p_param);
      
      if(level >= DBG_DISABLE && level <= DBG_DEBUG)
      {
      
          /* debug level */
          g_debug_level = level;
          MTFS30_TIPS("set debug level = %d", g_debug_level);
      }
      else
      {
          err_flag = NG;
          MTFS30_TIPS("参数(%s)错误", p_param);      
      }
        
      
    }
    va_end(valist);
    
    return err_flag;
}