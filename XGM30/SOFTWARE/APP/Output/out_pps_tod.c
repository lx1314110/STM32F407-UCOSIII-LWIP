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
#include "Debug/user_cmd.h"
#include "Debug/mtfs30_debug.h"
#include "Config/config.h"
#include "opt.h"
#include "out_pps_tod.h"
#include "bsp_spi.h"
#include "mtfs30.h"

#define  DLY_COMP_UNIT 8
typedef struct  _PPS_PARSER_T_
{
    u8_t  (*cmd_fun)(u8_t *p_param);
} pps_query_parse;

typedef struct  _PPS_PARSER_SET_
{
    u8_t  (*cmd_fun)(u8_t *p_param, tConfigParameters *p_sParameters);
} pps_set_parse;

const char *pTimeZone[TIME_ZONE_NUM] = {"utc+0", "utc+1", "utc+2", "utc+3", "utc+4", "utc+5", "utc+6", "utc+7",
                                        "utc+8", "utc+9","utc+10","utc+11","utc+12","utc-11","utc-10", "utc-9",
                                        "utc-8", "utc-7", "utc-6", "utc-5", "utc-4", "utc-3", "utc-2", "utc-1" 
                                       };
const char *pSignalType[SIGNAL_TYPE_NUM] = {"gnss-pps", "sys-pps", "dc-irigb", "2mhz", "10mhz"};

const char *pPrecuationValue[PRECAUTION_STR_NUM] = {"00", "01", "10", "11"};

const char *pps_help = {
"Control and monitor the 1PPS+TOD or 1PPS or IRIGB module\n"\
"Usage: <command>\r\n"\
" out (b)\r\n"\
"         (c)compensation <value>:change IRIGB compensation.\r\n" \
"         (l)leap <value>:change IRIGB leap precaution, 00 ~ 11.\r\n" \
"         (d)daylingt <value>:change IRIGB daylight precaution, 00 ~ 11.\r\n" \
"         (z)zone <value>:change IRIGB time zone,utc+0 ~ utc+12 and utc-11 ~ utc-1.\r\n" \
"         (q)quality <value>:change IRIGB quality zone.\r\n" \
" out (p)\r\n"\
"         (c)compensation <value>:change 1pps+tod compensation. \r\n"\
"         (t)type <value>: Change tod info contain status message.\r\n"\
"         (s)status <value>: Change pps status of tod message.\r\n"\
"         (j)Jitter <value>: Change jitter magnitude of tod message. \r\n"\
"         (k)clock <value>: Change clock type of tod message.\r\n"\
"         (l)lock <value>: Change lock mode of tod message.\r\n"\
"         (a)alarm <value>: Change clock source alarm.\r\n"\
" out (s)\r\n"\
"         (t)type <value>:change out signal type, gnss-pps|sys-pps|dc-irigb|2mhz|10mhz. \r\n"
};

const char * sub_irigb_command[]  = {"c", "l", "d", "z", "q"};
const char * sub_pps_command[]  = {"c", "t", "s", "j", "k", "l", "a"};
u8_t  sub_irigb_num = sizeof(sub_irigb_command)/sizeof(const char *);
u8_t  pps_subcmd_num = sizeof(sub_pps_command)/sizeof(const char *);

