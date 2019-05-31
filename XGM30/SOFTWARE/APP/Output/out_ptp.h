/*****************************************************************************/
/* 文件名:    out_ptp.h                                                      */
/* 描  述:    PTP相关处理头文件                                              */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __OUT_PTP_H_
#define __OUT_PTP_H_
#include "arch/cc.h"

/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/


/* 寄存器地址定义 */
#define PTP_VLANEN_REG_ADDR                    0x0100
#define PTP_VLANPRI_REG_ADDR                   0x0101
#define PTP_VLANCFI_REG_ADDR                   0x0102
#define PTP_VLANVID_REG1_ADDR                  0x0103
#define PTP_VLANVID_REG2_ADDR                  0x0104

#define PTP_MAC_REG1_ADDR                      0x0105
#define PTP_MAC_REG2_ADDR                      0x0106
#define PTP_MAC_REG3_ADDR                      0x0107
#define PTP_MAC_REG4_ADDR                      0x0108
#define PTP_MAC_REG5_ADDR                      0x0109
#define PTP_MAC_REG6_ADDR                      0x0110
#define PTP_IP_REG1_ADDR                       0x0111
#define PTP_IP_REG2_ADDR                       0x0112
#define PTP_IP_REG3_ADDR                       0x0113
#define PTP_IP_REG4_ADDR                       0x0114
#define PTP_MASK_REG1_ADDR                     0x0115
#define PTP_MASK_REG2_ADDR                     0x0116
#define PTP_MASK_REG3_ADDR                     0x0117
#define PTP_MASK_REG4_ADDR                     0x0118
#define PTP_GATEWAY_REG1_ADDR                  0x0119
#define PTP_GATEWAY_REG2_ADDR                  0x0120
#define PTP_GATEWAY_REG3_ADDR                  0x0121
#define PTP_GATEWAY_REG4_ADDR                  0x0122

#define PTP_PORTEN_REG_ADDR                    0x0123
#define PTP_DOMAIN_REG_ADDR                    0x0124    /* DOMAIN设置寄存器地址                   */
#define PTP_FLAG_SECU_REG_ADDR                 0x0125    /* PTP security设置寄存器地址             */
#define PTP_FLAG_PS2_REG_ADDR                  0x0126    /* PTP profile Specific 2设置寄存器地址   */
#define PTP_FLAG_PS1_REG_ADDR                  0x0127    /* PTP profile Specific 1设置寄存器地址   */
#define PTP_FLAG_ALT_MASTER_REG_ADDR           0x0128    /* alternateMasterFlag设置寄存器地址      */
#define PTP_FLAG_FREQ_TRAC_REG_ADDR            0x0129    /* frequencyTraceable设置寄存器地址       */
#define PTP_FLAG_TIME_TRAC_REG_ADDR            0x0130    /* timeTraceable设置寄存器地址            */
#define PTP_FLAG_TIME_SCALE_REG_ADDR           0x0131    /* ptpTimescale设置寄存器地址             */
#define PTP_FLAG_CUR_UTC_OFFSET_VALID_REG_ADDR 0x0132    /* currentUtcOffsetValid设置寄存器地址    */
#define PTP_FLAG_LEAP59_REG_ADDR               0x0133    /* leap59设置寄存器地址                   */
#define PTP_FLAG_LEAP61_REG_ADDR               0x0134    /* leap61设置寄存器地址                   */
#define PTP_UTC_OFFSET_REG_ADDR0               0x0135    /* currentUtcOffset设置寄存器地址0        */
#define PTP_UTC_OFFSET_REG_ADDR1               0x0136    /* currentUtcOffset设置寄存器地址1        */
#define PTP_GM_PRIO1_REG_ADDR                  0x0137    /* grandmasterPriority1设置寄存器地址     */
#define PTP_GM_CLK_QUAL_REG_ADDR0              0x0138    /* grandmasterClockQuality设置寄存器地址0 */
#define PTP_GM_CLK_QUAL_REG_ADDR1              0x0139    /* grandmasterClockQuality设置寄存器地址1 */
#define PTP_GM_CLK_QUAL_REG_ADDR2              0x0140    /* grandmasterClockQuality设置寄存器地址2 */
#define PTP_GM_CLK_QUAL_REG_ADDR3              0x0141    /* grandmasterClockQuality设置寄存器地址3 */
#define PTP_GM_PRIO2_REG_ADDR                  0x0142    /* grandmasterPriority2设置寄存器地址     */
#define PTP_STEPS_RM_REG_ADDR0                 0x0143    /* stepsRemoved设置寄存器地址0            */
#define PTP_STEPS_RM_REG_ADDR1                 0x0144    /* stepsRemoved设置寄存器地址1            */
#define PTP_TIME_SOURCE_REG_ADDR               0x0145    /* timeSource设置寄存器地址               */
#define PTP_UNIMUL_REG_ADDR                    0x0146    /* 单播多播类型设置寄存器地址             */
#define PTP_LAYER_REG_ADDR                     0x0147    /* 报文封装格式设置寄存器地址             */
#define PTP_STEP_REG_ADDR                      0x0148    /* 时戳发送模式设置寄存器地址             */
#define PTP_DELAY_REG_ADDR                     0x0149    /* 时戳发送模式设置寄存器地址             */
#define PTP_ANNOUNCE_REG_ADDR                  0x0150    /* announce报文发包间隔置寄存器地址       */
#define PTP_SYNC_REG_ADDR                      0x0151    /* sync报文发包间隔设置寄存器地址         */
#define PTP_ESMC_REG_ADDR                      0x0152    /* ESMC报文使能/不使能设置寄存器地址      */
#define PTP_SSM_REG_ADDR                       0x0153    /* SSM值设置寄存器地址  */
#define PTP_NTPEN_REG_ADDR                     0x0154    /* SSM值设置寄存器地址  */
#define PTP_MTC_START_ADDR                     0x00020000


