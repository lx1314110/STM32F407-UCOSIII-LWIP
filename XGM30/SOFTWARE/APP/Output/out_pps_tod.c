/*****************************************************************************/
/* 文件名:    pps_tod.c                                                      */
/* 描  述:    1PPS+TOD相关处理                                               */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
#include "arch/cc.h"
#include "lwip/api.h"
#include "opt.h"
#include "out_pps_tod.h"
#include "bsp_spi.h"





/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/



/*-------------------------------*/
/* 全局变量声明                  */
/*-------------------------------*/


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/


/*****************************************************************************
 * 函  数:    out_set_tod_offset                                                        
 * 功  能:    设置输出1pps+tod信号的延时补偿                                                                 
 * 输  入:    offset: 延时补偿值                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void out_set_tod_offset(u32_t offset)
{   
     u8_t tmp = 0;
     
     tmp = (offset & 0x0F000000) >> 24;
     SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR0);
     
     tmp = (offset & 0xFF0000) >> 16;
     SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR1); 
     
     
     tmp = (offset & 0xFF00) >> 8;
     SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR2);
     
     
     tmp = offset & 0xFF00;
     SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR3);     
        
}





/*****************************************************************************
 * 函  数:    out_set_tod_type                                                       
 * 功  能:    设置输出1pps+tod信号的类型                                                                 
 * 输  入:    type: 延时补偿值                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void out_set_tod_type(u8_t type)
{   

     SPI_FPGA_ByteWrite((type&0x1), OUT_TOD_TYPE_REG_ADDR);     
        
}

/*****************************************************************************
 * 函  数:    out_set_pps_state                                                       
 * 功  能:    设置输出1pps+tod信号的秒脉冲状态                                                                
 * 输  入:    state: 秒脉冲状态                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void pps_tod_set_pps_state(u8_t state)
{   

     SPI_FPGA_ByteWrite(state, OUT_TOD_PPS_STATE_REG_ADDR);     
        
}

/*****************************************************************************
 * 函  数:    pps_tod_set_tacc                                                       
 * 功  能:    设置输出1pps+tod信号的抖动量级                                                              
 * 输  入:    tacc: 抖动量级                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void pps_tod_set_tacc(u8_t tacc)
{   

     SPI_FPGA_ByteWrite(tacc, OUT_TOD_TACC_REG_ADDR);     
        
}

/*****************************************************************************
 * 函  数:    out_set_tod_clk_type                                                      
 * 功  能:    设置输出1pps+tod信号的时钟源类型                                                             
 * 输  入:    type: 时钟源类型                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void out_set_tod_clk_type(u8_t type)
{   

     SPI_FPGA_ByteWrite(type, OUT_TOD_CLK_TYPE_REG_ADDR);          
}

/*****************************************************************************
 * 函  数:    out_set_tod_clk_state                                                     
 * 功  能:    设置输出1pps+tod信号的时钟源工作状态                                                             
 * 输  入:    state: 时钟源工作状态                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void out_set_tod_clk_state(u16_t state)
{   

     u8_t tmp = 0;
     
     tmp = (state & 0xFF00) >> 8;
     SPI_FPGA_ByteWrite(tmp, OUT_TOD_CLK_STATE_REG_ADDR0);  
     
     tmp = state & 0xFF;
     SPI_FPGA_ByteWrite(tmp, OUT_TOD_CLK_STATE_REG_ADDR1);
}


/*****************************************************************************
 * 函  数:    out_set_tod_monitor_alarm                                                     
 * 功  能:    设置输出1pps+tod信号的监控告警                                                             
 * 输  入:    alarm: 监控告警                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void out_set_tod_monitor_alarm(u16_t alarm)
{   

     u8_t tmp = 0;
     
     tmp = (alarm & 0xFF00) >> 8;
     SPI_FPGA_ByteWrite(tmp, OUT_TOD_MONITOR_ALARM_REG_ADDR0);  
     
     tmp = alarm & 0xFF;
     SPI_FPGA_ByteWrite(tmp, OUT_TOD_MONITOR_ALARM_REG_ADDR1);
}


/*****************************************************************************
 * 函  数:    out_set_pps_offset                                                     
 * 功  能:    设置输出1pps信号的延时补偿                                                             
 * 输  入:    offset: 延时补偿                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void out_set_pps_offset(u32_t offset)
{   
     u8_t tmp = 0;
     
     tmp = (offset & 0x0F000000) >> 24;
     SPI_FPGA_ByteRead(&tmp, OUT_PPS_OFFSET_REG_ADDR0);
     
     tmp = (offset & 0xFF0000) >> 16;
     SPI_FPGA_ByteRead(&tmp, OUT_PPS_OFFSET_REG_ADDR1); 
     
     
     tmp = (offset & 0xFF00) >> 8;
     SPI_FPGA_ByteRead(&tmp, OUT_PPS_OFFSET_REG_ADDR2);
     
     
     tmp = offset & 0xFF00;
     SPI_FPGA_ByteRead(&tmp, OUT_PPS_OFFSET_REG_ADDR3);     
        
}


