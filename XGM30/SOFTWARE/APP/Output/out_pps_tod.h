/*****************************************************************************/
/* 文件名:    out_pps_tod.h                                                      */
/* 描  述:    1PPS+TOD相关头文件                                             */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __OUT_PPS_TOD_H_
#define __OUT_PPS_TOD_H_



/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

/* 寄存器地址定义 */
#define OUT_TOD_OFFSET_REG_ADDR0    0x0011
#define OUT_TOD_OFFSET_REG_ADDR1    0x0012
#define OUT_TOD_OFFSET_REG_ADDR2    0x0013
#define OUT_TOD_OFFSET_REG_ADDR3    0x0014
#define OUT_TOD_TYPE_REG_ADDR       0x0015
#define OUT_TOD_PPS_STATE_REG_ADDR  0x0016
#define OUT_TOD_TACC_REG_ADDR       0x0017
#define OUT_TOD_CLK_TYPE_REG_ADDR         0x0018
#define OUT_TOD_CLK_STATE_REG_ADDR0       0x0019
#define OUT_TOD_CLK_STATE_REG_ADDR1       0x0020
#define OUT_TOD_MONITOR_ALARM_REG_ADDR0   0x0021
#define OUT_TOD_MONITOR_ALARM_REG_ADDR1   0x0022

#define OUT_PPS_OFFSET_REG_ADDR0       0x0023
#define OUT_PPS_OFFSET_REG_ADDR1       0x0024
#define OUT_PPS_OFFSET_REG_ADDR2       0x0025
#define OUT_PPS_OFFSET_REG_ADDR3       0x0026

#define OUT_SIGNAL_TYPE_REG_ADDR       0x0045

#define OUT_B_OFFSET_REG_ADDR0         0x0050
#define OUT_B_OFFSET_REG_ADDR1         0x0051
#define OUT_B_OFFSET_REG_ADDR2         0x0052
#define OUT_B_OFFSET_REG_ADDR3         0x0053

#define OUT_B_LEAPPRE_REG_ADDR         0x0054
#define OUT_B_DAYLIGHT_REG_ADDR        0x0055
#define OUT_B_TIMESKEW_REG_ADDR        0x0056
#define OUT_B_TIMEQUALITY_REG_ADDR     0x0057
/*-------------------------------*/
/*-------------------------------*/
#define TIME_ZONE_NUM   24
#define WEST_1_ZONE    13
#define SIGNAL_TYPE_NUM    5
#define PPS_STATUS_LIMIT   5
#define PRECAUTION_STR_NUM 4
#define RTRV_PARMETERS_NUM 3
#define SET_PARMETERS_NUM  4

u8_t out_pps_handler(int num,...);

#endif
