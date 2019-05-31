/*****************************************************************************/
/* 文件名:    gnss_handler.c                                                 */
/* 描  述:    GNSS相关设置/查询处理                                          */
/* 创  建:    2018-07-20 changzehai(DTT)                                     */
/* 更  新:    无                                                             */
/* Copyright 1998 - 2018 DTT. All Rights Reserved                            */
/*****************************************************************************/
#include "Gnss/gnss_handler.h"
#include "Gnss/gnss_receiver.h"
#include "mtfs30.h"
#include "Debug/user_cmd.h"

#include "Debug/mtfs30_debug.h"
#include "Util/util.h"
#include "BSP/inc/bsp_spi.h"


/*******************************************************************************
* extern varible define                      
*******************************************************************************/
extern gnss_leap_t             g_leap;                /* 闰秒               */
extern gnss_info_t             g_gnss_info;           /* GNSS信息           */
extern u8_t                    g_gnss_init_finished_flag; /* 卫星初始化完成标志 */
extern u8_t                    g_gnss_echo_switch;    /* 卫星语句回显开关   */
extern u8_t                    g_debug_switch;        /* 串口调试开关变量   */
extern u8_t                    g_gnss_cmd_send_flag;      /*  卫星命令发送标志 */

/*******************************************************************************
*const char .
********************************************************************************/
 const char *gnss_help = {
"Control and monitor the gnss module\n"\
"Usage:gnss <command>\r\n"                                                                  \
"Command:\r\n"                                                                              \
"  gnss (m) inquiry about the working mode of satellite receiver\r\n"\
"         (m) config the work mode of satellite receiver,[[MIX]|[GPS]|[BDS]|[GLO]|[GAL]]\r\n" \
"         (p) [priority] in order is the priority of gps,bd,glonass,galileo\r\n"     \
"         (e) [enable|disable]\r\n" \
"       (t) inquiry about the utc time of statellite receiver hhmmss.xx,day,month,year\r\n" \
"       (l) inquiry about the locate information of statellite receiver, hhmmss.xx,day,month,year\r\n"\
"         (s):Query the number of satellite positioning stars\r\n"                          \
"         (t):Query the uct time of satellite receiver,hhmmss.xx\r\n"                       \
"         (i):Query the latitude of satellite positioning,xxxx.xxxxxxxxx,N|S\r\n"           \
"         (o):Query the longitude of satellite positioning,xxxx.xxxxxxxxx,E|W\r\n"          \
"         (a):Query the altitude of satellite positioning\r\n"                              \
"       (p) 1pps status\r\n"                                                                \
"       (a) antenna status\r\n"                                                             \
"       (v) the firm version of satellite receiver\r\n"                                     \
"       (o) the leap value of gps\r\n"                                                      \
"         (p) leap second forecasting hhmmss,day,month,year,next leap,now leap,leap mark\r\n"\
"       (g) inquiry about the satellite information\r\n"\
"         (g) Query about the gps satellite information\r\n"\
"         (b) Query about the bds satellite information\r\n"\
"         (l) Query about the glonass satellite information\r\n"\
"         (a) Query about the galileo satellite information\r\n"\
"       (d) inquiry the compensation of delay information\r\n"\
"         (c) [compensation] the compensation of delay information,range -1000000 ~ 1000000ns\r\n"  
};

const char *gnss_mode[5] = {"MIX", "GPS", "BD", "GLO", "GAL"};

/*****************************************************************************
 * 函  数:    gnss_global_variable_init                                                          
 * 功  能:    初始化GNSS全局变量                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_global_variable_init(void)
{
  
    /* 卫星初始化完成标志 */
    g_gnss_init_finished_flag = 0;
    
    /* 用户发送卫星模块命令标志 */
    g_gnss_cmd_send_flag = 0;
    
    /* 卫星语句回显开关 */
    g_gnss_echo_switch = 0;
    
    /* 闰秒 */
    g_leap.now_leap = 18; /* 当前闰秒 */
    g_leap.next_leap = 18; /* 下次闰秒 */
    g_leap.sign = 0;      /* 0： 无闰秒预告; 1:正闰秒; 2：负闰秒 */
    
    
    /* GNSS工作模式 */
    strcpy((char *)g_gnss_info.mode.info, "MIX;"); 