static u8_t out_set_irigb_dlycom(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_leapPreflag(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_daylight(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_timeskew(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_irigb_clockquality(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_ppstod_dlycom(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_tod_type(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_pps_status(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_tod_jitter(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_clock_type(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_clock_status(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_monitor_alarm(u8_t *p_param, tConfigParameters *p_sParameters);
static u8_t out_set_singal_type(u8_t *p_param, tConfigParameters *p_sParameters);

static u8_t out_irigb_dlycom_rtrv(u8_t *p_param);
static u8_t out_irigb_leapPreflag_rtrv(u8_t *p_param);
static u8_t out_irigb_daylight_rtrv(u8_t *p_param);
static u8_t out_irigb_timeskew_rtrv(u8_t *p_param);
static u8_t out_irigb_clockquality_rtrv(u8_t *p_param);
static u8_t out_ppstod_dlycom_rtrv(u8_t *p_param);
static u8_t out_pps_status_rtrv(u8_t *p_param);
static u8_t out_tod_type_rtrv(u8_t *p_param);
static u8_t out_tod_jitter_rtrv(u8_t *p_param);
static u8_t out_clock_type_rtrv(u8_t *p_param);
static u8_t out_clock_status_rtrv(u8_t *p_param);
static u8_t out_monitor_alarm_rtrv(u8_t *p_param);
static u8_t out_singal_type_rtrv(u8_t *p_param);

pps_query_parse irigb_query_parse[] ={  out_irigb_dlycom_rtrv,
                                        out_irigb_leapPreflag_rtrv,
                                        out_irigb_daylight_rtrv,
                                        out_irigb_timeskew_rtrv,
                                        out_irigb_clockquality_rtrv
                                      };

pps_query_parse ppstod_query_parse[] ={ out_ppstod_dlycom_rtrv,
                                        out_tod_type_rtrv,
                                        out_pps_status_rtrv,
                                        out_tod_jitter_rtrv,
                                        out_clock_type_rtrv,
                                        out_clock_status_rtrv,
                                        out_monitor_alarm_rtrv};

pps_set_parse irigb_set_parse[] ={      out_set_irigb_dlycom,
                                        out_set_irigb_leapPreflag,
                                        out_set_irigb_daylight,
                                        out_set_irigb_timeskew,
                                        out_set_irigb_clockquality};

pps_set_parse ppstod_set_parse[] ={     out_set_ppstod_dlycom,
                                        out_set_tod_type,
                                        out_set_pps_status,
                                        out_set_tod_jitter,
                                        out_set_clock_type,
                                        out_set_clock_status,
                                        out_set_monitor_alarm
                                  };
/*****************************************************************************
 * fuction:       out_pps_handler                                                        
 * description:   out pps,1pps+tod,irigb handler                                                                 
 * input:         int num,...                    
 * ouput:         null                                                    
 * return:        null                                                                                          
 ****************************************************************************/
u8_t out_pps_handler(int num,...)
{
    u8_t *p_param[MAX_PARAM_NUM] = NULL;
    u8_t  err_flag = OK;
    u8_t  inx = 0;
    u8_t  indx = 0;
    
    
    /*uncertain parameter list*/
    va_list valist;
    
    va_start(valist, num);
    for(inx = 0; inx < num; inx++)
    {
        p_param[inx] = (u8_t *)va_arg(valist, char *);
        /*point parameter as null return NG*/
        if(p_param[inx] == NULL)
        err_flag = NG;
    }
    va_end(valist);
    
    if(err_flag == OK)
    {
      
       /*ptp help ,echo string*/
       if(0== strncmp((char const*)p_param[0], "help", strlen("help")))
       {
          DEBUG_USART_Send((u8_t *)pps_help, strlen(pps_help));
          goto return_mark; 
       }
       
       switch(*p_param[0])
       {
           case 'b':
             {
             
                  for(indx = 0; indx < sub_irigb_num; indx++)
                  {
                      if(0 == strncmp((const char*)p_param[1], sub_irigb_command[indx], strlen(sub_irigb_command[indx])))
                        break;
                  }
                  if(indx == sub_irigb_num)
                  {
                      err_flag = NG;
                       goto return_mark;
                  }
                  if(RTRV_PARMETERS_NUM == num)
                        err_flag = irigb_query_parse[indx].cmd_fun(p_param[num -1]); 
                  else if(SET_PARMETERS_NUM == num)
                        err_flag = irigb_set_parse[indx].cmd_fun(p_param[num - 2], &g_sParameters);
                  else
                  {
                     err_flag = NG;
                       goto return_mark;
                  }
             }
             break;
             
             case 'p':
             {
             
                  for(indx = 0; indx < pps_subcmd_num; indx++)
                  {
                      if(0 == strncmp((const char*)p_param[1], sub_pps_command[indx], strlen(sub_pps_command[indx])))
                        break;
                  }
                  if(indx == pps_subcmd_num)
                  {
                      err_flag = NG;
                       goto return_mark;
                  }
                  if(RTRV_PARMETERS_NUM == num)
                        err_flag = ppstod_query_parse[indx].cmd_fun(p_param[num -1]); 
                  else if(SET_PARMETERS_NUM == num)
                        err_flag = ppstod_set_parse[indx].cmd_fun(p_param[num - 2], &g_sParameters);
                  else
                     err_flag = NG;
             }
             break;
             
              case 's':
             {
             
                  if(RTRV_PARMETERS_NUM == num)
                        err_flag = out_singal_type_rtrv(p_param[num -1]); 
                  else if(SET_PARMETERS_NUM == num)
                        err_flag = out_set_singal_type(p_param[num - 2], &g_sParameters);
                  else
                     err_flag = NG;
             }
             break;
             
             default:
                {
                  err_flag = NG;
                }
             break;
       }
       
    }
    
return_mark:   
    return err_flag;
}

/*****************************************************************************
 * fuction:       out_set_irigb_dlycom                                                        
 * description:   change the 1pps tod delay compensation                                                                 
 * input:         p_param,p_sParameters offset-delay compensation and save struct                    
 * ouput:         null                                                    
 * return:        null                                                                                          
 ****************************************************************************/
static u8_t out_set_irigb_dlycom(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
     int offset = 0;
     int value = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     offset = atoi((const char *)p_param);
     
     //save the delay compensation.
     p_sParameters->outPPXIRIGBParameters.delay_irigb_com = offset;
     
     //27 bit mark Positive and negative.
     value = abs(offset) / DLY_COMP_UNIT;
     
     if(offset < 0)
       value |= 0x08000000;
     
     
     tmp = (value & 0x0F000000) >> 24;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_OFFSET_REG_ADDR0))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR0, tmp);
          return NG;
     }
     
     tmp = (value & 0xFF0000) >> 16;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_OFFSET_REG_ADDR1))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR1, tmp);
          return NG;
     } 
     
     
     tmp = (value & 0xFF00) >> 8;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_OFFSET_REG_ADDR2))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR2, tmp);
          return NG;
     } 
     
     
     tmp = value & 0xFF;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_OFFSET_REG_ADDR3))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR3, tmp);
          return NG;
     }
     
     return OK;    
}

