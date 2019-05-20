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
extern u8_t                    g_debug_level;        /* 串口调试开关变量   */
/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void debug_init( void );
extern void debug_task_create( void );
extern void debug_msg_format(u8_t *format, ...);
extern void debug_level_format(u8_t *format, ...);
extern u8_t mtfs30_debug_set_handler(int num,...);
extern u8_t debug_level_set_handler(int num,...);

/*
  debug class
*/
enum {
	DBG_DISABLE             = 0x00,
        DBG_EMERGENCY		= 0x01,	//system is unusable
	DBG_ALERT		= 0x02, //immediate action needed
	DBG_CRITICAL		= 0x03,	//critical conditions
	DBG_ERROR		= 0x04, //error conditions
	DBG_WARNING		= 0x05, //warning conditions
	DBG_NOTICE		= 0x06, //normal but significant condition
	DBG_INFORMATIONAL	= 0x07, //informational messages
	DBG_DEBUG 		= 0x08 	//debug level messages
};
/*-------------------------------*/
/* 宏函数定义                    */
/*-------------------------------*/
#define  MTFS30_ERROR(fmt, args...)       debug_msg_format("[ERROR][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args) /* 输出错误信息 */
#define  MTFS30_DEBUG(fmt, args...)       if (g_debug_switch) {debug_msg_format("[DEBUG][%s:%d] "fmt, __FUNCTION__, __LINE__, ##args);} /* 输出调试信息 */
#define  LVL_DEBUG(fmt,args...)           if (g_debug_level) {debug_level_format("[DEBUG][%s:%d] "fmt, __FUNCTION__, __LINE__,##args);} /* 输出调试信息 */
#define  MTFS30_TIPS(fmt, args...)        debug_msg_format(fmt, ##args)                                          /* 输出提示信息 */

#endif