//    g_rev_info.ac_mode = REV_ACMODE_MIX;
    
    /* 闰秒信息 */
    strcpy((char *)g_gnss_info.leap.info, "235960,31,12,2016,18,18,00;");
    
    /* 时间信息 */
    strcpy((char *)g_gnss_info.time.info, "000000.00,00,00,0000;");
    
    /* 定位信息 */
    strcpy((char *)g_gnss_info.loc.info, "00,000000.00,0000.00000000,N,00000.00000000,E,000.0000;");
    
    /* 卫星信息 */
    //memset(&g_gnss_info.sat, 0x00, sizeof(g_gnss_info.sat));
    strcpy((char *)g_gnss_info.sat.info, "GPS,00;BDS,00;GLO,00;GAL,00;");
    g_gnss_info.sat.len = 0;    
    
    /* 1PPS状态信息 */
    strcpy((char *)g_gnss_info.pps.info, "0;");  
    
    /* 天线状态信息 */
    strcpy((char *)g_gnss_info.ant.info, "ON;");
    
    /* 版本信息 */
    strcpy((char *)g_gnss_info.ver.info, "UT4B0,000000000000000;");  
    
    /* leap 下发*/
    
    system_set_leap(g_leap.now_leap);

}




/*****************************************************************************
 * 函  数:    gnss_info_print                                                         
 * 功  能:    打印GNSS信息                                                               
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_info_print(void)
{
    u8_t buf[400];
    
    /* 打印GNSS工作模式信息 */
    gnss_receiver_get_info("MODE", buf);
    MTFS30_TIPS("GNSS工作模式: %s", buf);
    
    /* 打印时间信息 */
    gnss_receiver_get_info("TIME", buf);
    MTFS30_TIPS("时间信息: %s", buf);    
    
    /* 打印定位信息 */
    gnss_receiver_get_info("LOC", buf);
    MTFS30_TIPS("定位信息: %s", buf);
    
    /* 打印卫星信息 */
    gnss_receiver_get_info("SAT", buf);
    MTFS30_TIPS("卫星信息: %s", buf);
    
    /* 打印闰秒信息 */
    gnss_receiver_get_info("LEAP", buf);
    MTFS30_TIPS("闰秒信息: %s", buf);
    
    /* 打印1PPS状态信息 */
    gnss_receiver_get_info("PPS", buf);
    MTFS30_TIPS("1PPS状态信息: %s", buf);
    
    /* 打印天线状态信息 */
    gnss_receiver_get_info("ANT", buf);
    MTFS30_TIPS("天线状态信息: %s", buf);
    
    /* 打印GNSS版本信息 */
    gnss_receiver_get_info("VER", buf);
    MTFS30_TIPS("GNSS版本信息: %s", buf);

    MTFS30_TIPS("\n");
}


/*****************************************************************************
 * 函  数:    gnss_stmt_echo                                                         
 * 功  能:    卫星语句回显                                                               
 * 输  入:    p_msg: 指向卫星语句信息                         
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
void gnss_stmt_echo(u8_t *p_msg)
{
    u8_t buf[200] = {0};
    u8_t *p = p_msg;
    u8_t *q = NULL;

    
    /* 输出每一条卫星语句 */
    while(*p != '\0')
    {    
        q = buf;
//        while((*q++ = *p++) != '');
        while(*p != '\n')
        {
            *q++ = *p++;
        }
        *q = '\0';
        p++;
        
        MTFS30_TIPS(buf);

    }
    
}

/*****************************************************************************
 * 函  数:    gnss_init                                                           
 * 功  能:    GNSS模块初始化                                                                  
 * 输  入:    无                          
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/ 
void gnss_init(void)
{
    /* GNSS接收机初始化 */
    gnss_receiver_init();
}