/*****************************************************************************
 * fuction:     out_set_irigb_leapPreflag                                                        
 * description: change the 1pps tod leap precaution                                                                 
 * input:    p_param,p_sParameters leap precaution and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_irigb_leapPreflag(u8_t *p_param, tConfigParameters *p_sParameters)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "00", strlen("00")) == 0 )
           p_sParameters->outPPXIRIGBParameters.flag_leappre = tmp = 0;
      
      else if(strncmp((char const*)p_param, "01", strlen("01")) == 0 )      
           p_sParameters->outPPXIRIGBParameters.flag_leappre = tmp = 1;
      
      else if(strncmp((char const*)p_param, "10", strlen("10")) == 0 )      
           p_sParameters->outPPXIRIGBParameters.flag_leappre = tmp = 2;
      
      else if(strncmp((char const*)p_param, "11", strlen("11")) == 0 )      
           p_sParameters->outPPXIRIGBParameters.flag_leappre = tmp = 3;
      else
      {
          MTFS30_ERROR("irigb set leap precaution value:%s!", p_param);
          return NG; 
      }
      /*write reg*/
      if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_LEAPPRE_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_LEAPPRE_REG_ADDR, tmp);
          return NG;
      }
       
      return OK; 
}

/*****************************************************************************
 * fuction:    out_set_irigb_daylight                                                        
 * description:  p_param,p_sParameters daylight precaution and save struct                                                                  
 * input:    p_param,p_sParameters leap precaution and save struct                     
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_irigb_daylight(u8_t *p_param, tConfigParameters *p_sParameters)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if(strncmp((char const*)p_param, "00", strlen("00")) == 0 )
           p_sParameters->outPPXIRIGBParameters.daylight_pre = tmp = 0;
      
      else if(strncmp((char const*)p_param, "01", strlen("01")) == 0 )      
           p_sParameters->outPPXIRIGBParameters.daylight_pre = tmp = 1;
      
      else if(strncmp((char const*)p_param, "10", strlen("10")) == 0 )      
           p_sParameters->outPPXIRIGBParameters.daylight_pre = tmp = 2;
      
      else if(strncmp((char const*)p_param, "11", strlen("11")) == 0 )      
           p_sParameters->outPPXIRIGBParameters.daylight_pre = tmp = 3;
      else
      {
          MTFS30_ERROR("irigb set daylight precaution value:%s!", p_param);
          return NG; 
      }
      /*write reg*/
      if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_DAYLIGHT_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_DAYLIGHT_REG_ADDR, tmp);
          return NG;
      }
       
      return OK; 
}

