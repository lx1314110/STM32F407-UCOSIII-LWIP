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
#define PTP_SSM_REG_ADDR                       0x0153    /* SSM值设置寄存器地址                    */











 

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

#define PTP_IP0_REG_ADDR 0X0105
#define PTP_IP1_REG_ADDR 0X0106
#define PTP_IP2_REG_ADDR 0X0107
#define PTP_IP3_REG_ADDR 0X0108

#define PTP_MASK0_REG_ADDR 0X0105
#define PTP_MASK1_REG_ADDR 0X0106
#define PTP_MASK2_REG_ADDR 0X0107
#define PTP_MASK3_REG_ADDR 0X0108

#define PTP_GATEWAY0_REG_ADDR 0X0105
#define PTP_GATEWAY1_REG_ADDR 0X0106
#define PTP_GATEWAY2_REG_ADDR 0X0107
#define PTP_GATEWAY3_REG_ADDR 0X0108

#define PTP_VLANENABLE_REG_ADDR 0x0100
#define PTP_VLANPROI_REG_ADDR 0x0100
#define PTP_VLANCFI_REG_ADDR 0x0100
#define PTP_VLANID_HIGH_REG_ADDR 0x0100
#define PTP_VLANID_LOW_REG_ADDR 0x0100

/* PTP结构定义 */
typedef struct _PTP_T_
{
    u8_t  domain;                       /* DOMAIN                  */
    u16_t flag_secu:1;                  /* PTP security            */
    u16_t flag_ps2:1;                   /* PTP profile Specific 2  */
    u16_t flag_ps1:1;                   /* PTP profile Specific 1  */ 
    u16_t flag_alt_master:1;            /* alternateMasterFlag     */
    u16_t flag_freq_trac:1;             /* frequencyTraceable      */
    u16_t flag_time_trac:1;             /* timeTraceable           */
    u16_t flag_time_scale:1;            /* ptpTimescale            */
    u16_t flag_cur_utc_offset_valid:1;  /* currentUtcOffsetValid   */
    u16_t flag_leap59:1;                /* leap59                  */
    u16_t flag_leap61:1;                /* leap61                  */
    u16_t unimul:1;                     /* 单播多播类型            */
    u16_t layer:1;                      /* 封装方式                */
    u16_t step:1;                       /* 时戳发送模式            */
    u16_t delay:1;                      /* 时延类型                */
    u16_t emsc:1;                       /* EMSC使能                */
    u16_t :1;                           /*                         */
    u16_t utc_offset;                   /* currentUtcOffset        */
    u8_t  gm_prio1;                     /* grandmasterPriority1    */
    u32_t gm_clk_qual;                  /* grandmasterClockQuality */
    u8_t  gm_prio2;                     /* grandmasterPriority2    */    
    u16_t steps_rm;                     /* stepsRemoved            */
    u8_t  time_source;                  /* timeSource              */
    u8_t  announce;                     /* announce报文发包间隔    */
    u8_t  sync;                         /* sync报文发包间隔        */
    u8_t  ssm;                          /* ssm                     */

} ptp_t;


/* 用户命令处理器结构定义 */
typedef struct  _PTP_PARSER_T_
{
    u8_t  (*cmd_fun)(u8_t *p_param, ptp_t *p_ptp);
} ptp_parser_t;








/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
extern u8_t ptp_set_handler(u8_t *p_msg, u8_t *p_rtrv);
extern u8_t ptp_set(ptp_t *p_ptp);

#endif