/*****************************************************************************
 * 函  数:    gnss_acmode_set_handler                                                           
 * 功  能:    配置接收机工作模式                                                                
 * 输  入:    p_param  : 命令参数                      
 * 输  出:    无                                                    
 * 返回值:    OK: 设置成功； NG:设置失败                                                   
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/  
u8_t gnss_acmode_set_handler(int num,...)
{
    u8_t *p_param = NULL;
    u8_t  err_flag = OK;
    va_list valist;
    
    va_start(valist, num);
    p_param = (u8_t *)va_arg(valist, char *);
    
    if(p_param != NULL)
        err_flag = gnss_receiver_acmode_set(p_param);
    
    else
        err_flag = NG;

    va_end(valist);
    
    return err_flag;
   // return gnss_receiver_acmode_set(p_param);
}

/*****************************************************************************
 * 函  数:    gnss_serial baud_set_handler                                                           
 * 功  能:    配置卫星串口波特率                                                                
 * 输  入:    p_param  : 命令参数
 *            presult: 设置结果标志(成功|失败)                         
 * 输  出:    无                                                    
 * 返回值:    OK: 设置成功; NG: 设置失败                                                
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t gnss_serial_baud_set_handler(int num,...)
{
    u8_t *p_param = NULL;
    u8_t  err_flag = OK;
    va_list valist;
    
    va_start(valist, num);
    p_param = (u8_t *)va_arg(valist, char *);
    
    if(p_param != NULL)
      err_flag = gnss_receiver_serial_baud_set(p_param);
    va_end(valist);
    
    return err_flag;
}

u8_t gnss_helper(int num,...)
{
    u8_t *p_param[MAX_PARAM_NUM] = NULL;
    u8_t  err_flag = OK;
    u8_t  inx;
    va_list valist;
    
    va_start(valist, num);
    for(inx = 0; inx < num; inx++)
    {
        p_param[inx] = (u8_t *)va_arg(valist, char *);
        if(p_param[inx] == NULL)
          err_flag = NG;
    }
    va_end(valist);
    
    if(err_flag == OK)
    if(0== strncmp((char const*)p_param[0], "help", strlen("help")))
    {
      //strncpy(p_param[1], gnss_help, strlen(gnss_help) );
      DEBUG_USART_Send((u8_t *)gnss_help, strlen(gnss_help));
    }
    
    return err_flag;
    //return gnss_receiver_serial_baud_set(p_param);
}


u8_t gnss_command_execute(int num,...)
{
    u8_t *p_param[MAX_PARAM_NUM] = NULL;
    u8_t tmp_buf[GNSS_STATELLITE_INFO_LEN];
    u8_t  err_flag = OK;
    u8_t  inx;
    
    va_list valist;
    
    va_start(valist, num);
    for(inx = 0; inx < num; inx++)
    {
        p_param[inx] = (u8_t *)va_arg(valist, char *);
        if(p_param[inx] == NULL)
          err_flag = NG;
    }
    va_end(valist);
    
    if(err_flag == OK)
    {
      
       if(0 == strncmp((char const*)p_param[0], "help", strlen("help")))
       {
          DEBUG_USART_Send((u8_t *)gnss_help, strlen(gnss_help));
          goto return_mark; 
       }
       if(num <= GNSS_COMMAND_RTRV)
       {
          switch(*p_param[0])
          {
               case 'm':
               {
                     if(*p_param[1] == 'm')
                     { 
                         inx = g_sParameters.GnssParameters.mode;
                         sprintf((char *)p_param[num -1], "%s\r\n", gnss_mode[inx-1]); 
                     }
                     else if(*p_param[1] == 'p')
                          sprintf((char *)p_param[num -1], "%s\r\n", g_sParameters.GnssParameters.priority); 

                     else if(*p_param[1] == 'e')
                          sprintf((char *)p_param[num -1], "%d\r\n", g_sParameters.GnssParameters.enable); 
                     
                     else
                          err_flag = NG;
               }
               break;
               
               case 't':
               {
                    if(num == GNSS_COMMAND_RTRV -1)
                    {
                         memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                         get_gnss_info("TIME", '\0', ";", tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                    }
                    else
                         err_flag = NG;  
               }
               break;
               
               case 'l':
                 {
                     memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                     if(num == GNSS_COMMAND_RTRV -1)
                     {
                         gnss_receiver_get_info("LOC", tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                     }
                     else
                     {
                         switch(*p_param[1])
                         {
                             case 's':
                               {
                                 get_gnss_info("LOC", '\0', ",", tmp_buf);
                                 sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                               }
                             break;
                             
                             case 't':
                               { 
                                 get_gnss_info("TIME", '\0', ";", tmp_buf);
                                 sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                                 
                               }
                             break;
                             
                             case 'i':
                               {
                                 split_gnss_info("LOC", ',', ',', 2, 4, tmp_buf);
                                 sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                               }
                              break;
                              
                             case 'o':
                               {
                                 split_gnss_info("LOC", ',', ',', 4, 6, tmp_buf);
                                 sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                               }
                             break;
                             
                             case 'a':
                               {
                                 get_gnss_info("LOC", ',', ";", tmp_buf);
                                 sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf); 
                               }
                             break;
                             
                             default:
                               {
                                  err_flag = NG;
                               }
                             break;
                        }
                         
                     }
                 }
                 break;
                 
                case 'p':
                 {
                   if(num == GNSS_COMMAND_RTRV -1)
                   {
                         memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                         get_gnss_info("PPS", '\0', ";", tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else
                         err_flag = NG; 
                 }
                break;
               
                case 'a':
                 {
                   if(num == GNSS_COMMAND_RTRV -1)
                   {
                         memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                         get_gnss_info("ANT", '\0', ";", tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else 
                     err_flag = NG;
                 }
                break;
                
                case 'v':
                 {
                   if(num == GNSS_COMMAND_RTRV -1)
                   {
                         memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                         get_gnss_info("VER", '\0', ";", tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else 
                     err_flag = NG;
                 }
                break;
                
                case 'o':
                 {
                   memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                   if(num == GNSS_COMMAND_RTRV -1)
                   {
                         split_gnss_info("LEAP", ',', ',', 5, 6, tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else if(*p_param[1] == 'p')
                   {
                          gnss_receiver_get_info("LEAP", tmp_buf);
                          sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else
                          err_flag = NG;
                 }
                break;
                
                case 'g':
                 {
                   memset(tmp_buf, '\0', GNSS_STATELLITE_INFO_LEN);
                   if(num == GNSS_COMMAND_RTRV -1)
                   {
                         gnss_receiver_get_info("SAT", tmp_buf);
                         sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else if(*p_param[1] == 'g')
                   {
                          split1_gnss_info("SAT", "GPS", ';',tmp_buf);
                          sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else if(*p_param[1] == 'b')
                   {
                          split1_gnss_info("SAT", "BDS", ';',tmp_buf);
                          sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else if(*p_param[1] == 'l')
                   {
                          split1_gnss_info("SAT", "GLO", ';',tmp_buf);
                          sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else if(*p_param[1] == 'a')
                   {
                          split1_gnss_info("SAT", "GAL", ';',tmp_buf);
                          sprintf((char *)p_param[num -1], "%s\r\n", tmp_buf);
                   }
                   else
                          err_flag = NG;
                 }
                  break;
                 
               case 'd':
                 {
                   int dlycom = 0;
                   if(num == GNSS_COMMAND_RTRV -1 || 
                                                *p_param[1] == 'c')
                   {
                         dlycom = gnss_get_gps_pps_offset();
                         sprintf((char *)p_param[num -1], "%d\r\n", dlycom);
                   }
                   else
                         err_flag = NG;
                 }
                  break;
               
               default:
                    err_flag = NG;
                   
                  break;
                
       }
    }
    else if(num == GNSS_COMMAND_SET)
    {
        switch(*p_param[0])
        {
          case 'm':
          {
            if(*p_param[1] == 'm')
            {
                   if(p_param[2] != NULL)
                      err_flag = gnss_acmode_set_handler(1, p_param[2]);
                   else
                   {
                      err_flag = NG;
                      MTFS30_ERROR("p_param[2] is null!");
                   }
            }
            else if(*p_param[1] == 'p')
            {
                    if(p_param[2] != NULL && strlen((char const *)p_param[2]) == GNSS_PRIORITY_LEN)
                    {
                       sprintf((char *)g_sParameters.GnssParameters.priority,"%s",(char *)p_param[2]);
                       g_sParameters.GnssParameters.priority[GNSS_PRIORITY_LEN] = '\0';
                    }
                    else
                    {
                       err_flag = NG;
                       MTFS30_ERROR("p_param[2] is null!");
                    }
                    
            }
            else if(*p_param[1] == 'e')
            {
              if(p_param[2] != NULL)
              { 
                  if(0 == strncmp((char *)p_param[2], "enable", strlen("enable")))
                     g_sParameters.GnssParameters.enable = 1;
                  
                  else if(0 == strncmp((char *)p_param[2], "disable", strlen("disable")))
                     g_sParameters.GnssParameters.enable = 0;
                  
                  else
                     err_flag = NG;
              }
              else
              {
                   err_flag = NG;
                   MTFS30_TIPS("参数(%s)有误!\n", p_param[2]);
              }
            }
            else 
            {
                  err_flag = NG;
                  MTFS30_ERROR("p_param[2] is null!");
            }
            
          }
          break;
          
          case 'd':
          {
            int delaycom = 0;
            if(*p_param[1] == 'c')
            {
              if(p_param[2] != NULL)
              {
                  delaycom = atoi((char *)p_param[2]);
                  err_flag = gnss_set_gps_pps_offset(delaycom);
              }
              else
              {
                   MTFS30_ERROR("p_param[2] is null!");
                  err_flag = NG;
              }
            }
            else
                err_flag = NG;
          }
          break; 
        default:
          err_flag = NG;
          break;
        }
    }
  }

return_mark:   
    return err_flag;
    //return gnss_receiver_serial_baud_set(p_param);
}

/*****************************************************************************
 * 函  数:    gnss_echo_set_handler                                                           
 * 功  能:    打开/关闭指定语句的回显功能                                                                
 * 输  入:    p_param  : 命令参数                 
 * 输  出:    无                                                    
 * 返回值:    OK: 设置成功; NG: 设置失败                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t gnss_echo_set_handler(int num,...)
{  
    u8_t *p_param = NULL;
    u8_t  err_flag = OK;
    va_list valist;
    
    va_start(valist, num);
    p_param = (u8_t *)va_arg(valist, char *);
    
    if(p_param != NULL)
    {
      if (0 == strncmp((char const *)p_param, "OFF", strlen("OFF")))
      {
          /* 关闭循环打印 */
          g_gnss_echo_switch = 0;
      }
      else if (0 == strncmp((char const *)p_param, "ON", strlen("ON")))
      {
          /* 打开循环打印 */
          g_gnss_echo_switch = 1;
      }
      else
      {
          MTFS30_TIPS("参数(%s)有误!\n", p_param);  
          err_flag = NG;
      }
    }
    va_end(valist);
    
    return err_flag;
    //return gnss_receiver_echo_set(p_param);
}