/*****************************************************************************
 * fuction:    out_set_irigb_timeskew                                                        
 * description:    change the irigb time skew precaution.                                                                  
 * input:    p_param,p_sParameters time skew and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_irigb_timeskew(u8_t *p_param, tConfigParameters *p_sParameters)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      u8_t indx= 0;
      u8_t time_zone = 0xff;
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      for(indx = 0; indx < TIME_ZONE_NUM; indx++)
      {
          if(strncmp((char const*)p_param, pTimeZone[indx], strlen(pTimeZone[indx])) == 0 )
          {
            time_zone = indx;
            break;
          }
      }
       
      if(time_zone >= TIME_ZONE_NUM)
      {
          MTFS30_ERROR("no such name");
          return NG;
      }
      else if(time_zone >= WEST_1_ZONE )
      {
          tmp = (TIME_ZONE_NUM - time_zone) | 0x10;
          p_sParameters->outPPXIRIGBParameters.time_zone = tmp;
      }
      else
      {
          tmp = time_zone;
          p_sParameters->outPPXIRIGBParameters.time_zone = tmp;
      }
   
      /*write reg*/
      if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_TIMESKEW_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_TIMESKEW_REG_ADDR, tmp);
          return NG;
      }
       
      return OK; 
}

/*****************************************************************************
 * fuction:    out_set_irigb_clockquality                                                        
 * description:    change the irigb clock quality.                                                                  
 * input:    p_param,p_sParameters irigb quality and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_irigb_clockquality(u8_t *p_param, tConfigParameters *p_sParameters)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      tmp = atoi((char const *)p_param) & 0x0f;
      p_sParameters->outPPXIRIGBParameters.clock_quality = tmp; 
      
      /*write reg*/
      if (NG == SPI_FPGA_ByteWrite(tmp, OUT_B_TIMEQUALITY_REG_ADDR))
      {
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_B_TIMEQUALITY_REG_ADDR, tmp);
          return NG;
      }
       
      return OK; 
}
/*****************************************************************************
 * fuction:    out_set_ppstod_dlycom                                                        
 * description:    change the pps tod delay compensation.                                                                  
 * input:    p_param,p_sParameters pps tod delay compensation and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_ppstod_dlycom(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
     int offset = 0;
     int value = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     offset = atoi((const char *)p_param);
     p_sParameters->outPPXIRIGBParameters.delay_pps_tod_com = offset;
     
     value = abs(offset) / DLY_COMP_UNIT;
     
     if(offset < 0)
       value |= 0x08000000;
     tmp = (value & 0x0F000000) >> 24;
  
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_OFFSET_REG_ADDR0))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR0, tmp);
          return NG;
     }
     
     tmp = (value & 0xFF0000) >> 16;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_OFFSET_REG_ADDR1))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR1, tmp);
          return NG;
     } 
     
     tmp = (value & 0xFF00) >> 8;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_OFFSET_REG_ADDR2))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR2, tmp);
          return NG;
     } 
     
     
     tmp = value & 0xFF;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_OFFSET_REG_ADDR3))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR3, tmp);
          return NG;
     }
     
     return OK;
        
}

/*****************************************************************************
 * fuction:     out_set_tod_type                                                        
 * description: change the tod type.                                                                  
 * input:    p_param,p_sParameters tod type and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_tod_type(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
     
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     if(strncmp((char const*)p_param, "0", strlen("0")) == 0)
        p_sParameters->outPPXIRIGBParameters.tod_clock_type = tmp = 0;
     
     else if(strncmp((char const*)p_param, "1", strlen("1")) == 0)
        p_sParameters->outPPXIRIGBParameters.tod_clock_type = tmp = 1;
     
     else
     {
        MTFS30_ERROR("no such name!");
        return NG;
     }
     
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_TYPE_REG_ADDR))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_TYPE_REG_ADDR, tmp);
          return NG;
     }
     
     return OK;    
}

/*****************************************************************************
 * fuction:     out_set_pps_status                                                        
 * description: change the pps status.                                                                  
 * input:    p_param,p_sParameters pps status and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_pps_status(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
     
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     tmp = atoi((const char*) p_param);
      
     p_sParameters->outPPXIRIGBParameters.pps_status = tmp;
    
     
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_PPS_STATE_REG_ADDR))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_PPS_STATE_REG_ADDR, tmp);
          return NG;
     }
     
     return OK;       
}

/*****************************************************************************
 * fuction:     out_set_tod_jitter                                                        
 * description: change the pps status.                                                                  
 * input:    p_param,p_sParameters tod jitter and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_tod_jitter(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     //
     //save tod_jitter.
     tmp =(u8_t)atoi((const char*) p_param);
     p_sParameters->outPPXIRIGBParameters.tod_jitter = tmp;
     
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_TACC_REG_ADDR))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_TACC_REG_ADDR, tmp);
          return NG;
     }
     
     return OK;       
}


/*****************************************************************************
 * fuction:     out_set_clock_type                                                        
 * description: change the pps clock type.                                                                  
 * input:    p_param,p_sParameters clock type and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_clock_type(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     tmp =(u8_t)atoi((const char*) p_param);
     tmp &= 0xc0;
     p_sParameters->outPPXIRIGBParameters.tod_clock_type = tmp;
     
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_CLK_TYPE_REG_ADDR))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_CLK_TYPE_REG_ADDR, tmp);
          return NG;
     }
     
     return OK;       
}


/*****************************************************************************
 * fuction:     out_set_clock_status                                                        
 * description: change the pps clock status.                                                                  
 * input:    p_param,p_sParameters clock status and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_clock_status(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u16_t temp = 0;
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     temp =(u16_t)atoi((const char*) p_param);
     p_sParameters->outPPXIRIGBParameters.tod_clock_status = tmp;
       
     tmp = (temp >> 8) & 0xff;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_CLK_STATE_REG_ADDR0))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_CLK_STATE_REG_ADDR0, tmp);
          return NG;
     }
     tmp = temp& 0xff;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_CLK_STATE_REG_ADDR1))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_CLK_STATE_REG_ADDR1, tmp);
          return NG;
     }
     return OK;       
}

/*****************************************************************************
 * fuction:     out_set_monitor_alarm                                                        
 * description: change monitor alarm.                                                                  
 * input:    p_param,p_sParameters monitor alarm and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_monitor_alarm(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u16_t temp = 0;
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     temp =(u16_t)atoi((const char*) p_param);
     p_sParameters->outPPXIRIGBParameters.tod_moni_alarm = tmp;
     tmp = (temp >> 8) & 0xff;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_MONITOR_ALARM_REG_ADDR0))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_MONITOR_ALARM_REG_ADDR0, tmp);
          return NG;
     }
     tmp = temp& 0xff;
     if (NG == SPI_FPGA_ByteWrite(tmp, OUT_TOD_MONITOR_ALARM_REG_ADDR1))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_TOD_MONITOR_ALARM_REG_ADDR1, tmp);
          return NG;
     }
     return OK;       
}

/*****************************************************************************
 * fuction:     out_set_signal_type                                                       
 * description: change output signal type.                                                                  
 * input:    p_param,p_sParameters signal type and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_set_singal_type(u8_t *p_param, tConfigParameters *p_sParameters)
{   
     u8_t tmp = 0;
     u8_t indx = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     for(indx = 0; indx < SIGNAL_TYPE_NUM; indx++)
     {
        if(0 == strncmp((const char*)p_param, pSignalType[indx],strlen(pSignalType[indx])))
          break;
     }
     
     if(indx == SIGNAL_TYPE_NUM)
     {
        MTFS30_ERROR("no such name");
        return NG;
     }
     p_sParameters->outPPXIRIGBParameters.signal_type = indx;
     
     if (NG == SPI_FPGA_ByteWrite(indx, OUT_SIGNAL_TYPE_REG_ADDR))
     { 
          MTFS30_ERROR("spi write_address:%#x,value:%#x", OUT_SIGNAL_TYPE_REG_ADDR, tmp);
          return NG;
     }
    
     return OK;       
}

/*******************************************************************************
*fuction: u8_t  out_irigb_dlycom_rtrv(u8_t *p_param)
*descrip: inquery the vid.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t  out_irigb_dlycom_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      u8_t  ret_sign = 0;
      int ret_value = 0;
     
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_OFFSET_REG_ADDR0))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR0, tmp);
              return NG;
      }
      ret_sign = (tmp & 0x08)? 1 : 0;
      ret_value |= tmp & 0x07;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_OFFSET_REG_ADDR1))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR1, tmp);
              return NG;
      }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_OFFSET_REG_ADDR2))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR2, tmp);
              return NG;
      }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
       if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_OFFSET_REG_ADDR3))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_OFFSET_REG_ADDR3, tmp);
              return NG;
      }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
      ret_value = ret_sign? (-ret_value * 8):(ret_value * 8);
      g_sParameters.outPPXIRIGBParameters.delay_irigb_com = ret_value;
      
      sprintf((char *)p_param, "%d\r\n",ret_value);
      return OK;
}

/*******************************************************************************
*fuction: u8_t  out_irigb_leapPreflag_rtrv(u8_t *p_param)
*descrip: inquery the vid.
*paramet: p_param( backpass point)
*return:  NG|OK
********************************************************************************/
static u8_t out_irigb_leapPreflag_rtrv(u8_t *p_param)
{
     /*tempority point variable*/
      u8_t  tmp = 0;
      
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
     
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_LEAPPRE_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_LEAPPRE_REG_ADDR, tmp);
              return NG;
      }
      if(tmp >= PRECAUTION_STR_NUM) 
      {
          MTFS30_ERROR("no such name");
          return NG; 
      }
      g_sParameters.outPPXIRIGBParameters.flag_leappre = tmp & 0x3;
      
      sprintf((char *)p_param, "%s\r\n", pPrecuationValue[tmp]);
      return OK;
}


