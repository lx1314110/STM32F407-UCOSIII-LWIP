/*****************************************************************************/
/* 文件名:    out_ptp.c                                                      */
/* 描  述:    PTP相关处理                                                    */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include <includes.h>
//#include "arch/cc.h"
//#include "lwip/api.h"
//#include "opt.h"
#include "Output/out_ptp.h"
#include "BSP/inc/bsp_spi.h"
#include "mtfs30.h"
#include "Debug/mtfs30_debug.h"
#include "Util/util_string.h"





/*-------------------------------*/
/* 全局变量声明                  */
/*-------------------------------*/


/*-------------------------------*/
/* 函数声明                      */
/*-------------------------------*/
static u8_t num_param_check(u8_t *p_param);
static u8_t value2_param_check(u8_t *p_param, u8_t *p_value);
static u8_t domain_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_secu_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_ps2_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_ps1_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_alt_master_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_freq_trac_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_time_trac_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_time_scale_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_cur_utc_offset_valid_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_leap59_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t flag_leap61_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t unimul_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t layer_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t step_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t delay_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t emsc_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t utc_offset_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t gm_prio1_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t gm_clk_qual_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t gm_prio2_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t steps_rm_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t time_source_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t announce_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t sync_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t ssm_parser(u8_t *p_param, ptp_t *p_ptp);
static u8_t set_cmd_param_parser(u8_t *p_msg, ptp_t *p_ptp);
/*-------------------------------*/
/* 变量定义                      */
/*-------------------------------*/
static ptp_parser_t  parser_tbl[30] = {
    domain_parser, 
    flag_secu_parser,
    flag_ps2_parser,
    flag_ps1_parser,
    flag_alt_master_parser,
    flag_freq_trac_parser,
    flag_time_trac_parser,
    flag_time_scale_parser,
    flag_cur_utc_offset_valid_parser,
    flag_leap59_parser,
    flag_leap61_parser,
    unimul_parser,
    layer_parser,
    step_parser,
    delay_parser,
    emsc_parser,
    utc_offset_parser,
    gm_prio1_parser,
    gm_clk_qual_parser,
    gm_prio2_parser,
    steps_rm_parser,
    time_source_parser,
    announce_parser,
    sync_parser,
    ssm_parser,

};


ptp_t    g_ptp;            /* 存放PTP配置信息 */


/*****************************************************************************
 * 函  数:    ptp_set_handler                                                         
 * 功  能:    PTP配置命令处理函数                                                                 
 * 输  入:    p_msg：配置信息                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t ptp_set_handler(u8_t *p_msg, u8_t *p_rtrv)
{

    ptp_t  ptp;
    
    
    /* 解析出命令中的每个参数 */
    if (set_cmd_param_parser(p_msg, &ptp))
    {
        return NG;
    }
      
    
    /* 配置PTP */
    if (NG == ptp_set(&ptp))
    {
        MTFS30_TIPS("PTP配置失败");    
        return NG;
    }
    
    return OK;
}