#define PTP_FLAG_GM_PRIO_REG_ADDR 0X0127
//#define PTP_FLAG_CUO_HIGH_REG_ADDR 0x0127 
//#define PTP_FLAG_CUO_LOW_REG_ADDR 0x0127

//#define PTP_FLAG_GMCQ_HIGH1_REG_ADDR 0x0127
//#define PTP_FLAG_GMCQ_HIGH2_REG_ADDR 0x0127
//#define PTP_FLAG_GMCQ_LOW1_REG_ADDR 0x127
//#define PTP_FLAG_GMCQ_LOW2_REG_ADDR 0x127
//#define PTP_FLAG_GMPRIO2_REG_ADDR 0x0127




#define PTP_MAC0_REG_ADDR 0X0105
#define PTP_MAC1_REG_ADDR 0X0106
#define PTP_MAC2_REG_ADDR 0X0107
#define PTP_MAC3_REG_ADDR 0X0108
#define PTP_MAC4_REG_ADDR 0X0109
#define PTP_MAC5_REG_ADDR 0X0110

#define PTP_IP0_REG_ADDR  0X0105
#define PTP_IP1_REG_ADDR  0X0106
#define PTP_IP2_REG_ADDR  0X0107
#define PTP_IP3_REG_ADDR  0X0108

#define PTP_MASK0_REG_ADDR 0X0105
#define PTP_MASK1_REG_ADDR 0X0106
#define PTP_MASK2_REG_ADDR 0X0107
#define PTP_MASK3_REG_ADDR 0X0108

#define PTP_GATEWAY0_REG_ADDR 0X0105
#define PTP_GATEWAY1_REG_ADDR 0X0106
#define PTP_GATEWAY2_REG_ADDR 0X0107
#define PTP_GATEWAY3_REG_ADDR 0X0108

//#define PTP_VLANENABLE_REG_ADDR   0x0100
//#define PTP_VLANPROI_REG_ADDR     0x0100
//#define PTP_VLANCFI_REG_ADDR      0x0100
//#define PTP_VLANID_HIGH_REG_ADDR  0x0100
//#define PTP_VLANID_LOW_REG_ADDR   0x0100

/*ptp command line */
#define MAC_STR_LIMIT_LEN       12
#define MAC_DIG_FIX_LEN         6
#define MAX_IPADDR_LEN          15
#define IP_DIG_FIX_NUM          4
//#define PTP2_PORT_SUBCOM_NUM    12
#define PTP_MESSAGE_FRE_NUM     15
#define PTP_MESSAGE_FRE_OFFSET  8
#define PTP_MESSAGE_MIN_FRE     -8
#define PTP_MESSAGE_MAX_FRE     6
#define PTP_BYTE_MAX_VALUE      255
#define PTP_BYTE_MIN_VALUE      0
#define PTP_SLAVE_IP_TB_LEN     1024
#define PTP_QULITY_SPLIT_NUM      2
#define PTP_PRIORITY_SPLIT_NUM    1

/**********************************
* function define                      
***********************************/
u8_t ptp_set_handler(int num,...);
u8_t ptp_ipconfig_handler(int num,...);
u8_t ptp_ifconfig_handler(int num,...);
extern unsigned char StringtoInt( char* s );

#endif