/*****************************************************************************
 * fuction:    out_irigb_daylight_rtrv                                                        
 * description:  p_param,p_sParameters daylight precaution and save struct                                                                  
 * input:    p_param,p_sParameters daylight precaution and save struct                     
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_irigb_daylight_rtrv(u8_t *p_param)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_DAYLIGHT_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_DAYLIGHT_REG_ADDR, tmp);
              return NG;
      }
      
      
      if(tmp >= PRECAUTION_STR_NUM) 
      {
          MTFS30_ERROR("no such name");
          return NG; 
      }
      
      g_sParameters.outPPXIRIGBParameters.daylight_pre = tmp & 0x3;
      
      sprintf((char *)p_param, "%s\r\n", pPrecuationValue[tmp]);
       
      return OK; 
}

/*****************************************************************************
 * fuction:    out_irigb_timeskew_rtrv                                                        
 * description:    change the irigb time skew precaution.                                                                  
 * input:    p_param,p_sParameters time skew and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_irigb_timeskew_rtrv(u8_t *p_param)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      u8_t indx= 0;
      u8_t west_flag = 0;
      u8_t time_zone = 0xff;
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_TIMESKEW_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_TIMESKEW_REG_ADDR, tmp);
              return NG;
      }

      if(tmp & 0x10 ) 
      {
          west_flag = 1;
          indx = TIME_ZONE_NUM - (tmp & 0x0f) ;
      }
      else
          indx = tmp & 0x0f;
      if(indx >= TIME_ZONE_NUM)
      {
           MTFS30_ERROR("no such name");
           return NG;
      }
      g_sParameters.outPPXIRIGBParameters.time_zone = tmp;
      sprintf((char *)p_param, "%s\r\n", pTimeZone[indx]);
      
      return OK; 
}

/*****************************************************************************
 * fuction:    out_set_irigb_clockquality_rtrv                                                        
 * description:    change the irigb clock quality.                                                                  
 * input:    p_param,p_sParameters irigb quality and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                          
 ****************************************************************************/