/*****************************************************************************
 * 函  数:    ptp_set                                                         
 * 功  能:    配置PTP                                                                 
 * 输  入:    p_ptp：指向PTP配置信息                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t ptp_set(ptp_t *p_ptp)
{
    u8_t ret = OK;
    u8_t tmp = 0;
    
    
    /* 参数检查 */
    if (NULL == p_ptp)
    {
        MTFS30_ERROR("set_handler()参数有误");
        return NG;
    }

    
    /* 配置DOMAIN */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->domain, PTP_DOMAIN_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置PTP security */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_secu, PTP_FLAG_SECU_REG_ADDR)))
    {
        goto PTP_SET_ERR; 
    }
    
    /* 配置PTP profile Specific 2 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_ps2, PTP_FLAG_PS2_REG_ADDR)))
    {
        goto PTP_SET_ERR;    
    }
    
    /* 配置PTP profile Specific 1 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_ps1, PTP_FLAG_PS1_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
   
    /* 配置alternateMasterFlag */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_alt_master, PTP_FLAG_ALT_MASTER_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }        

    /* 配置frequencyTraceable */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_freq_trac, PTP_FLAG_FREQ_TRAC_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置timeTraceable */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_time_trac, PTP_FLAG_TIME_TRAC_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置ptpTimescale */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_time_scale, PTP_FLAG_TIME_SCALE_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置currentUtcOffsetValid */   
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_cur_utc_offset_valid, PTP_FLAG_CUR_UTC_OFFSET_VALID_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }            
        
    /* 配置leap59 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_leap59, PTP_FLAG_LEAP59_REG_ADDR))) 
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置leap61 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->flag_leap61, PTP_FLAG_LEAP61_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置单播多播类型 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->unimul, PTP_UNIMUL_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置封装方式 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->layer, PTP_LAYER_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置时戳发送模式 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->step, PTP_STEP_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置时延类型 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->delay, PTP_DELAY_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置EMSC使能 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->emsc, PTP_ESMC_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置currentUtcOffset */
    tmp = (p_ptp->utc_offset & 0xFF00) >> 8;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->utc_offset, PTP_UTC_OFFSET_REG_ADDR0))) 
    {
        goto PTP_SET_ERR;
    }               
    tmp = p_ptp->utc_offset & 0xFF;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->utc_offset, PTP_UTC_OFFSET_REG_ADDR1)))
    {
        goto PTP_SET_ERR;
    }  
    
    
    /* 配置grandmasterPriority1 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->gm_prio1, PTP_GM_PRIO1_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置grandmasterClockQuality */
    tmp = (p_ptp->gm_clk_qual & 0xFF000000) >> 24;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->gm_clk_qual, PTP_GM_CLK_QUAL_REG_ADDR0)))   
    {
        goto PTP_SET_ERR;
    }               
    tmp = (p_ptp->gm_clk_qual & 0xFF0000) >> 16;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->gm_clk_qual, PTP_GM_CLK_QUAL_REG_ADDR1))) 
    {
        goto PTP_SET_ERR;
    }               
    tmp = (p_ptp->gm_clk_qual & 0xFF00) >> 8;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->gm_clk_qual, PTP_GM_CLK_QUAL_REG_ADDR2)))
    {
        goto PTP_SET_ERR;
    }               
    tmp = p_ptp->gm_clk_qual & 0xFF;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->gm_clk_qual, PTP_GM_CLK_QUAL_REG_ADDR3)))      
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置grandmasterPriority2 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->gm_prio2, PTP_GM_PRIO2_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置stepsRemoved */
    tmp = (p_ptp->steps_rm & 0xFF00) >> 8;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->steps_rm, PTP_STEPS_RM_REG_ADDR0)))  
    {
        goto PTP_SET_ERR;
    }               
    tmp = p_ptp->steps_rm & 0xFF;
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->steps_rm, PTP_STEPS_RM_REG_ADDR1))) 
    {
        goto PTP_SET_ERR;
    }               


    /* 配置timeSource */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->time_source, PTP_TIME_SOURCE_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置announce报文发包间隔 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->announce, PTP_ANNOUNCE_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置sync报文发包间隔 */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->sync, PTP_SYNC_REG_ADDR)))
    {
        goto PTP_SET_ERR;
    }
    
    /* 配置ssm */
    if (NG == (ret = SPI_FPGA_ByteWrite(p_ptp->ssm, PTP_SSM_REG_ADDR)))    
    {
        goto PTP_SET_ERR;
    }


PTP_SET_ERR:
    
    if (NG == ret)
    {
        MTFS30_ERROR("通过SPI配置PTP失败");
        return NG;
    }
        
    return OK;
}