/*****************************************************************************
 * 函  数:    gnss_stmt_set_handler                                                           
 * 功  能:    打开/关闭GNSS板卡输出指定语句                                                               
 * 输  入:    p_param  : 命令参数
 * 输  出:    无                                                    
 * 返回值:    OK: 设置成功; NG: 设置失败                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t gnss_stmt_set_handler(int num,...)
{
    u8_t *p_param = NULL;
    u8_t  err_flag = OK;
    va_list valist;
    
    va_start(valist, num);
    p_param = (u8_t *)va_arg(valist, char *);
    
    if(p_param != NULL)
      err_flag = gnss_receiver_stmt_set(p_param);
    va_end(valist);
    
    return err_flag;
    //return gnss_receiver_stmt_set(p_param);
}



/*****************************************************************************
 * 函  数:    gnss_get_handler                                                           
 * 功  能:    查询GNSS模块指定信息                                                               
 * 输  入:    p_param : 命令参数           
 * 输  出:    p_rtrv: 查询结果                                           
 * 返回值:    OK: 查询成功; NG: 查询失败                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                   
 ****************************************************************************/
u8_t gnss_get_info_handler(u8_t *p_param, u8_t *p_rtrv)
{
    return gnss_receiver_get_info(p_param, p_rtrv);
}

