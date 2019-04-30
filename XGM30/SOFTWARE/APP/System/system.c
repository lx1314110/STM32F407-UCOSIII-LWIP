/*****************************************************************************/
/* 文件名:    system.c                                                       */
/* 描  述:    系统管理相关处理                                               */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "opt.h"
#include "System/system.h"
#include "mtfs30.h"
#include "Debug/mtfs30_debug.h"
#include "BSP/inc/bsp_spi.h"





/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
static system_time_t    g_time;    /* 系统时间 */


/*-------------------------------*/
/* 全局变量声明                  */
/*-------------------------------*/


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/

/*****************************************************************************
 * 函  数:    system_update_time                                                         
 * 功  能:    更新系统时间                                                                 
 * 输  入:    无                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_update_time(u16_t year, u8_t mon, u8_t day, u8_t hour, u8_t min, u8_t sec)
{
     g_time.year = year;
     g_time.mon = mon;
     g_time.day = day;
     g_time.hour = hour;
     g_time.min = min;
     g_time.sec = sec;
}

/*****************************************************************************
 * 函  数:    system_time_get                                                       
 * 功  能:    获取系统时间                                                                 
 * 输  入:    无                         
 * 输  出:    p_time: 指向系统时间                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_time_get(system_time_t *p_time)
{

    if (p_time == NULL)
    {
         return;
    }
    
    
    *p_time = g_time;
}



/*****************************************************************************
 * 函  数:    system_hardware_version_get                                                         
 * 功  能:    获取硬件版本                                                                 
 * 输  入:    enable: 使能/不使能                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_version_get(system_v_t *p_sysv)
{
    u8_t master_v;
    u8_t sec_v;
    
    
    SPI_FPGA_ByteRead(&master_v, SYSTEM_HARDWARE_VERSION_HIGH_REG_ADDR); 
    SPI_FPGA_ByteRead(&sec_v, SYSTEM_HARDWARE_VERSION_LOW_REG_ADDR);
    
    sprintf((char *)p_sysv->hw_version, "V%02d.%02d", master_v, sec_v);
    
    
    SPI_FPGA_ByteRead(&master_v, SYSTEM_FPGA_VERSION_HIGH_REG_ADDR); 
    SPI_FPGA_ByteRead(&sec_v, SYSTEM_FPGA_VERSION_LOW_REG_ADDR);

    sprintf((char *)p_sysv->fpga_version, "V%02d.%02d", master_v, sec_v);    

}


/*****************************************************************************
 * 函  数:    system_set_wdienable                                                         
 * 功  能:    看门狗使能设置                                                                 
 * 输  入:    enable: 使能/不使能                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_wdienable(u8_t wdienable)
{   
     SPI_FPGA_ByteWrite(wdienable, SYSTEM_WDI_ENABLE_REG_ADDR);     
}


/*****************************************************************************
 * 函  数:    system_set_wdi                                                         
 * 功  能:    看门狗喂狗                                                                 
 * 输  入:    无                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_wdi()
{   
     SPI_FPGA_ByteWrite(0, SYSTEM_WDI_REG_ADDR);
     SPI_FPGA_ByteWrite(1, SYSTEM_WDI_REG_ADDR);     
}

/*****************************************************************************
 * 函  数:    system_get_pll_state                                                         
 * 功  能:    获取锁相环状态                                                                 
 * 输  入:    无                        
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_get_pll_state(u8_t *p_state)
{   
     
     SPI_FPGA_ByteRead(p_state, SYSTEM_PLL_STATE_REG_ADDR);      
}


///*****************************************************************************
// * 函  数:    system_set_tod_offset                                                        
// * 功  能:    设置输出1pps+tod信号的延时补偿                                                                 
// * 输  入:    offset: 延时补偿值                     
// * 输  出:    无                                                    
// * 返回值:    无                                                    
// * 创  建:    2018-07-18 changzehai(DTT)                            
// * 更  新:    无                                       
// ****************************************************************************/
//void system_set_tod_offset(u32_t offset)
//{   
//     u8_t tmp_offset = 0;
//     
//     
//     SPI_FPGA_ByteRead(&p_state, SYSTEM_PLLSTATE_REG_ADDR);  
//     
//}