/*****************************************************************************
 * 函  数:    domain_parser                                                         
 * 功  能:    解析DOMAIN                                                                 
 * 输  入:    p_param：配置的domian信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                  
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t domain_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;
    
    
    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("DOMIAN设定值(%s)不正确,正确值为0~255", p_param);
        return NG;
    }
    
    p_ptp->domain = value;     
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_secu_parser                                                         
 * 功  能:    解析PTP security                                                                
 * 输  入:    p_param：配置的PTP security信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_secu_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("PTP security设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_secu = value&0x1; 
    return OK;
}


/*****************************************************************************
 * 函  数:    flag_ps2_parser                                                         
 * 功  能:    解析PTP profile Specific 2                                                                
 * 输  入:    p_param：配置的PTP profile Specific 2 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_ps2_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("PTP profile Specific 2 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_ps2 = value&0x1;    
    return OK;
}


/*****************************************************************************
 * 函  数:    flag_ps1_parser                                                         
 * 功  能:    解析PTP profile Specific 1                                                                
 * 输  入:    p_param：配置的PTP profile Specific 1 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_ps1_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("PTP profile Specific 1 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_ps1 = value&0x1;        
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_alt_master_parser                                                         
 * 功  能:    解析alternateMasterFlag                                                                
 * 输  入:    p_param：配置的alternateMasterFlag 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_alt_master_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("alternateMasterFlag 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_alt_master = value&0x1;     
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_freq_trac_parser                                                         
 * 功  能:    解析frequencyTraceable                                                          
 * 输  入:    p_param：配置的frequencyTraceable 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_freq_trac_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("frequencyTraceable 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_freq_trac = value&0x1;       
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_time_trac_parser                                                         
 * 功  能:    解析timeTraceable                                                          
 * 输  入:    p_param：配置的timeTraceable 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_time_trac_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("timeTraceable 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_time_trac = value&0x1;      
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_time_scale_parser                                                         
 * 功  能:    解析ptpTimescale                                                          
 * 输  入:    p_param：配置的ptpTimescale 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_time_scale_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("ptpTimescale 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_time_scale = value&0x1;   
    return OK;
}


/*****************************************************************************
 * 函  数:    flag_cur_utc_offset_valid_parser                                                         
 * 功  能:    解析currentUtcOffsetValid                                                         
 * 输  入:    p_param：配置的currentUtcOffsetValid 信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_cur_utc_offset_valid_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;

    
    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("currentUtcOffsetValid 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->flag_cur_utc_offset_valid = value&0x1;   
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_leap59_parser                                                         
 * 功  能:    解析leap59                                                        
 * 输  入:    p_param：配置的leap59信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_leap59_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("leap59 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
	
    p_ptp->flag_leap59 = value&0x1;    
    return OK;
}

/*****************************************************************************
 * 函  数:    flag_leap61_parser                                                         
 * 功  能:    解析leap61                                                        
 * 输  入:    p_param：配置的leap61信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t flag_leap61_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("leap61 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
	
    p_ptp->flag_leap61 = value&0x1;  
    return OK;
}

/*****************************************************************************
 * 函  数:    unimul_parser                                                         
 * 功  能:    解析单播多播类型配置信息                                                        
 * 输  入:    p_param：配置的播多播类型信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t unimul_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("单播多播 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
	
    p_ptp->unimul = value&0x1;  
    return OK;
}

/*****************************************************************************
 * 函  数:    layer_parser                                                         
 * 功  能:    解析封装方式配置信息                                                        
 * 输  入:    p_param：配置的封装方式信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t layer_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("封装方式(二层/三层) 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
	
    p_ptp->layer = value&0x1;  
    return OK;
}

/*****************************************************************************
 * 函  数:    step_parser                                                         
 * 功  能:    解析时戳发送模式配置信息                                                        
 * 输  入:    p_param：配置的时戳发送模式信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t step_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("时戳发送模式(onestep/twostep) 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    p_ptp->step = value&0x1;    
    return OK;
}

/*****************************************************************************
 * 函  数:    delay_parser                                                         
 * 功  能:    解析时延类型配置信息                                                        
 * 输  入:    p_param：配置的时延类型信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t delay_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("时延类型(P2P/E2E) 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    p_ptp->delay = value&0x1;  
    return OK;
}

/*****************************************************************************
 * 函  数:    emsc_parser                                                         
 * 功  能:    解析EMSC使能配置信息                                                        
 * 输  入:    p_param：配置的EMSC使能信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t emsc_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;


    if (NG == value2_param_check(p_param, &value))
    {
        MTFS30_TIPS("EMSC使能 设定值(%s)不正确,正确值为0或者1", p_param);
        return NG;
    }
    
    p_ptp->emsc = value&0x1;                    
    return OK;
}

/*****************************************************************************
 * 函  数:    utc_offset_parser                                                         
 * 功  能:    解析currentUtcOffset配置信息                                                        
 * 输  入:    p_param：配置的currentUtcOffset信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t utc_offset_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u16_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("currentUtcOffset 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u16_t)atoi((char const *)p_param);
    
    p_ptp->utc_offset = value;                    
    return OK;
}


/*****************************************************************************
 * 函  数:    gm_prio1_parser                                                         
 * 功  能:    解析grandmasterPriority1配置信息                                                        
 * 输  入:    p_param：配置的grandmasterPriority1信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t gm_prio1_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("grandmasterPriority1 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u8_t)atoi((char const *)p_param);
    
    p_ptp->gm_prio1 = value;                    
    return OK;
}

/*****************************************************************************
 * 函  数:    gm_clk_qual_parser                                                         
 * 功  能:    解析grandmasterClockQuality配置信息                                                        
 * 输  入:    p_param：配置的grandmasterClockQuality信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t gm_clk_qual_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u32_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("grandmasterClockQuality 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u32_t)atoi((char const *)p_param);
    
    p_ptp->gm_clk_qual = value;                    
    return OK;
}


/*****************************************************************************
 * 函  数:    gm_prio2_parser                                                         
 * 功  能:    解析grandmasterPriority2配置信息                                                        
 * 输  入:    p_param：配置的grandmasterPriority2信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t gm_prio2_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("grandmasterPriority2 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u8_t)atoi((char const *)p_param);
    
    p_ptp->gm_prio2 = value;                    
    return OK;
}


/*****************************************************************************
 * 函  数:    steps_rm_parser                                                         
 * 功  能:    解析stepsRemoved配置信息                                                        
 * 输  入:    p_param：配置的stepsRemoved信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t steps_rm_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u16_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("stepsRemoved 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u16_t)atoi((char const *)p_param);
    
    p_ptp->steps_rm = value;                    
    return OK;
}


/*****************************************************************************
 * 函  数:    time_source_parser                                                         
 * 功  能:    解析timeSource配置信息                                                        
 * 输  入:    p_param：配置的timeSource信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t time_source_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("timeSource 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u8_t)atoi((char const *)p_param);
    
    p_ptp->time_source = value;                    
    return OK;
}


/*****************************************************************************
 * 函  数:    announce_parser                                                         
 * 功  能:    解析announce报文发包间隔配置信息                                                        
 * 输  入:    p_param：配置的announce报文发包间隔信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t announce_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("announce报文发包间隔 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u8_t)atoi((char const *)p_param);
    
    p_ptp->announce = value;                    
    return OK;
}


/*****************************************************************************
 * 函  数:    sync_parser                                                         
 * 功  能:    解析sync报文发包间隔配置信息                                                        
 * 输  入:    p_param：配置的sync报文发包间隔信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t sync_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("sync报文发包间隔 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u8_t)atoi((char const *)p_param);
    
    p_ptp->sync = value;                    
    return OK;
}

/*****************************************************************************
 * 函  数:    ssm_parser                                                         
 * 功  能:    解析ssm配置信息                                                        
 * 输  入:    p_param：配置的ssm信息  
 * 输  出:    p_ptp: 存放解析后的PTP配置信息                                                 
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t ssm_parser(u8_t *p_param, ptp_t *p_ptp)
{
    u8_t value = 0;



    if (NG == num_param_check(p_param))
    {
        MTFS30_TIPS("ssm 设定值(%s)不正确,请输入整数值", p_param);
        return NG;
    }
    
    value = (u8_t)atoi((char const *)p_param);
    
    p_ptp->ssm = value;                    
    return OK;
}

/*****************************************************************************
 * 函  数:    set_cmd_param_parser                                                         
 * 功  能:    PTP配置命令参数解析                                                                 
 * 输  入:    p_msg：配置命令信息    
 *            p_ptp: 指向解析后的参数
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t set_cmd_param_parser(u8_t *p_msg, ptp_t *p_ptp)
{
    u8_t cnt = 0;
    u8_t *p = p_msg;
    u8_t *p_param = NULL;

    
    
    /* 解析出每个参数 */
    while(*p)
    {
        /* 取出每个参数 */
        if (NULL != (p_param = (u8_t *)strtok((char *)p, ",")))
        {
            
            
            /* 调用每个参数对应的解析函数 */
            if (NG == parser_tbl[cnt].cmd_fun(p_param, p_ptp))
            {
                
                return NG; 
            }
            
            p += strlen((const char *)p_param);
            cnt++;
        
        }
        else
        {
            MTFS30_TIPS("PTP配置命令中含有空参数，请重新输入");
            return NG;            
        }
                                  
        p++;
    }
    
    return OK;

}






