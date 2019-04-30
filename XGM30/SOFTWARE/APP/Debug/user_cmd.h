/*****************************************************************************/
/* 文件名:    user_cmd.h                                                     */
/* 描  述:    用户命令处理相关头文件                                         */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __USER_CMD_H_
#define __USER_CMD_H_
#include "arch/cc.h"
#include "Gnss/gnss.h"


/* 用户命令处理器结构定义 */
typedef struct  _USER_CMDHANDLE_T_
{
    u8_t  cmd_type; /* 命令类型: 设置命令|查询命令 */
	u8_t* p_head;   /* 命令头                      */
    /* p_param: 命令信息， p_rtrv：存放查询结果 */
	u8_t (*cmd_fun)(int num,...);
} user_cmdhandle_t;


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define USER_CMD_SET      0   /* 设置命令               */
#define USER_CMD_GET      1   /* 查询命令               */
#define RTRV_BUF_MAX_SIZE GNSS_SAT_INFO_MAX_LEN /* 查询结果缓冲区最大大小 （最大数据为卫星信息） */
#define USER_CMD_MAX_LEN  128   /* 命令最大长度 */

#define MAX_PARAM_NUM     6

#define GNSS_COMMAND_RTRV      3
#define GNSS_COMMAND_SET       4


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void telnet_cmd_handle(u8_t *pdata);
extern void user_cmd_parser(u8_t *pmsg);
#endif