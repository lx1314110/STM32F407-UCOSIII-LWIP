/*****************************************************************************/
/* 文件名:    debug.h                                                        */
/* 描  述:    调试处理头文件                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __MTFS30_DEBUG_H_
#define __MTFS30_DEBUG_H_
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "Config/config.h"
#include "opt.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define  DEBUG_MSG_MAX_LEN        256     /* 每条调试信息最大长度，包括最后的"\r\n\0" */

/* 调试开关 */
#define  MTFS30_DEBUG_EN             1       /* 使能调试信息输出     */
#define  MTFS30_ERROR_EN             0       /* 使能错误信息输出     */
//#define  MTFS30_NET_DEBUG_EN         0       /* 使能网络调试         */


/*-------------------------------*/
/* 变量声明                      */
/*-------------------------------*/
extern u8_t                    g_debug_switch;        /* 串口调试开关变量   */

/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void debug_init( void );
extern void debug_task_create( void );
extern void debug_msg_format(u8_t *format, ...);
extern u8_t mtfs30_debug_set_handler(int num,...);


/*-------------------------------*/
/* 宏函数定义                    */
/*-------------------------------*/
#define  MTFS30_ERROR(fmt, args...)       debug_msg_format("[ERROR][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args) /* 输出错误信息 */
#define  MTFS30_DEBUG(fmt, args...)       if (g_debug_switch) {debug_msg_format("[DEBUG][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args);} /* 输出调试信息 */
#define  MTFS30_TIPS(fmt, args...)        debug_msg_format(fmt, ##args)                                          /* 输出提示信息 */

#endif