/*****************************************************************************
 * funtion:       gnss_inquery_set                                                        
 * description:   description gnss help information  and gnss receiver information                                                              
 * parameters:    command1,command2, tmp_buffer                          
 * return :       inquery result                                                                                          
 ****************************************************************************/
u8_t gnss_inquery_set(u8_t *p_param, u8_t *p_rtrv)
{
    return gnss_receiver_get_info(p_param, p_rtrv);
    
}
/*****************************************************************************
 * 函  数:    gnss_set_gps_pps_offset                                                     
 * 功  能:    设置输入gps信号的延时补偿                                                             
 * 输  入:    offset: 延时补偿                     
 * 输  出:    无                                                    
 * 返回值:    无                                                    
 * 创  建:    2018-07-18 changzehai(DTT)                            
 * 更  新:    无                                       
 ****************************************************************************/
u8_t gnss_set_gps_pps_offset(int offset)
{   
     u8_t tmp = 0;
     int value = 0;
     
     if(offset > GNSS_PPS_DLYCOM_HI || offset < GNSS_PPS_DLYCOM_LO)
              return NG;
     
     g_sParameters.GnssParameters.delaycom = offset;
     value = abs(offset) /8;
     if(offset < 0) 
       value |= 0x08000000;
     
     
     tmp = (value & 0x0F000000) >> 24;
     if (NG ==SPI_FPGA_ByteWrite(tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR0))
     {
         MTFS30_ERROR("spi write_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR0, tmp);
         return NG;
     }
     
     tmp = (value & 0xFF0000) >> 16;
     if (NG ==SPI_FPGA_ByteWrite(tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR1))
     {
         MTFS30_ERROR("spi write_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR1, tmp);
         return NG;
     } 
  
     tmp = (value & 0xFF00) >> 8;
     if (NG ==SPI_FPGA_ByteWrite(tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR2))
     {
         MTFS30_ERROR("spi write_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR2, tmp);
         return NG;
     } 
     
     tmp = value & 0xFF;
     if (NG ==SPI_FPGA_ByteWrite(tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR3))
     {
         MTFS30_ERROR("spi write_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR3, tmp);
         return NG;
     }  
      
     return OK;
        
}

int gnss_get_gps_pps_offset(void)
{   
     u8_t tmp = 0;
     u8_t ret_sign = 0;
     int  ret_value = 0;
     
     
     SPI_FPGA_ByteRead(&tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR0);
     ret_sign = (tmp & 0x08)? 1 : 0;
     ret_value |= tmp & 0x07;
     
     
     if(NG == SPI_FPGA_ByteRead(&tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR1))
     {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR1, tmp);
          return NG;
     }
     ret_value <<= 8;
     ret_value |= tmp & 0xff;
     
     if(NG == SPI_FPGA_ByteRead(&tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR2))
     {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR2, tmp);
          return NG;
     }
     ret_value <<= 8;
     ret_value |= tmp & 0xff;
     
     if(NG == SPI_FPGA_ByteRead(&tmp, GNSS_GPS_PPS_OFFSET_REG_ADDR3))
     {
          MTFS30_ERROR("spi read_address:%#x,value:%#x", GNSS_GPS_PPS_OFFSET_REG_ADDR3, tmp);
          return NG;
     }
     ret_value <<= 8;
     ret_value |= tmp & 0xff;
     
     ret_value = ret_sign? (-ret_value * 8):(ret_value * 8);
     g_sParameters.GnssParameters.delaycom = ret_value;
     return ret_value;
}

/*check pps status and pps loss alarm. serial alarm */
void check_gnss_available(void)
{
  u8_t tmp = 0;
  static u8_t avail_count = 0;
  static u8_t unavail_count = 0;
  u8_t alarm_tmp = GNSS_PPS_LOSS_BIT | GNSS_SER_LOSS_BIT;
  
  /* check gnss enable status,60 seconds filter*/
  if(g_sParameters.GnssParameters.enable == 1 &&
                            OK == SPI_FPGA_ByteRead(&tmp, GNSS_ALARM_REG_ADDR) &&
                    (tmp & alarm_tmp) == 0x00 && g_gnss_info.pps.info[0] == '3' )
   {
          unavail_count = 0;
       
          if(avail_count < GNSS_PPS_AVAIL_FITER)
             avail_count++;
          else
          {
            SPI_FPGA_ByteWrite(available, GNSS_PPS_AVAIL_REG_ADDR);
            LVL_DEBUG("gnss available!\n",DBG_NOTICE);
          }
 
   }
  else
  {
          avail_count = 0;
          if(unavail_count < GNSS_PPS_AVAIL_FITER)
            unavail_count++;
          else
          {
            SPI_FPGA_ByteWrite(unavailable, GNSS_PPS_AVAIL_REG_ADDR);
            LVL_DEBUG("gnss unavailable!\n",DBG_NOTICE);
          }
  }
}