static u8_t out_irigb_clockquality_rtrv(u8_t *p_param)
{   
      /*tempority point variable*/
      u8_t tmp = 0;
      
     /*p_param point null,error*/ 
      if(p_param == NULL)
      {
          MTFS30_ERROR("p_param is null!");
          return NG;
      } 
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_B_TIMEQUALITY_REG_ADDR))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_B_TIMEQUALITY_REG_ADDR, tmp);
              return NG;
      }
      
      g_sParameters.outPPXIRIGBParameters.clock_quality = tmp & 0x0f; 
      
      sprintf((char *)p_param, "%d\r\n", tmp & 0x0f);
       
      return OK; 
}

/*****************************************************************************
 * fuction:    out_ppstod_dlycom_rtrv                                                        
 * description:    change the pps tod delay compensation.                                                                  
 * input:    p_param,p_sParameters pps tod delay compensation and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_ppstod_dlycom_rtrv(u8_t *p_param)
{   
     u8_t tmp = 0;
     u8_t ret_sign = 0;
     int ret_value = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR0))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR0, tmp);
              return NG;
      }
      ret_sign = (tmp & 0x08)? 1 : 0;
      ret_value |= tmp & 0x07;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR1))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR1, tmp);
              return NG;
      }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
      if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR2))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR2, tmp);
              return NG;
      }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
       if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_OFFSET_REG_ADDR3))
      {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_OFFSET_REG_ADDR3, tmp);
              return NG;
      }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
      ret_value = ret_sign? (-ret_value * 8):(ret_value * 8);
      g_sParameters.outPPXIRIGBParameters.delay_pps_tod_com = ret_value;
      
      sprintf((char *)p_param, "%d\r\n",ret_value);
      
     return OK;   
}

/*****************************************************************************
 * fuction:     out_tod_type_rtrv                                                        
 * description: change the tod type.                                                                  
 * input:    p_param,p_sParameters tod type and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_tod_type_rtrv(u8_t *p_param)
{   
     u8_t tmp = 0;
     
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_TYPE_REG_ADDR))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_TYPE_REG_ADDR, tmp);
              return NG;
     }
     
     if(tmp == 0 || tmp == 1)
        g_sParameters.outPPXIRIGBParameters.tod_clock_type = tmp;
     else
     {
        MTFS30_ERROR("no such name");
          return NG;
     }
     sprintf((char *)p_param, "%d\r\n",tmp);   
     return OK;    
}

/*****************************************************************************
 * fuction:     out_pps_status_rtrv                                                        
 * description: change the pps status.                                                                  
 * input:    p_param,p_sParameters pps status and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_pps_status_rtrv(u8_t *p_param)
{   
     u8_t tmp = 0;
     
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     }
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_PPS_STATE_REG_ADDR))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_PPS_STATE_REG_ADDR, tmp);
              return NG;
     }
     
      
     g_sParameters.outPPXIRIGBParameters.pps_status = tmp;
     
     sprintf((char *)p_param, "%d\r\n",tmp); 
     return OK;       
}

/*****************************************************************************
 * fuction:     out_tod_jitter_rtrv                                                        
 * description: change the pps status.                                                                  
 * input:    p_param,p_sParameters tod jitter and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_tod_jitter_rtrv(u8_t *p_param)
{   
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
    
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_TACC_REG_ADDR))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_TACC_REG_ADDR, tmp);
              return NG;
     }
  
     g_sParameters.outPPXIRIGBParameters.tod_jitter = tmp;
     sprintf((char *)p_param, "%d\r\n",tmp); 
     return OK;       
}

/*****************************************************************************
 * fuction:     out_clock_type_rtrv                                                        
 * description: change the pps clock type.                                                                  
 * input:    p_param,p_sParameters clock type and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_clock_type_rtrv(u8_t *p_param)
{   
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_CLK_TYPE_REG_ADDR))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_CLK_TYPE_REG_ADDR, tmp);
              return NG;
     }
     
     g_sParameters.outPPXIRIGBParameters.tod_clock_type = tmp & 0xc0;
     
     sprintf((char *)p_param, "%d\r\n",tmp & 0xc0);
     return OK;       
}

/*****************************************************************************
 * fuction:     out_clock_status_rtrv                                                        
 * description: change the pps clock status.                                                                  
 * input:    p_param,p_sParameters clock status and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_clock_status_rtrv(u8_t *p_param)
{   
     u16_t ret_value = 0;
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_CLK_STATE_REG_ADDR0))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_CLK_STATE_REG_ADDR0, tmp);
              return NG;
     }
     ret_value |= tmp & 0xff;
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_CLK_STATE_REG_ADDR1))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_CLK_STATE_REG_ADDR1, tmp);
              return NG;
     }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
     g_sParameters.outPPXIRIGBParameters.tod_clock_status = ret_value;
       
     sprintf((char *)p_param, "%d\r\n",ret_value);
     return OK;       
}

/*****************************************************************************
 * fuction:     out_monitor_alarm_rtrv                                                        
 * description: change monitor alarm.                                                                  
 * input:    p_param,p_sParameters monitor alarm and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_monitor_alarm_rtrv(u8_t *p_param)
{   
     u16_t ret_value = 0;
     u8_t tmp = 0;
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_MONITOR_ALARM_REG_ADDR0))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_MONITOR_ALARM_REG_ADDR0, tmp);
              return NG;
     }
     ret_value |= tmp & 0xff;
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_TOD_MONITOR_ALARM_REG_ADDR1))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_TOD_MONITOR_ALARM_REG_ADDR1, tmp);
              return NG;
     }
      ret_value <<= 8;
      ret_value |= tmp & 0xff;
      
     g_sParameters.outPPXIRIGBParameters.tod_moni_alarm = ret_value;
       
     sprintf((char *)p_param, "%d\r\n",ret_value);
     
     return OK;       
}

/*****************************************************************************
 * fuction:     out_signal_type_rtrv                                                       
 * description: change output signal type.                                                                  
 * input:    p_param,p_sParameters signal type and save struct                    
 * ouput:    null                                                    
 * return:   null                                                                                           
 ****************************************************************************/
static u8_t out_singal_type_rtrv(u8_t *p_param)
{   
     u8_t tmp = 0;
     
      /*p_param point null,error*/ 
     if(p_param == NULL)
     {
          MTFS30_ERROR("p_param is null!");
          return NG;
     } 
     
     if (NG == SPI_FPGA_ByteRead(&tmp, OUT_SIGNAL_TYPE_REG_ADDR))
     {
              MTFS30_ERROR("spi read_address:%#x,value:%#x", OUT_SIGNAL_TYPE_REG_ADDR, tmp);
              return NG;
     }
     
     if(tmp >= SIGNAL_TYPE_NUM)
     {
        MTFS30_ERROR("no such name");
        return NG;
     }
     g_sParameters.outPPXIRIGBParameters.signal_type = tmp;
     sprintf((char *)p_param, "%s\r\n",pSignalType[tmp]);
     return OK;       
}
