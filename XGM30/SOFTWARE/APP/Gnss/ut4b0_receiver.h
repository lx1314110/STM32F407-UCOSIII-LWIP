/*****************************************************************************/
/* 文件名:    ut4b0_receiver.h                                               */
/* 描  述:    UT4B0接收机头文件                                              */
/* 创  建:    2018-07-19 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __UT4B0_RECEIVER_H_
#define __UT4B0_RECEIVER_H_
#include "arch/cc.h"
#include "mtfs30.h"





/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define  CHECKCODE_EQ    0     /* 校验码相同 */
#define  CHECKCODE_NOEQ  1     /* 校验码不同 */


 
#define UT4B0_CONFIG_CMD_MAX_LEN    50 /* UT4B0配置命令最大长度 */
#define UT4B0_COMBCMD_MAX_NUM  5       /* 组合命令包含的命令最大数 */


/*-------------------------------*/
/* 结构定义                      */
/*-------------------------------*/

/* 组合命令结构 */
typedef struct _UT4B0_COMBCMD_T_
{
    u8_t num;                            /* 组合命令中包含的命令条数 */
    u8_t index[UT4B0_COMBCMD_MAX_NUM];   /* 每条命令在buf中的位置    */
    u8_t buf[UT4B0_CONFIG_CMD_MAX_LEN];  /* 用于保存组合命令         */
} ut4b0_combcmd_t;



/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern void ut4b0_receiver_init(void);
extern void ut4b0_receiver_msg_parser(u8_t *pdata);
u8_t ut4b0_receiver_acmode_set(u8_t acmode);
extern void t4b0_receiver_msg_set(u8_t reset_mode);
extern u8_t ut4b0_receiver_serial_baud_set(u8_t *pbaud);
extern void ut4b0_receiver_echo_set(u8_t *pmsg);
extern u8_t ut4b0_receiver_statement_set(u8_t on_off, u8_t statement);
extern void ut4b0_rtrv_mode(u8_t **pmode);
extern void ut4b0_rtrv_location(u8_t **plocation);
extern void ut4b0_rtrv_status(u8_t **pstatus);
extern void ut4b0_rtrv_leap_forecast(u8_t **pleap_forecast);
extern u8_t ut4b0_receiver_msg_set(void);

#endif