/*****************************************************************************
 * 函  数:    value2_param_check                                                        
 * 功  能:    2值(0/1)参数值检查                                                                 
 * 输  入:    p_param: 指向设定值 
 *            p_value: 参数值
 * 输  出:    无                                                    
 * 返回值:    OK: 参数值正确； NG：参数值错误                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t value2_param_check(u8_t *p_param, u8_t *p_value)
{
    u8_t value = 0;
    
    *p_value = 0;
    
    value = *(u8_t *)p_param - '0';
    if (value != 0 && value != 1)
    {       
        return NG;
    }

    *p_value = value;
    return OK;
} 

/*****************************************************************************
 * 函  数:    num_param_check                                                         
 * 功  能:    数字参数值检查                                                               
 * 输  入:    p_param: 指向设定值                          
 * 输  出:    无                                                 
 * 返回值:    OK: 参数正确; NG:参数不正确                                                      
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
static u8_t num_param_check(u8_t *p_param)
{
    u8_t i = 0;
    
    
    /* 检查是否含有0~9之外的其他字符 */
    while(p_param[i])
    {
        if (p_param[i] < '0' || p_param[i] > '9')
        {
            if (i == 0 && p_param[i] == '-') /* 负号 */
            {
                continue;
            }
            
            return NG;
        }
    
        i++;
    }
    
    
    return OK;
}







