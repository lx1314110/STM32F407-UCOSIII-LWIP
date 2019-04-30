/*****************************************************************************/
/* 文件名:    system_manage.h                                                */
/* 描  述:    系统管理头文件                                                 */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#ifndef __SYSTEM_H_
#define __SYSTEM_H_


#define SYSTEM_HARDWARE_VERSION_LEN 7
#define SYSTEM_FPGA_VERSION_LEN 7

#define SYSTEM_HARDWARE_VERSION_HIGH_REG_ADDR 0x0001
#define SYSTEM_HARDWARE_VERSION_LOW_REG_ADDR 0x0002

#define SYSTEM_FPGA_VERSION_HIGH_REG_ADDR 0x0001
#define SYSTEM_FPGA_VERSION_LOW_REG_ADDR 0x0002

#define SYSTEM_WDI_ENABLE_REG_ADDR 0x0003
#define SYSTEM_WDI_REG_ADDR 0x0004
#define SYSTEM_LEAP_SEC_REG_ADDR 0X0007
#define SYSTEM_TIME_SKEW_REG_ADDR 0X0008
#define SYSTEM_PLL_STATE_REG_ADDR 0x0010
#define SYSTEM_PHASE_REG_ADDR0 0x0031
#define SYSTEM_PHASE_REG_ADDR1 0x0032
#define SYSTEM_PHASE_REG_ADDR2 0x0033


#define SYSTEM_ALARM_LED_CTL_REG_ADDR 0x0039
#define SYSTEM_GNSS_LED_CTL_REG_ADDR 0x0039
#define SYSTEM_REF_SOURCE_ALARM_REG_ADDR 0x0041
#define SYSTEM_OUT_SIGNAL_TYPE_REG_ADDR 0x0045
#define SYSTEM_REFACTORY_SIGNAL_REG_ADDR 0x0046

#define SYSTEM_VLAN_ENABLE_REG_ADDR 0x0100
#define SYSTEM_VLAN_PROI_REG_ADDR 0x0101
#define SYSTEM_VLAN_CFI_REG_ADDR 0x0102
#define SYSTEM_VLAN_ID_REG_ADDR0 0x0103
#define SYSTEM_VLAN_ID_REG_ADDR1 0x0104

#define SYSTEM_MAC_REG_ADDR0 0X0105
#define SYSTEM_MAC_REG_ADDR1 0X0106
#define SYSTEM_MAC_REG_ADDR2 0X0107
#define SYSTEM_MAC_REG_ADDR3 0X0108
#define SYSTEM_MAC_REG_ADDR4 0X0109
#define SYSTEM_MAC_REG_ADDR5 0X0110


#define SYSTEM_IP_REG_ADDR0 0X0111
#define SYSTEM_IP_REG_ADDR1 0X0112
#define SYSTEM_IP_REG_ADDR2 0X0113
#define SYSTEM_IP_REG_ADDR3 0X0114

#define SYSTEM_MASK_REG_ADDR0 0X0115
#define SYSTEM_MASK_REG_ADDR1 0X0116
#define SYSTEM_MASK_REG_ADDR2 0X0117
#define SYSTEM_MASK_REG_ADDR3 0X0118

#define SYSTEM_GATEWAY_REG_ADDR0 0X0105
#define SYSTEM_GATEWAY_REG_ADDR1 0X0106
#define SYSTEM_GATEWAY_REG_ADDR2 0X0107
#define SYSTEM_GATEWAY_REG_ADDR3 0X0108




/* 系统版本结构定义 */
typedef struct _SYSTEM_VERSION_T_
{
    u8_t hw_version[SYSTEM_HARDWARE_VERSION_LEN];
    u8_t fpga_version[SYSTEM_FPGA_VERSION_LEN];

} system_v_t;


/* 系统时间结构定义 */
typedef struct _SYSTEM_TIME_T_
{
    u16_t year; 
    u8_t  mon;
    u8_t  day;
    u8_t  hour;
    u8_t  min;
    u8_t  sec;

} system_time_t;

/*-------------------------------*/
/* 宏定义                        */
/*-------------------------------*/

/* 寄存器地址定义 */








/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
void system_set_leap(u8_t leap);

#endif