/*****************************************************************************
 * 函  数:    system_get_phase                                                        
 * 功  能:    获取系统鉴相值                                                                 
 * 输  入:    p_phase: 指向获取到的鉴相值                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_get_phase(s32_t *p_phase)
{   
     u8_t tmp = 0;
     u32_t phase = 0;
     s8_t sign = 1;
     
     
     if (p_phase == NULL)
     {
         MTFS30_ERROR("system_get_phase()参数错误");
         return;
     }
     
     
     SPI_FPGA_ByteRead(&tmp, SYSTEM_PHASE_REG_ADDR0);
     
     
     
     if ((tmp & (1 << 7)))
     {
         sign = -1;
     }
     
     phase |= (tmp & 0x7F) << 16;
     
     
     SPI_FPGA_ByteRead(&tmp, SYSTEM_PHASE_REG_ADDR1);
     phase |= (tmp & 0xFF) << 8;
     
     
     SPI_FPGA_ByteRead(&tmp, SYSTEM_PHASE_REG_ADDR2);
     phase |= tmp & 0xFF;
     
     
     
     *p_phase = sign * phase;
         
}



/*****************************************************************************
 * 函  数:    system_vlan_enable                                                        
 * 功  能:    VLAN使能设置                                                                 
 * 输  入:    enable: 使能/不使能                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_vlan_enable(u8_t enable)
{
    
    SPI_FPGA_ByteWrite((enable&0x1), SYSTEM_VLAN_ENABLE_REG_ADDR);    
}






/*****************************************************************************
 * 函  数:    system_vlan_proi                                                        
 * 功  能:    VLAN优先级设置                                                                 
 * 输  入:    proi: 优先级                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_vlan_proi(u8_t proi)
{
    
    SPI_FPGA_ByteWrite((proi&0x7), SYSTEM_VLAN_PROI_REG_ADDR);    
}







/*****************************************************************************
 * 函  数:    system_vlan_cfi                                                        
 * 功  能:    VLAN格式设置                                                                 
 * 输  入:    cfi: 格式                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_vlan_cfi(u8_t cfi)
{
    
    SPI_FPGA_ByteWrite(cfi, SYSTEM_VLAN_CFI_REG_ADDR);    
}

/*****************************************************************************
 * 函  数:    system_vlan_id                                                       
 * 功  能:    VLAN ID设置                                                                 
 * 输  入:    id: ID                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_vlan_id(u8_t id)
{
    u8_t tmp = 0;
    
    tmp = (id &0xFF00) >> 8;
    SPI_FPGA_ByteWrite(tmp, SYSTEM_VLAN_ID_REG_ADDR0);   
    
    tmp = id &0xFF;
    SPI_FPGA_ByteWrite(tmp, SYSTEM_VLAN_ID_REG_ADDR1);      
}

/*****************************************************************************
 * 函  数:    system_set_mac                                                         
 * 功  能:    MAC地址设置                                                                 
 * 输  入:    mac: mac地址                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_mac(u8_t mac[])
{
    
    u8_t tmp_mac = 0;
    
    
    tmp_mac = mac[0] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mac, SYSTEM_MAC_REG_ADDR0);
    
    tmp_mac = mac[1] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mac, SYSTEM_MAC_REG_ADDR1);  
    
    tmp_mac = mac[2] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mac, SYSTEM_MAC_REG_ADDR2);
    
    tmp_mac = mac[3] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mac, SYSTEM_MAC_REG_ADDR3);
    
    tmp_mac = mac[4] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mac, SYSTEM_MAC_REG_ADDR4);
    
    tmp_mac = mac[5] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mac, SYSTEM_MAC_REG_ADDR5);
}


/*****************************************************************************
 * 函  数:    system_set_ip                                                         
 * 功  能:    MAC地址设置                                                                 
 * 输  入:    ip: ip地址                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_ip(u8_t ip[])
{
    
    u8_t tmp_ip = 0;
    
    
    tmp_ip = ip[0] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_ip, SYSTEM_IP_REG_ADDR0);
    
    tmp_ip = ip[1] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_ip, SYSTEM_IP_REG_ADDR1);
    
    tmp_ip = ip[2] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_ip, SYSTEM_IP_REG_ADDR2);

    tmp_ip = ip[3] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_ip, SYSTEM_IP_REG_ADDR3);    
}


/*****************************************************************************
 * 函  数:    system_set_mask                                                         
 * 功  能:    MASK设置                                                                 
 * 输  入:    mask: MASK                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_mask(u8_t mask[])
{
    
    u8_t tmp_mask = 0;
    
    
    tmp_mask = mask[0] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mask, SYSTEM_MASK_REG_ADDR0);
    
    tmp_mask = mask[1] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mask, SYSTEM_MASK_REG_ADDR1);
    
    tmp_mask = mask[2] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mask, SYSTEM_MASK_REG_ADDR2);

    tmp_mask = mask[3] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_mask, SYSTEM_MASK_REG_ADDR3);
}



/*****************************************************************************
 * 函  数:    system_set_gateway                                                        
 * 功  能:    GATEWAY设置                                                                 
 * 输  入:    gateway: GATEWAY                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_gateway(u8_t gateway[])
{
    
    u8_t tmp_gateway = 0;
    
    
    tmp_gateway = gateway[0] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_gateway, SYSTEM_GATEWAY_REG_ADDR0);
    
    tmp_gateway = gateway[1] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_gateway, SYSTEM_GATEWAY_REG_ADDR1);
    
    tmp_gateway = gateway[2] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_gateway, SYSTEM_GATEWAY_REG_ADDR2);

    tmp_gateway = gateway[3] & 0xFF;
    SPI_FPGA_ByteWrite(tmp_gateway, SYSTEM_GATEWAY_REG_ADDR3);    
}


/*****************************************************************************
 * 函  数:    system_get_refactory_signal                                                       
 * 功  能:    获取恢复出厂设置信号                                                                 
 * 输  入:    无                         
 * 输  出:    无                                                   
 * 返回值:    OK：读取到恢复出厂设置信号; NG：无恢复出厂设置信号                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t system_get_refactory_signal()
{
   
    u8_t tmp = 0;
    
    SPI_FPGA_ByteRead(&tmp, SYSTEM_REFACTORY_SIGNAL_REG_ADDR);
    
    if (tmp != 0xAA)
    {
        return NG;
    }
    else
    {
        return OK;   
    }
}


/*****************************************************************************
 * 函  数:    system_set_leap                                                       
 * 功  能:    设置闰秒                                                                 
 * 输  入:    leap：闰秒值                         
 * 输  出:    无                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_leap(u8_t leap)
{
   
    SPI_FPGA_ByteWrite(leap, SYSTEM_LEAP_SEC_REG_ADDR);
}


/*****************************************************************************
 * 函  数:    system_set_time_skew                                                      
 * 功  能:    设置UTC时间偏移                                                                 
 * 输  入:    skew：时间偏移值                         
 * 输  出:    无                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_time_skew(u8_t skew)
{
   
    SPI_FPGA_ByteWrite((skew&0x1F), SYSTEM_TIME_SKEW_REG_ADDR);
}


/*****************************************************************************
 * 函  数:    system_xo_check                                                      
 * 功  能:    晶体钟检查                                                                 
 * 输  入:    无                         
 * 输  出:    无                                                   
 * 返回值:    OK：晶体钟在线; NG: 晶体钟LOS                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t system_xo_check()
{
   static u8_t value = 0;
   u8_t tmp = 0;
   u8_t ret = OK;
   
   SPI_FPGA_ByteWrite(value, SYSTEM_TIME_SKEW_REG_ADDR);
   SPI_FPGA_ByteRead(&tmp, SYSTEM_TIME_SKEW_REG_ADDR);
   
   if (value == ~tmp)
   {
       ret = OK;
   }
   else
   {
       ret = NG;
   }
   
   value++;

   return ret;     
}


/*****************************************************************************
 * 函  数:    system_set_alarm_led                                                      
 * 功  能:    控制告警灯                                                                 
 * 输  入:    level: 告警级别                        
 * 输  出:    无                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_alarm_led(u8_t level)
{
   
    SPI_FPGA_ByteWrite(level, SYSTEM_ALARM_LED_CTL_REG_ADDR);
}

/*****************************************************************************
 * 函  数:    system_set_gnss_led                                                      
 * 功  能:    控制卫星模块运行状态灯                                                                 
 * 输  入:    state: 卫星模块运行状态                       
 * 输  出:    无                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_gnss_led(u8_t state)
{
   
    SPI_FPGA_ByteWrite(state, SYSTEM_GNSS_LED_CTL_REG_ADDR);
}

/*****************************************************************************
 * 函  数:    system_set_out_signal_type                                                      
 * 功  能:    设置输出信号类型                                                                 
 * 输  入:    type: 输出信号类型                       
 * 输  出:    无                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_out_signal_type(u8_t type)
{
   
    SPI_FPGA_ByteWrite(type, SYSTEM_OUT_SIGNAL_TYPE_REG_ADDR);
}



/*****************************************************************************
 * 函  数:    system_set_ref_source_alarm                                                      
 * 功  能:    设置参考源告警                                                                 
 * 输  入:    state: 有/无告警                       
 * 输  出:    无                                                   
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void system_set_ref_source_alarm(u8_t state)
{
   
    SPI_FPGA_ByteWrite((state&0x1), SYSTEM_REF_SOURCE_ALARM_REG_ADDR);
}







