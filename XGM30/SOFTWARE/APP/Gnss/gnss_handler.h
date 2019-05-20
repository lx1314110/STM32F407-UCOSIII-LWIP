/*****************************************************************************/
/* 文件名:    gnss_handler.h                                                 */
/* 描  述:    GNSS相关设置/查询处理头文件                                    */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __GNSS_HANDLER_H_
#define __GNSS_HANDLER_H_
#include "arch/cc.h"


/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/
/* 寄存器地址定义 */
#define GNSS_GPS_PPS_OFFSET_REG_ADDR0 0x0027
#define GNSS_GPS_PPS_OFFSET_REG_ADDR1 0x0028
#define GNSS_GPS_PPS_OFFSET_REG_ADDR2 0x0029
#define GNSS_GPS_PPS_OFFSET_REG_ADDR3 0x0030

#define GNSS_ALARM_REG_ADDR           0x0009 /*bit0 1pps loss, bit1 serial info loss*/  
#define GNSS_PPS_AVAIL_REG_ADDR       0x0041 /*0 unavailable, 1 available*/ 


#define GNSS_PPS_LOSS_BIT             0x0001
#define GNSS_SER_LOSS_BIT             0x0002
#define GNSS_PPS_AVAIL_FITER          60
#define available                     1
#define unavailable                   0

#define GNSS_PPS_DLYCOM_HI            1000000
#define GNSS_PPS_DLYCOM_LO           -1000000
/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
void gnss_global_variable_init(void);
void gnss_init(void);
u8_t gnss_acmode_set_handler(int num,...);
u8_t gnss_serial_baud_set_handler(int num,...);
u8_t gnss_echo_set_handler(int num,...);
u8_t gnss_debug_set_handler(u8_t *p_param, u8_t *p_rtrv);
u8_t gnss_stmt_set_handler(int num,...);
u8_t gnss_get_info_handler(u8_t *p_param, u8_t *p_rtrv);
u8_t gnss_inquery_set(u8_t *p_param, u8_t *p_rtrv);
u8_t gnss_helper(int num,...);
u8_t gnss_command_execute(int num,...);
void gnss_info_print(void);


extern void get_gnss_info(unsigned char *type, char startch, unsigned char *endstr, unsigned char *tmp);
extern void split_gnss_info(unsigned char *type, char startch, char endch,unsigned char split_num,
                            unsigned char end_num, unsigned char *tmp);
extern void split1_gnss_info(unsigned char *type, char* startstr, char endch,
                             unsigned char *tmp);

void gnss_stmt_echo(u8_t *p_msg);
void check_gnss_available(void);

u8_t gnss_set_gps_pps_offset(int offset);
int gnss_get_gps_pps_offset(void);

#endif
