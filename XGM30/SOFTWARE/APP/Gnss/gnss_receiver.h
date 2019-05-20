/*****************************************************************************/
/* 文件名:    gnss_receiver.h                                                */
/* 描  述:    GNSS接收器相关处理头文件                                       */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __GNSS_RECEIVER_H_
#define __GNSS_RECEIVER_H_
#include "gnss.h"


/*-------------------------------*/
/* 结构定义                      */
/*-------------------------------*/


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
#define GNSS_ON       1       /* 打开 */
#define GNSS_OFF      0       /* 关闭 */

/* 语句类型 */
#define GNSS_STMT_ZDA  0       /* GGA语句      */
#define GNSS_STMT_GGA  1       /* GGA语句      */
#define GNSS_STMT_GSV  2       /* GSV语句      */
#define GNSS_STMT_UTC  3       /* 闰秒信息语句 */
#define GNSS_STMT_PPS  4       /* 1PPS信息语句 */
#define GNSS_STMT_ANT  5       /* 天线状态语句 */
#define GNSS_STMT_VER  6       /* 版本信息语句 */


/* 卫星类型 */
#define GNSS_SAT_TYPE_GPS  0       /* GPS卫星    */
#define GNSS_SAT_TYPE_BDS  1       /* BDS卫星    */
#define GNSS_SAT_TYPE_GLO  2       /* GLO卫星    */
#define GNSS_SAT_TYPE_GAL  3       /* GAL卫星    */  




/* 使用的定位卫星 */
#define REV_USED_GPS         0x01    /* 使用GPS定位                  */
#define REV_USED_BDS         0x02    /* 使用BDS定位                  */
#define REV_USED_GLO         0x04    /* 使用GLO定位                  */
#define REV_USED_GAL         0x08    /* 使用GAL定位                  */

/* 接收机工作模式 */
#define REV_ACMODE_MIX         0    /* GPS、BDS、GLO、GAL联合定位 */ 
#define REV_ACMODE_GPS         1    /* 单GPS定位                  */
#define REV_ACMODE_BDS         2    /* 单BDS定位                  */
#define REV_ACMODE_GLO         3    /* 单GLO定位                  */
#define REV_ACMODE_GAL         4    /* 单GAL定位                  */


 
/* 接收机类型定义 */
#define    REV_TYPE_UT4B0    0    /* UT4B0接收机 */

/* 语句回显 */
#define    GNSS_GGA_STMT_ON       0x01 /* 打开GGA语句回显  */
#define    GNSS_GSV_STMT_ON       0x02 /* 打开GSV语句回显  */
#define    GNSS_UTC_STMT_ON       0x04 /* 打开UTC语句回显  */
#define    GNSS_PPS_STMT_ON       0x08 /* 打开PPS语句回显  */
#define    GNSS_ANT_STMT_ON       0x10 /* 打开ANT语句回显  */
#define    GNSS_ALL_STMT_ON       0X1F /* 打开全部语句回显 */
#define    GNSS_ALL_STMT_OFF      0x0  /* 关闭全部语句回显 */




/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
void gnss_receiver_msg_parser(u8_t *pdata);
void gnss_receiver_init(void);
void gnss_receiver_data_clear(void);
u8_t gnss_receiver_acmode_set(u8_t *pmode);
u8_t gnss_receiver_echo_set(u8_t *pparam);
u8_t gnss_receiver_debug_set(u8_t *p_param);
u8_t gnss_receiver_stmt_set(u8_t *p_param);
void gnss_receiver_msg_set(u8_t ac_mode);
void gnss_receiver_reset(u8_t reset_mode);
u8_t gnss_receiver_serial_baud_set(u8_t *pbaud);
u8_t gnss_receiver_get_info(u8_t *p_param, u8_t *p_rtrv);
void gnss_receiver_get_sat_info(u8_t *p_rtrv);
u8_t gnss_receiver_check_primary_mode();
void gnss_receiver_info_clear(void);

#endif
